/*
 * uart.c
 *
 *  Created on: 2020. 12. 8.
 *      Author: baram
 */


#include "uart.h"
#include "qbuffer.h"

#ifdef _USE_HW_UART


static bool is_open[UART_MAX_CH];
static qbuffer_t qbuffer[UART_MAX_CH];
static uint8_t rx_buf[256];

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    is_open[i] = false;
  }

  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;

  switch(ch)
  {
    case _DEF_UART2:
		huart2.Instance = USART2;
		huart2.Init.BaudRate = 115200;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		huart2.Init.OverSampling = UART_OVERSAMPLING_16;

      qbufferCreate(&qbuffer[ch], rx_buf, 256);

	  if (HAL_UART_Init(&huart2) != HAL_OK)
	  {
		ret = false;
	  }
      else
      {
        ret = true;
        is_open[ch] = true;

        if(HAL_UART_Receive_DMA(&huart2, rx_buf, 256) != HAL_OK)
        {
          ret = false;
        }
        qbuffer[ch].in  = qbuffer[ch].len - hdma_usart2_rx.Instance->CNDTR;
        qbuffer[ch].out = qbuffer[ch].in;
      }
      break;
  }

  return ret;
}

bool uartClose(uint8_t ch)
{
  return true;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case _DEF_UART2:
      qbuffer[ch].in = (qbuffer[ch].len - hdma_usart2_rx.Instance->CNDTR);
      ret = qbufferAvailable(&qbuffer[ch]);
      break;
  }

  return ret;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART2:
      qbufferRead(&qbuffer[_DEF_UART2], &ret, 1);
      break;
  }
  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  HAL_StatusTypeDef status;

  switch(ch)
  {
    case _DEF_UART2:
      status = HAL_UART_Transmit(&huart2, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length;
      }
      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
  char buf[256];
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args);

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  switch(ch)
  {
    case _DEF_UART2:
      ret = huart2.Init.BaudRate;
      break;
  }

  return ret;
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART2)
  {
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
#if 0
  if (huart->Instance == USART2)
  {
    qbufferWrite(&qbuffer[_DEF_UART2], &rx_data[_DEF_UART2], 1);

    HAL_UART_Receive_IT(&huart2, (uint8_t *)&rx_data[_DEF_UART2], 1);
  }
#endif
}

#endif
