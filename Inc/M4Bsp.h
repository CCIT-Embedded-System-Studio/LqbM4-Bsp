/**
 * @file M4Bsp.h
 * @author RealEyex (none)
 * @brief 
 * @version 0.1
 * @date 2021-04-22
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */

#pragma once

#include "M4Bsp_conf.h"
#include "stm32g4xx_hal.h"
#include "math.h"

#ifdef M4_LED_ENABLE

// Latches gpio port and gpio pins
#define M4_LATCHES_GPIO_PORT GPIOD
#define M4_LATCHES_GPIO_PIN GPIO_PIN_2

// Led gpio pins
#define M4_LED_1 0x0100
#define M4_LED_2 0x0200
#define M4_LED_3 0x0400
#define M4_LED_4 0x0800
#define M4_LED_5 0x1000
#define M4_LED_6 0x2000
#define M4_LED_7 0x4000
#define M4_LED_8 0x8000

// led gpio port
#define M4_LED_GPIO_PORT GPIOC

/**
 * @brief led number, can be combinedd by '|',
 *       such as (M4_LED_1|M4_LED_2)
 * 
 * @param led led number such as M4_LED_1
 * 
 * @return void
 */
void M4_Led_Set(uint16_t led);

#endif

#ifdef M4_KEY_ENABLE

// Keys max sum
#define M4_KEY_SUM_MAX 4

// Delay for one key scanf
#define M4_KEY_SCAN_DELAY 10

// Keys status for down or up 
#define M4_KEY_UP 0x00
#define M4_KEY_DOWN 0x01

// Keys gpio port
#define M4_KEY_B1_PORT GPIOB
#define M4_KEY_B2_PORT GPIOB
#define M4_KEY_B3_PORT GPIOB
#define M4_KEY_B4_PORT GPIOA

// Keys gpio pins
#define M4_KEY_B1_PIN GPIO_PIN_0
#define M4_KEY_B2_PIN GPIO_PIN_1
#define M4_KEY_B3_PIN GPIO_PIN_2
#define M4_KEY_B4_PIN GPIO_PIN_0

// Key ID
#define M4_KEY_B1 0x01
#define M4_KEY_B2 0x02
#define M4_KEY_B3 0x03
#define M4_KEY_B4 0x04

// Math function
#define ABS(x) ((x < 0)? -x : x) 

// Save key info 
struct KeyInfo
{
    GPIO_TypeDef *Port;
    uint16_t Pin;
    uint8_t Status;
    uint32_t TimCount;
};
typedef struct KeyInfo KeyInfo_t;

/**
 * @brief Scan M4 board key status
 * 
 * @return void
 */
void M4_Key_Scan(void);

/**
 * @brief Rewrite by user, this function in M4_Key_Scan() call
 * 
 * @param KeyID Key id, value can is M4_KEY_B1
 * @param KeyStatus Now keys's status
 * @param DownTim The number of milliseconds(ms) to press the button
 * 
 * @return void
 */
__weak void M4_Key_Scan_Callback(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim);

#endif

#ifdef M4_R37_ENABLE

#ifndef M4_R37_BASE_VOLT
#define M4_R37_BASE_VOLT 3.3
#endif

/**
 * @brief Get R37 adc value
 * 
 * @param void
 * 
 * @return uint16_t R37 value in 0 ~ M4_R37_BASE_VOLT
 */
uint16_t M4_R37_GetValue(void);

#endif

#ifdef M4_R38_ENABLE

#ifndef M4_R38_BASE_VOLT
#define M4_R38_BASE_VOLT 3.3
#endif

/**
 * @brief Get R38 adc value
 * 
 * @param void
 * 
 * @return uint16_t R38 value in 0 ~ M4_R38_BASE_VOLT
 */
uint16_t M4_R38_GetValue(void);

#endif
