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
	TIM_TypeDef TIM;
	TIM_HandleTypeDef *htim;   // 타이머 핸들
	uint32_t trig_channel;         // 트리거용 PWM 채널
	uint32_t reic_channel;         // Rising Edge Input Capture 채널
	uint32_t feic_channel;         // Falling Edge Input Capture 채널
	uint32_t timer_clock;          // 타이머 클록 (Hz 단위), tim1이냐 2냐에 따라 인가되는 클락에서 분주하여 구함

	GPIO_TypeDef *trig_port;       // 트리거 핀 포트
	uint16_t trig_pin;             // 트리거 핀 번호
	GPIO_TypeDef *echo1_port;      // 에코1 핀 포트
	uint16_t echo1_pin;
	GPIO_TypeDef *echo2_port;      // 에코2 핀 포트
	uint16_t echo2_pin;

} ultrasonicConfig_t;

bool ultrasonicInit(void);
uint32_t ultrasnoicGetDistanceTemperture(uint32_t start, uint32_t end, float temp);

#endif /* INC_ULTRASONIC_H_ */
