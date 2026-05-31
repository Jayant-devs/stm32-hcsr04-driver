/*
 * main.h
 *
 *  Created on: Dec 14, 2025
 *      Author: AnimeGuy
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_
#define clock_freq_120 120
#define clock_freq_84 84
#define clock_freq_50 50
#define clock_freq_180 180




#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;


void SystemClockConfig(void);
void UART2_Init(void);
void ErrorHandler(void);
void Tim_Init();
void SystemClock_Config_HSE(uint8_t clock_freq);



#endif /* INC_MAIN_H_ */
