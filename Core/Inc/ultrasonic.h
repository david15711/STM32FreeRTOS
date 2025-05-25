/*
 * ultrasonic.h
 *
 *  Created on: May 9, 2025
 *      Author: MS
 */

#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "hw_def.h"

typedef struct {
  TIM_TypeDef TIM;    //TIM1 ch: PA 8 9 10, 11 TIM4 ch: PB 6 7 8 9
  TIM_HandleTypeDef *htim;
  uint32_t pwm_channel;
  uint32_t ic1_channel;
  uint32_t ic2_channel;
  uint32_t APB_clock;
  uint16_t trig_pin;
  uint16_t echo_pin;
} ultrasonicInit_t;

bool ultrasonicInit(void);
uint32_t ultrasnoicGetDistanceTemperture(uint32_t start, uint32_t end, float temp);

#endif /* INC_ULTRASONIC_H_ */
