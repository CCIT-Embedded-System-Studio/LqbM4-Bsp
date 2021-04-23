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

#ifdef M4_LED_ENABLE

#define M4_LED_1 0x01
#define M4_LED_2 0x02
#define M4_LED_3 0x04
#define M4_LED_4 0x08
#define M4_LED_5 0x10
#define M4_LED_6 0x20
#define M4_LED_7 0x40
#define M4_LED_8 0x80

#define LED_ALL_PIN (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

/**
 * @brief Set the led according to param led
 * 
 * @param led led number such as M4_LED_1
 * 
 * @note led number can be combinedd by '|',
 *       such as (M4_LED_1|M4_LED_2)
 */
void M4_Led_Set(uint8_t led);

#endif
#ifdef M4_KEY_ENABLE

#define M4_KEY_SUM_MAX 4
#define M4_KEY_SCAN_DELAY 10

#define M4_KEY_UP 0x00
#define M4_KEY_DOWN 0x01

#define M4_KEY_LONG_MAX 1000
#define M4_KEY_LONG_DOWN

#define M4_KEY_B1_PORT GPIOB
#define M4_KEY_B2_PORT GPIOB
#define M4_KEY_B3_PORT GPIOB
#define M4_KEY_B4_PORT GPIOA

#define M4_KEY_B1_PIN GPIO_PIN_0
#define M4_KEY_B2_PIN GPIO_PIN_1
#define M4_KEY_B3_PIN GPIO_PIN_2
#define M4_KEY_B4_PIN GPIO_PIN_0

struct KeyInfo
{
    GPIO_TypeDef *Port;
    uint16_t Pin;
    uint8_t Status;
    uint8_t Count;
};
typedef struct KeyInfo KeyInfo_t;
typedef void (*Callback)(void);

/**
 * @brief Scan M4 board key status
 * 
 */
void M4_Key_Scan(void);

/**
 * @brief Handlling key events
 * 
 */
void M4_Key_Handle(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B1_Short(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B2_Short(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B3_Short(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B4_Short(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B1_Long(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B2_Long(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B3_Long(void);

/**
 * @brief Rewritten by the user
 * 
 */
__weak void M4_B4_Long(void);

#endif
