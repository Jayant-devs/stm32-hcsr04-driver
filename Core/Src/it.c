/*
 * it.c
 *
 *  Created on: Dec 14, 2025
 *      Author: AnimeGuy
 */
#include "main.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

