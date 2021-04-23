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

#ifdef M4_LED_ENABLE

__INLINE void M4_Led_Set(uint8_t led)
{
    HAL_GPIO_WritePin(GPIOC, LED_ALL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, (uint16_t)led << 8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

#endif

#ifdef M4_KEY_ENABLE

static KeyInfo_t KeyList[M4_KEY_SUM_MAX] = {
    {M4_KEY_B1_PORT, M4_KEY_B1_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B2_PORT, M4_KEY_B2_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B3_PORT, M4_KEY_B3_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B4_PORT, M4_KEY_B4_PIN, M4_KEY_UP, 0x00}};

static Callback KeyHandles[M4_KEY_SUM_MAX * 2] = {
    M4_B1_Short,
    M4_B2_Short,
    M4_B3_Short,
    M4_B4_Short,
    M4_B1_Long,
    M4_B2_Long,
    M4_B3_Long,
    M4_B4_Long};

static uint32_t KeyScanTim = 0;
static uint8_t KeyClick = 0x00;

void M4_Key_Scan(void)
{
    if (HAL_GetTick() - KeyScanTim < M4_KEY_SCAN_DELAY)
        return;
    KeyScanTim = HAL_GetTick();
    for (size_t i = 0; i < M4_KEY_SUM_MAX; i++)
    {
        if (HAL_GPIO_ReadPin(KeyList[i].Port, KeyList[i].Pin))
        {
            if (KeyList[i].Status == M4_KEY_DOWN)
            {
                if (KeyList[i].Count > M4_KEY_LONG_MAX)
                    KeyClick |= (0x10 << i);
                else
                    KeyClick |= (0x01 << i);
                KeyList[i].Count = 0;
            }

            KeyList[i].Status = M4_KEY_UP;
        }
        else
        {
            if (KeyList[i].Status == M4_KEY_DOWN)
            {
                if (KeyList[i].Count > M4_KEY_LONG_MAX)
                {
#ifdef M4_KEY_LONG_DOWN
                    KeyClick |= (0x10 << i);
#endif
                    continue;
                }
                ++KeyList[i].Count;
            }
            KeyList[i].Status = M4_KEY_DOWN;
        }
    }
}

void M4_Key_Handle(void)
{
    for (size_t i = 0; i < M4_KEY_SUM_MAX * 2; i++)
    {
        if(KeyClick & (0x01 << i))
        {
            KeyHandles[i]();
            KeyClick ^= (0x01 << i);
        }
    }
    
}

__weak void M4_B1_Short(void)
{
}

__weak void M4_B2_Short(void)
{
}

__weak void M4_B3_Short(void)
{
}

__weak void M4_B4_Short(void)
{
}

__weak void M4_B1_Long(void)
{
}

__weak void M4_B2_Long(void)
{
}

__weak void M4_B3_Long(void)
{
}

__weak void M4_B4_Long(void)
{
}

#endif
