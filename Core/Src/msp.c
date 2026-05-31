
#include "main.h"

void HAL_MspInit(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	SCB->SHCSR |= 0x7<<16;
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;

	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	gpio_uart.Pin=GPIO_PIN_2;
	gpio_uart.Mode=GPIO_MODE_AF_PP;
	gpio_uart.Pull=GPIO_PULLUP;
	gpio_uart.Speed=GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate=GPIO_AF7_USART2;//uart2_tx
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin=GPIO_PIN_3;//uart2_rx
	HAL_GPIO_Init(GPIOA, &gpio_uart);

}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{

	__HAL_RCC_TIM2_CLK_ENABLE();

}
