/**
 * @file M4Bsp.c
 * @author RealEyex (none)
 * @brief 
 * @version 0.1
 * @date 2021-04-22
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */

#include "M4Bsp.h"

void M4_Led_Set(uint8_t led)
{
    HAL_GPIO_WritePin(GPIOC, LED_ALL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, 0x0100 << (led & 0x07), GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}
