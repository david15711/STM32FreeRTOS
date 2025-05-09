/*
 * ap.c
 *
 *  Created on: May 2, 2025
 *      Author: MS
 */
#include "ap.h"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	;
}

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
/* Definitions for myIPCQueue03 */
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

ultrasonicConfig_t sonic1;

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);

void apInit(void)
{
	uartOpen(_DEF_UART2, 115200);
	ultrasonicInit();
	osKernelInitialize();
	myUARTSem01Handle = osSemaphoreNew(1, 1, &myUARTSem01_attributes);
	myIPCQueue01Handle = osMessageQueueNew (8, 8, &myIPCQueue01_attributes);
	myIPCQueue02Handle = osMessageQueueNew (8, 8, &myIPCQueue02_attributes);
	myIPCQueue03Handle = osMessageQueueNew (8, 8, &myIPCQueue03_attributes);
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
	myUARTTaskHandle = osThreadNew(StartTask02, NULL, &myUARTTask_attributes);
	myIOTaskHandle = osThreadNew(StartTask03, NULL, &myIOTask_attributes);
	myICEvent01Handle = osEventFlagsNew(&myICEvent01_attributes);
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
  /* Infinite loop */
  for(;;)
  {
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
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}
