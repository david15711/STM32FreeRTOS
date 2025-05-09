/*
 * temperature.h
 *
 *  Created on: May 9, 2025
 *      Author: MS
 */

#ifndef INC_TEMPERATURE_H_
#define INC_TEMPERATURE_H_

#include "hw_def.h"

bool temperauteInit(void);
float temperatureRun(uint32_t adc_value);

#endif /* INC_TEMPERATURE_H_ */
