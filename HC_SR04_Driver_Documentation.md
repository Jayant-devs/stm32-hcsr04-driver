# HC-SR04 Ultrasonic Sensor Driver (Polling Method) for STM32F446RE

## 1. Project Overview

This project implements a reusable HC-SR04 ultrasonic sensor driver for the STM32F446RE using:

* HAL library
* Bare-metal timer access where required
* UART for debugging and data output
* TIM2 as a microsecond time base
* Polling-based echo pulse measurement

The purpose of this project was not only to measure distance but also to learn:

* STM32 clock configuration
* Timer configuration
* UART communication
* GPIO configuration
* Driver architecture
* Public vs private APIs
* Microsecond timing

This project serves as the next learning step after GPIO, UART, and basic timer programming.

---

# 2. Hardware Used

## Development Board

* STM32 NUCLEO-F446RE

## Sensor

* HC-SR04 Ultrasonic Distance Sensor

## Connections

| HC-SR04 | STM32F446RE     |
| ------- | --------------- |
| VCC     | 5V              |
| GND     | GND             |
| TRIG    | PA5             |
| ECHO    | PA6             |
| UART TX | PA2 (USART2_TX) |
| UART RX | PA3 (USART2_RX) |

---

# 3. Important Hardware Note

The HC-SR04 ECHO pin outputs a 5V signal.

PA6 on STM32F446RE is a 5V tolerant (FT) pin, therefore it can safely receive the ECHO signal directly.

If a non-5V tolerant pin is used, a voltage divider or level shifter must be added.

---

# 4. Project Structure

```text
Core/
├── Inc/
│   └── main.h
│
├── Src/
│   ├── main.c
│   ├── msp.c
│   └── it.c

Drivers/
└── HCSR04/
    ├── hcsr04.c
    └── hcsr04.h
```

---

# 5. Clock Configuration

The project uses:

```text
HSE (Bypass)
     ↓
PLL
     ↓
180 MHz SYSCLK
```

Clock settings:

| Parameter | Value   |
| --------- | ------- |
| SYSCLK    | 180 MHz |
| HCLK      | 180 MHz |
| APB1      | 45 MHz  |
| APB2      | 90 MHz  |

For APB1 timers:

```text
TIM Clock = 2 × APB1
```

Therefore:

```text
TIM2 Clock = 90 MHz
```

---

# 6. Timer Configuration

TIM2 is configured as a free-running 32-bit timer.

```c
Prescaler = 89
ARR        = 0xFFFFFFFF
```

Timer frequency:

```text
90 MHz / (89 + 1)
=
1 MHz
```

Therefore:

```text
1 Timer Tick = 1 Microsecond
```

This timer is used for:

* Microsecond delays
* Echo pulse width measurement

---

# 7. Driver Workflow

## Step 1

Generate trigger pulse

```text
LOW  -> 2 us
HIGH -> 10 us
LOW
```

---

## Step 2

Wait for ECHO rising edge

```text
ECHO = HIGH
```

---

## Step 3

Reset timer

```c
__HAL_TIM_SET_COUNTER(&htim2,0);
```

---

## Step 4

Wait for ECHO falling edge

```text
ECHO = LOW
```

---

## Step 5

Read pulse width

```c
echo_time = TIM2->CNT;
```

---

## Step 6

Convert to distance

HC-SR04 datasheet formula:

```text
Distance(cm)
=
Echo Time(us) / 58
```

---

# 8. Public Driver APIs

## Initialize Driver

```c
void HCSR04_Init(void);
```

Configures:

* PA5 as output
* PA6 as input

---

## Read Distance

```c
float HCSR04_ReadDistanceCM(void);
```

Returns:

```text
Distance in centimeters
```

Returns:

```text
-1.0f on timeout
```

---

# 9. Internal Driver Functions

The following helper functions are intentionally kept private using the static keyword:

```c
static void delay_us(uint32_t us);
```

Microsecond delay using TIM2.

---

```c
static void HCSR04_Trigger(void);
```

Generates the trigger pulse.

---

```c
static uint32_t HCSR04_GetEchoTime(void);
```

Measures echo pulse width.

---

# 10. Timeout Protection

Initial implementation used:

```c
while(GPIO == HIGH);
```

and

```c
while(GPIO == LOW);
```

which could block forever if:

* Sensor disconnected
* Wiring incorrect
* No echo returned

The driver was updated to include timeout protection.

```c
#define HCSR04_TIMEOUT_US 30000U
```

If timeout occurs:

```c
return 0;
```

and the public API returns:

```c
-1.0f
```

---

# 11. UART Debugging

USART2 was used for debugging.

Configuration:

```text
115200 Baud
8 Data Bits
No Parity
1 Stop Bit
```

Example output:

```text
Distance = 20.14 cm
Distance = 19.95 cm
Distance = 20.31 cm
```

---

# 12. Issues Encountered During Development

## 1. Function Visibility Issue

Initially:

```c
static void HCSR04_Trigger(void);
```

was called from main.c.

This caused:

```text
undefined reference to HCSR04_Trigger
```

because static functions are visible only inside their own source file.

Solution:

* Keep helper functions private
* Expose only public APIs in header files

---

## 2. Floating Point Printing

Using:

```c
sprintf(msg,"Distance = %.2f cm",distance);
```

did not work initially.

Solution:

Add linker option:

```text
-u _printf_float
```

in:

```text
MCU GCC Linker
→ Miscellaneous
```

---

## 3. 5V Echo Concern

The HC-SR04 ECHO pin outputs a 5V signal, while the STM32 operates at 3.3V logic levels. Before connecting the sensor, the STM32F446RE datasheet was reviewed to verify whether the selected GPIO pin could safely tolerate a 5V input.

It was confirmed that:

```text
PA6 = FT (5V Tolerant)
```

The "FT" designation indicates that PA6 can safely accept a 5V signal when used as a digital input. Therefore, the HC-SR04 ECHO pin was connected directly to PA6 without requiring a voltage divider or level shifter.

This highlighted the importance of verifying electrical specifications in both the sensor and microcontroller datasheets rather than relying on assumptions.

---

# 13. Improvements Made During Development

## Added Driver Layer

Instead of placing all code in main.c:

```text
hcsr04.c
hcsr04.h
```

were created.

---

## Added Timeout Protection

Prevents CPU lockup.

---

## Added Pin Macros

```c
#define HCSR04_TRIG_PORT GPIOA
#define HCSR04_TRIG_PIN  GPIO_PIN_5

#define HCSR04_ECHO_PORT GPIOA
#define HCSR04_ECHO_PIN  GPIO_PIN_6
```

Improves portability.

---

## Added Documentation

Code comments explain:

* Clock tree
* Timer calculations
* Sensor operation
* Driver architecture

---

# 14. Results

Measured distances were stable.

Example readings:

```text
19.8 cm
20.2 cm
20.1 cm
19.9 cm
20.3 cm
```

Error remained small when the target was stationary.

---

# 15. Limitations of Current Version

Current implementation uses polling:

```c
while(HAL_GPIO_ReadPin(...));
```

CPU remains occupied during measurement.

Limitations:

* CPU blocking
* Less efficient
* Not suitable for multitasking applications

---

# 16. Future Improvements (Version 2)

Planned upgrades:

## Input Capture Mode

Replace polling with:

```text
Timer Input Capture
```

Workflow:

```text
Rising Edge
↓
Capture Start

Falling Edge
↓
Capture End
```

Advantages:

* Higher accuracy
* Lower CPU usage
* Professional implementation style

---

## Driver Handle Structure

Planned handle:

```c
typedef struct
{
    TIM_HandleTypeDef *htim;

    GPIO_TypeDef *TrigPort;
    uint16_t TrigPin;

    GPIO_TypeDef *EchoPort;
    uint16_t EchoPin;

} HCSR04_HandleTypeDef;
```

Advantages:

* Multiple sensor support
* Configurable pins
* Configurable timers
* Better driver abstraction

---

# 17. Conclusion

A complete HC-SR04 polling-based driver was successfully implemented on STM32F446RE using HAL and timer-based microsecond timing.

The project provided practical experience with:

* STM32 clocks
* PLL configuration
* UART
* GPIO
* Timers
* Driver design
* Embedded debugging

This project serves as the foundation for a future Input Capture based ultrasonic driver implementation.
