/*
 * ap.c
 *
 *  Created on: May 2, 2025
 *      Author: MS
 */
#include "ap.h"


/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myUARTTask */
osThreadId_t myUARTTaskHandle;
const osThreadAttr_t myUARTTask_attributes = {
  .name = "myUARTTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myIOTask */
osThreadId_t myIOTaskHandle;
const osThreadAttr_t myIOTask_attributes = {
  .name = "myIOTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myIPCQueue01 */ //Main task to UltrasonicTask
osMessageQueueId_t myIPCQueue01Handle;
const osMessageQueueAttr_t myIPCQueue01_attributes = {
  .name = "myIPCQueue01"
};
/* Definitions for myIPCQueue02 */ //sonic to main task
osMessageQueueId_t myIPCQueue02Handle;
const osMessageQueueAttr_t myIPCQueue02_attributes = {
  .name = "myIPCQueue02"
};
/* Definitions for myIPCQueue03 */ //IC Callback to sonic task
osMessageQueueId_t myIPCQueue03Handle;
const osMessageQueueAttr_t myIPCQueue03_attributes = {
  .name = "myIPCQueue03"
};
/* Definitions for myUARTSem01 */
osSemaphoreId_t myUARTSem01Handle;
const osSemaphoreAttr_t myUARTSem01_attributes = {
  .name = "myUARTSem01"
};
/* Definitions for myICEvent01 */
osEventFlagsId_t myICEvent01Handle;
const osEventFlagsAttr_t myICEvent01_attributes = {
  .name = "myICEvent01"
};

uint32_t adc_value[2];
ultrasonicConfig_t sonic1;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim1;

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myUARTSem01Handle);
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
			osEventFlagsSet(myICEvent01Handle, 1);
		}
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			rawEnd = htim->Instance->CCR3;
			memcpy(&IPC3queue.msg[0], &rawEnd, sizeof(uint32_t));
			IPC3queue.msg_type = 2;
			osEventFlagsSet(myICEvent01Handle, 2);
		}
		osMessageQueuePut(myIPCQueue03Handle, &IPC3queue, 0, 0);
	}
}

void apInit(void)
{
	uartOpen(_DEF_UART2, 115200);
	ultrasonicInit();
	osKernelInitialize();
	myUARTSem01Handle = osSemaphoreNew(1, 1, &myUARTSem01_attributes);
	myIPCQueue01Handle = osMessageQueueNew (8, 8, &myIPCQueue01_attributes);
	myIPCQueue02Handle = osMessageQueueNew (8, 8, &myIPCQueue02_attributes);
	myIPCQueue03Handle = osMessageQueueNew (8, 8, &myIPCQueue03_attributes);
	myICEvent01Handle = osEventFlagsNew(&myICEvent01_attributes);
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
	myUARTTaskHandle = osThreadNew(StartTask02, NULL, &myUARTTask_attributes);
	myIOTaskHandle = osThreadNew(StartTask03, NULL, &myIOTask_attributes);
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
	uint32_t adc[2], distance;
	IPCmessage_t IPC1queue, IPC2queue;
  /* Infinite loop */
	for(;;)
	{
		//main thread, run ADC_DMA, wait for ADC_DMA cb semaphore, put/get IPC message, send UART
		HAL_ADC_Start_DMA(&hadc1, adc_value, 2);
		osSemaphoreAcquire(myUARTSem01Handle, 100);
		adc[0] = adc_value[0];
		adc[1] = adc_value[1];
		memcpy(&IPC1queue.msg[0], &adc[0], sizeof(uint32_t));
		osMessageQueuePut(myIPCQueue01Handle, &IPC1queue, 0, 100);
		osMessageQueueGet(myIPCQueue02Handle, &IPC2queue, 0, 100);
		memcpy(&distance, &IPC2queue.msg[0], sizeof(uint32_t));
		uartPrintf(_DEF_UART2, "raw ADC: %lu, %lu, %lu\r\n", adc[0], adc[1], distance);
		osDelay(1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myUARTTask thread.
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
