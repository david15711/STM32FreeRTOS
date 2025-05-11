/*
 * ap.c
 *
 *  Created on: May 2, 2025
 *      Author: MS
 */
#include "ap.h"


osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myIoTask */
osThreadId_t myIoTaskHandle;
const osThreadAttr_t myIoTask_attributes = {
  .name = "myIoTask",
  .stack_size = 64 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
#if 0
/* Definitions for myDistanceTask */
osThreadId_t myDistanceTaskHandle;
const osThreadAttr_t myDistanceTask_attributes = {
  .name = "myDistanceTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myIPCQueue01 */
osMessageQueueId_t myIPCQueue01Handle;
const osMessageQueueAttr_t myIPCQueue01_attributes = {
  .name = "myIPCQueue01"
};
/* Definitions for myIPCQueue02 */
osMessageQueueId_t myIPCQueue02Handle;
const osMessageQueueAttr_t myIPCQueue02_attributes = {
  .name = "myIPCQueue02"
};

/* Definitions for myICEvent01 */
osEventFlagsId_t myICEvent01Handle;
const osEventFlagsAttr_t myICEvent01_attributes = {
  .name = "myICEvent01"
};
#endif
/* Definitions for myADCSem01 */
osSemaphoreId_t myADCSem01Handle;
const osSemaphoreAttr_t myADCSem01_attributes = {
  .name = "myADCSem01"
};
/* Definitions for myIPCQueue03 */
osMessageQueueId_t myIPCQueue03Handle;
const osMessageQueueAttr_t myIPCQueue03_attributes = {
  .name = "myIPCQueue03"
};

uint32_t adc_value[3];
ultrasonicConfig_t sonic1;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
//void StartTask03(void *argument);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myADCSem01Handle);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t rawReceive, rawEnd;
	IPCmessage_t IPC3queue;
	if(htim == &htim1){
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			rawReceive = htim->Instance->CCR2;
			memcpy(&IPC3queue.msg[0], &rawReceive, sizeof(uint32_t));
			IPC3queue.msg_type = 1;
			//osEventFlagsSet(myICEvent01Handle, 1);
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			rawEnd = htim->Instance->CCR3;
			memcpy(&IPC3queue.msg[0], &rawEnd, sizeof(uint32_t));
			IPC3queue.msg_type = 2;
			//osEventFlagsSet(myICEvent01Handle, 2);
		}
		osMessageQueuePut(myIPCQueue03Handle, &IPC3queue, 0, 0);
	}
}

void apInit(void)
{
	uartOpen(_DEF_UART2, 115200);
	ultrasonicInit();
	osKernelInitialize();
	myADCSem01Handle = osSemaphoreNew(1, 1, &myADCSem01_attributes);
	//myIPCQueue01Handle = osMessageQueueNew (8, 8, &myIPCQueue01_attributes);
	//myIPCQueue02Handle = osMessageQueueNew (8, 8, &myIPCQueue02_attributes);
	myIPCQueue03Handle = osMessageQueueNew (8, 8, &myIPCQueue03_attributes);
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
	myIoTaskHandle = osThreadNew(StartTask02, NULL, &myIoTask_attributes);
	//myDistanceTaskHandle = osThreadNew(StartTask03, NULL, &myDistanceTask_attributes);
	//myICEvent01Handle = osEventFlagsNew(&myICEvent01_attributes);
}

void apMain(void)
{
	  osKernelStart();
	  while(1)
	  {

	  }
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
	uint32_t adc[3];
	uint32_t distance = 0, start = 0, end = 0;
	IPCmessage_t IPC1queue, IPC2queue;
	float temperature = 0;
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3);
  /* Infinite loop */
	for(;;)
	{
		//main thread, run ADC_DMA, wait for ADC_DMA cb semaphore, put/get IPC message, send UART
		HAL_ADC_Start_DMA(&hadc1, adc_value, 3);
		osSemaphoreAcquire(myADCSem01Handle, 100);
		adc[0] = *(uint16_t*)adc_value;
		temperature = temperatureRun(adc[0]);
		adc[1] = *( ( (uint16_t*)adc_value ) + 1 );
		adc[2] = *( ( (uint16_t*)adc_value ) + 2 );
		//memcpy(&IPC1queue.msg[0], &adc[0], sizeof(uint16_t));
		//osMessageQueuePut(myIPCQueue01Handle, &IPC1queue, 0, 100);




		osMessageQueueGet(myIPCQueue03Handle, &IPC2queue, 0, 100);

		if(IPC2queue.msg_type == 1)
		{
			memcpy(&start, &IPC2queue.msg[0], sizeof(uint32_t));
		}
		else if (IPC2queue.msg_type == 2)
		{
			memcpy(&end, &IPC2queue.msg[0], sizeof(uint32_t));
		}
		distance = ultrasnoicGetDistanceTemperture(start, end, temperature);
		//memcpy(&distance, &IPC2queue.msg[0], sizeof(uint16_t));
		uartPrintf(_DEF_UART2, "raw ADC: %lu, %lu, %lu, %lu\r\n", (uint32_t)temperature, adc[1], adc[2], distance);
		osDelay(1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myUltrasonicTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	  osDelay(500);
  }
  /* USER CODE END StartTask02 */
}

#if 0
/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myIOTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
	/* USER CODE BEGIN StartTask03 */
	IPCmessage_t IPC1queue, IPC2queue, IPC3queue;
	uint32_t start, end, distance;
	float temperature;
	/* Infinite loop */
	for(;;)
	{
		//TODO: event handle, temperature IPC message get, get distance, IPC send
		while(osMessageQueueGet(myIPCQueue01Handle, &IPC1queue, NULL, 0) == osOK)
		{
			memcpy(&temperature, &IPC1queue.msg[0], sizeof(float));
		}
		osEventFlagsWait(myICEvent01Handle, 3, 1, 100);
		while( osMessageQueueGet(myIPCQueue03Handle, &IPC3queue, NULL, 0) == osOK)
		{
			if(IPC3queue.msg_type == 1)
			{
				memcpy(&start, &IPC3queue.msg[0], sizeof(uint32_t));
			}
			else if (IPC3queue.msg_type == 2)
			{
				memcpy(&end, &IPC3queue.msg[0], sizeof(uint32_t));
			}
		}
		distance = ultrasnoicGetDistanceTemperture(start, end, temperature);
		memcpy(&IPC2queue.msg[0], &distance, sizeof(uint32_t));
		IPC2queue.msg_type = 0;
		osMessageQueuePut(myIPCQueue02Handle, &IPC2queue, 0, 0);
		osDelay(1);
	}
  /* USER CODE END StartTask03 */
}
#endif
