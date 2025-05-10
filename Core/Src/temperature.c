/*
 * temperature.c
 *
 *  Created on: May 9, 2025
 *      Author: MS
 */

#include "temperature.h"

#ifdef _USE_HW_TEMPERATURE

static const float VREF = 3.3;
static const float V25degC = 1.43;
static const float avg_slope = 0.0043;
static bool is_init;

bool temperauteInit(void)
{
	is_init = true;
	return true;
}

float temperatureRun(uint32_t adc_value)
{
    float VSense = (adc_value * VREF) / 4096.0;  // ADC 값을 전압(mV)으로 변환
    float temperature = ((V25degC - VSense) / avg_slope) + 25.0;  // 온도 변환
    return temperature;
}

#endif
