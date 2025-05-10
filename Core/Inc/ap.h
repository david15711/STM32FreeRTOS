/*
 * ap.h
 *
 *  Created on: May 2, 2025
 *      Author: MS
 */

#ifndef INC_AP_H_
#define INC_AP_H_

#include "hw.h"

typedef struct {
	char msg[7];
	char msg_type;
} IPCmessage_t;

void apInit(void);
void apMain(void);

#endif /* INC_AP_H_ */
