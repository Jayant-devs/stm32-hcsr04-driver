/*
 * hcsr01_main.h
 *
 *  Created on: May 31, 2026
 *      Author: AnimeGuy
 */

#ifndef HCSR04_HCSR04_MAIN_H_
#define HCSR04_HCSR04_MAIN_H_

#include "main.h"

/*
 * Initializes GPIO pins used by HC-SR04.
 * TRIG -> PA5
 * ECHO -> PA6
 */
void HCSR04_Init(void);

/*
 * Returns measured distance in centimeters.
 *
 * Return value:
 *  >0  : Valid distance
 *  -1  : Timeout / invalid measurement
 */
float HCSR04_ReadDistanceCM(void);


#endif /* HCSR04_HCSR04_MAIN_H_ */
