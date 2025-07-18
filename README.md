# Space Invaders Clone on STM32L476RG

A minimalist Space Invaders-style game implemented on the STM32L476RG Nucleo board, featuring:

- 1.8" SPI LCD screen for graphics
- Analog joystick with push button for control

## Features

- Classic arcade gameplay: move and shoot
- Simple sprite rendering and animations
- Real-time input via analog joystick
- Frame buffer for optimized smooth display
- Runs bare-metal on STM32

## Hardware Requirements

- **STM32L476RG Nucleo board**
- **1.8" SPI TFT LCD module** (ST7735 or similar)
- **Analog joystick module** (with VRX, VRY, and switch)

## Pin Configuration

### LCD (SPI, 1.8" TFT)

| LCD Pin | STM32 Pin | Description         |
|---------|-----------|---------------------|
| VCC     | VDD       | Power               |
| GND     | GND       | Ground              |
| DIN     | PC3       | SPI MOSI            |
| CLK     | PB10      | SPI Clock           |
| CS      | PB12      | Chip Select         |
| DC      | PB11      | Data/Command Select |
| RST     | PB2       | Reset (active low)  |

### Joystick

| Joystick Pin | STM32 Pin | Description           |
|--------------|-----------|-----------------------|
| GND          | GND       | Ground                |
| +5V          | 3V3       | Power (3.3V safe)     |
| VRX          | PA0       | Analog X-axis (ADC)   |
| VRY          | PA1       | Analog Y-axis (ADC)   |
| SW           | PA10      | Button (Digital In)   |

## Software Details

- Written in **C** using **STM32CubeIDE**
- Uses **HAL drivers** for GPIO, ADC, and SPI
- Uses HAGL for frame generation (https://github.com/tuupola/hagl - correct version of library files included in this repository)
- Includes custom driver for the ST7735 LCD
- Game loop runs in the main loop without RTOS

## Photos
![zptSOkbn](https://github.com/user-attachments/assets/0680cad9-98ae-4cef-9465-b3a25e443086)
![Q_AYsGm6](https://github.com/user-attachments/assets/c48f078c-3c89-403d-902e-acd99b5c02ab)
![BV1B68gF](https://github.com/user-attachments/assets/bd7247cb-3345-47cd-a389-b1caf3a4de31)
