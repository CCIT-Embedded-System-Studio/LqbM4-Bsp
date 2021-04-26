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

__INLINE void M4_Led_Set(uint16_t led)
{
    // Reset all led pins
    M4_LED_GPIO_PORT->BSRR = 0xff00;

    M4_LED_GPIO_PORT->BRR = (led & 0xff00);
    M4_LATCHES_GPIO_PORT->BSRR |= M4_LATCHES_GPIO_PIN;
    __NOP();
    __NOP();
    __NOP();
    M4_LATCHES_GPIO_PORT->BRR |= M4_LATCHES_GPIO_PIN;
    __NOP();
    __NOP();
    __NOP();
}

#endif

#ifdef M4_KEY_ENABLE

static KeyInfo_t KeyList[M4_KEY_SUM_MAX] = {
    {M4_KEY_B1_PORT, M4_KEY_B1_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B2_PORT, M4_KEY_B2_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B3_PORT, M4_KEY_B3_PIN, M4_KEY_UP, 0x00},
    {M4_KEY_B4_PORT, M4_KEY_B4_PIN, M4_KEY_UP, 0x00}};

static uint32_t KeyLastTick = 0;

void M4_Key_Scan(void)
{
    uint32_t NowTick = HAL_GetTick();
    if (NowTick - KeyLastTick < M4_KEY_SCAN_DELAY)
        return;

    for (size_t i = 0; i < M4_KEY_SUM_MAX; i++)
    {
        if (HAL_GPIO_ReadPin(KeyList[i].Port, KeyList[i].Pin))
        {
            if (KeyList[i].Status == M4_KEY_DOWN)
            {
                M4_Key_Scan_Callback(i + 1, M4_KEY_UP, KeyList[i].TimCount);
                KeyList[i].TimCount = 0;
            }
            KeyList[i].Status = M4_KEY_UP;
        }
        else
        {
            if (KeyList[i].Status == M4_KEY_DOWN)
            {

                KeyList[i].TimCount += ABS(((int64_t)NowTick - KeyLastTick));
                M4_Key_Scan_Callback(i + 1, M4_KEY_DOWN, KeyList[i].TimCount);
            }
            KeyList[i].Status = M4_KEY_DOWN;
        }
    }

    KeyLastTick = NowTick;
}

__weak void M4_Key_Scan_Callback(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim) {}

#endif

#ifdef M4_R37_ENABLE

extern ADC_HandleTypeDef hadc2;

uint16_t M4_R37_GetValue(void)
{
    HAL_ADC_Start(&hadc2);
    uint16_t ret = (HAL_ADC_GetValue(&hadc2) / 4095.0) * M4_R37_BASE_VOLT;
    HAL_ADC_Stop(&hadc2);
    return ret;
}

#endif

#ifdef M4_R38_ENABLE

extern ADC_HandleTypeDef hadc1;

uint16_t M4_R38_GetValue(void)
{
    HAL_ADC_Start(&hadc1);
    uint16_t ret = (HAL_ADC_GetValue(&hadc1) / 4095.0) * M4_R38_BASE_VOLT;
    HAL_ADC_Stop(&hadc1);
    return ret;
}

#endif

#ifdef M4_UART_ENABLE

extern UART_HandleTypeDef huart1;

static uint8_t RecvBuf[M4_UART_RECV_SIZE] = {};
static UartCallback RxCallback = NULL;

void M4_Uart_RxCallback(UartCallback callback)
{
    RxCallback = callback;
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    HAL_UART_Receive_DMA(&huart1, (uint8_t *)RecvBuf, M4_UART_RECV_SIZE);
}

void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
    {
        // Clear IDLE Flag
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_UART_DMAStop(&huart1);
        RxCallback();
        memset((void *)RecvBuf, 0, M4_UART_RECV_SIZE);
        HAL_UART_Receive_DMA(&huart1, (uint8_t *)RecvBuf, M4_UART_RECV_SIZE);
    }
    HAL_UART_IRQHandler(&huart1);
}

#endif
