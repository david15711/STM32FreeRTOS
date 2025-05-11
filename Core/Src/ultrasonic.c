/*
 * ultrasonic.c
 *
 *  Created on: May 9, 2025
 *      Author: MS
 */

#include "ultrasonic.h"
#include "temperature.h"

#ifdef _USE_HW_ULTRASONIC

static bool is_running;

bool ultrasonicInit(void)
{
	is_running = true;
	return true;
}

uint32_t ultrasnoicGetDistanceTemperture(uint32_t start, uint32_t end, float temp)
{
	float sonicSpeed;
	int32_t startl = start, endl = end;
	uint32_t timerCount;
	timerCount = ((endl - startl) > 0) ? (end - start) : (24000 + end - start) ;		//us
	sonicSpeed = 343.0 + (temp - 20.0) * 0.6;		//m/sec
	float distance = timerCount / 2000000.0 * (float) ((uint32_t)(sonicSpeed*100)) ;	//cm, us * 2 / m * s
	if(is_running)
	{
		return (uint32_t)distance;
	}
	else return 0;
}

#endif
