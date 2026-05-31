#include "hcsr04main.h"

/*=============================
 * HC-SR04 Pin Configuration
 *=============================*/
#define HCSR04_TRIG_PORT      GPIOA
#define HCSR04_TRIG_PIN       GPIO_PIN_5

#define HCSR04_ECHO_PORT      GPIOA
#define HCSR04_ECHO_PIN       GPIO_PIN_6

/* Maximum waiting time for echo */
#define HCSR04_TIMEOUT_US     30000U

/*=============================
 * Private Function Prototypes
 *=============================*/
static void delay_us(uint32_t us);
static void HCSR04_Trigger(void);
static uint32_t HCSR04_GetEchoTime(void);

/*
 * TIM2 configuration:
 *
 * SYSCLK  = 180 MHz
 * APB1    = 45 MHz
 * TIM2CLK = 90 MHz
 *
 * PSC = 89
 *
 * Timer Frequency
 * = 90 MHz / (89 + 1)
 * = 1 MHz
 *
 * Therefore:
 * 1 Timer Tick = 1 us
 */
static void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    while(__HAL_TIM_GET_COUNTER(&htim2) < us);
}

/*
 * Configure GPIO pins used by HC-SR04
 *
 * TRIG -> PA5
 * ECHO -> PA6
 */
void HCSR04_Init(void)
{
    GPIO_InitTypeDef gpio;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure Trigger Pin */
    gpio.Pin   = HCSR04_TRIG_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(HCSR04_TRIG_PORT, &gpio);

    /* Configure Echo Pin */
    gpio.Pin  = HCSR04_ECHO_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(HCSR04_ECHO_PORT, &gpio);
}

/*
 * Generate Trigger Pulse
 *
 * HC-SR04 Datasheet:
 * LOW  -> 2 us
 * HIGH -> 10 us
 * LOW
 */
static void HCSR04_Trigger(void)
{
    HAL_GPIO_WritePin(HCSR04_TRIG_PORT,
                      HCSR04_TRIG_PIN,
                      GPIO_PIN_RESET);

    delay_us(2);

    HAL_GPIO_WritePin(HCSR04_TRIG_PORT,
                      HCSR04_TRIG_PIN,
                      GPIO_PIN_SET);

    delay_us(10);

    HAL_GPIO_WritePin(HCSR04_TRIG_PORT,
                      HCSR04_TRIG_PIN,
                      GPIO_PIN_RESET);
}

/*
 * Measure ECHO Pulse Width
 *
 * Returns:
 * Pulse Width in microseconds
 *
 * Returns:
 * 0 -> Timeout
 */
static uint32_t HCSR04_GetEchoTime(void)
{
    uint32_t timeout;

    /* Wait for previous echo to finish */
    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT,
                           HCSR04_ECHO_PIN) == GPIO_PIN_SET);

    HCSR04_Trigger();

    /* Wait for rising edge */
    timeout = HCSR04_TIMEOUT_US;

    __HAL_TIM_SET_COUNTER(&htim2, 0);

    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT,
                           HCSR04_ECHO_PIN) == GPIO_PIN_RESET)
    {
        if(__HAL_TIM_GET_COUNTER(&htim2) > timeout)
        {
            return 0;
        }
    }

    /* Measure pulse width */
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    timeout = HCSR04_TIMEOUT_US;

    while(HAL_GPIO_ReadPin(HCSR04_ECHO_PORT,
                           HCSR04_ECHO_PIN) == GPIO_PIN_SET)
    {
        if(__HAL_TIM_GET_COUNTER(&htim2) > timeout)
        {
            return 0;
        }
    }

    return __HAL_TIM_GET_COUNTER(&htim2);
}

/*
 * Distance Formula:
 *
 * Distance(cm)
 * =
 * Echo_Time(us) / 58
 *
 * Returns:
 * Distance in centimeters
 *
 * Returns:
 * -1.0f -> Timeout
 */
float HCSR04_ReadDistanceCM(void)
{
    uint32_t echo_time;

    echo_time = HCSR04_GetEchoTime();

    if(echo_time == 0)
    {
        return -1.0f;
    }

    return ((float)echo_time / 58.0f);
}
