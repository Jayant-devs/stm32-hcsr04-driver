/*
 * main.c
 *
 *  Created on: Dec 14, 2025
 *      Author: AnimeGuy
 */



#include "main.h"
#include "hcsr04main.h"


void SystemClockConfig(void);
void UART2_Init(void);
void ErrorHandler(void);
void Tim_Init();
void SystemClock_Config_HSE(uint8_t clock_freq);

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;


char *user_data="The appliaction is working!!\r\n";

int main(void)
{
    HAL_Init();
    SystemClock_Config_HSE(clock_freq_180);

    UART2_Init();
    Tim_Init();
    HAL_TIM_Base_Start(&htim2);

    HAL_UART_Transmit(&huart2,(uint8_t *)user_data,strlen(user_data),HAL_MAX_DELAY);

    HCSR04_Init();

    float distance;

    char msg[50];

    while(1)
    {
    	distance = HCSR04_ReadDistanceCM();

    	if(distance < 0)
    	{
    	    sprintf(msg,"Timeout\r\n");
    	}
    	else
    	{
    	    sprintf(msg,"Distance = %.2f cm\r\n",distance);
    	}

        HAL_UART_Transmit(&huart2,
                          (uint8_t*)msg,
                          strlen(msg),
                          HAL_MAX_DELAY);

        HAL_Delay(500);
    }
}
void SystemClockConfig(void)
{

}

//high level peripheral initialization
void UART2_Init(void){
	huart2.Instance=USART2;
	huart2.Init.BaudRate=115200;
	huart2.Init.WordLength=UART_WORDLENGTH_8B;
	huart2.Init.StopBits=UART_STOPBITS_1;
	huart2.Init.Parity=UART_PARITY_NONE;
	huart2.Init.HwFlowCtl=UART_HWCONTROL_NONE;
	huart2.Init.Mode=UART_MODE_TX_RX;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if(	(HAL_UART_Init(&huart2))!= HAL_OK){
		//there is a problem
		ErrorHandler();
	}

}

/*
 * TIM2 configuration:
 *
 * SYSCLK = 180 MHz
 * APB1   = 45 MHz
 * TIM2CLK= 90 MHz
 *
 * PSC = 89
 *
 * Timer Frequency
 * = 90 MHz / (89 + 1)
 * = 1 MHz
 *
 * Therefore:
 * 1 timer tick = 1 us
 */
void Tim_Init(){
	htim2.Instance=TIM2;
	htim2.Init.Prescaler = 89;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK){
		ErrorHandler();
	}
}


void SystemClock_Config_HSE(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint32_t FLASH_LAT=0;

	osc_init.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState=RCC_HSE_BYPASS;
	osc_init.PLL.PLLState=RCC_PLL_ON;
	osc_init.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	switch (clock_freq) {
		case clock_freq_50:
		{
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=100;
			osc_init.PLL.PLLP=2;
			osc_init.PLL.PLLQ=2;
			osc_init.PLL.PLLR=2;

			clk_init.ClockType=RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider=RCC_HCLK_DIV2;
			FLASH_LAT=FLASH_ACR_LATENCY_1WS;

			break;
		}
		case clock_freq_84:
		{
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=168;
			osc_init.PLL.PLLP=2;
			osc_init.PLL.PLLQ=2;
			osc_init.PLL.PLLR=2;

			clk_init.ClockType=RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider=RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider=RCC_HCLK_DIV2;
			FLASH_LAT=FLASH_ACR_LATENCY_2WS;
			break;
		}
		case clock_freq_120:
		{
			osc_init.PLL.PLLM=8;
			osc_init.PLL.PLLN=240;
			osc_init.PLL.PLLP=2;
			osc_init.PLL.PLLQ=2;
			osc_init.PLL.PLLR=2;

			clk_init.ClockType=RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider=RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider=RCC_HCLK_DIV2;
			FLASH_LAT=FLASH_ACR_LATENCY_3WS;
			break;
		}
		case clock_freq_180:
				{
					//enabe the clock for the power controller

					__HAL_RCC_PWR_CLK_ENABLE();
					//set regulator voltage scale as 1
					__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
					//turn on the over drive mode of the voltage regulator
					__HAL_PWR_OVERDRIVE_ENABLE();

					osc_init.PLL.PLLM=8;
					osc_init.PLL.PLLN=360;
					osc_init.PLL.PLLP=2;
					osc_init.PLL.PLLQ=2;
					osc_init.PLL.PLLR=2;

					clk_init.ClockType=RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
					clk_init.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
					clk_init.AHBCLKDivider=RCC_SYSCLK_DIV1;
					clk_init.APB1CLKDivider=RCC_HCLK_DIV4;
					clk_init.APB2CLKDivider=RCC_HCLK_DIV2;
					FLASH_LAT=FLASH_ACR_LATENCY_5WS;
					break;
				}
		default:
		{
			return;
		}

	}
	if (HAL_RCC_OscConfig(&osc_init)!=HAL_OK)
		{
			ErrorHandler();
		}
	if (HAL_RCC_ClockConfig(&clk_init, FLASH_LAT)!=HAL_OK)
			{
				ErrorHandler();
			}

	//Systick Configuration

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void ErrorHandler(void){
	while(1);
}
