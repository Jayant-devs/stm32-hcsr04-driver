# STM32 HC-SR04 Driver

HC-SR04 ultrasonic sensor driver for STM32F446RE using HAL and TIM2 polling.

## Features

* UART debugging
* TIM2 microsecond time base
* Distance measurement in centimeters
* Timeout protection
* Modular driver architecture

## Hardware

* STM32 NUCLEO-F446RE
* HC-SR04 Ultrasonic Sensor

## Connections

| HC-SR04 | STM32 |
| ------- | ----- |
| TRIG    | PA5   |
| ECHO    | PA6   |
| VCC     | 5V    |
| GND     | GND   |

## Usage

```c
HCSR04_Init();

while(1)
{
    float distance;

    distance = HCSR04_ReadDistanceCM();

    HAL_Delay(500);
}
```

## Future Work

* Timer Input Capture implementation
* Interrupt-based measurement
* Handle-based driver abstraction
* Multiple sensor support
