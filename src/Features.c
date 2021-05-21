/**
 * @file Features.c
 * @author RealEyex (none)
 * @brief Reusable function definition
 * @version 0.1
 * @date 2021-05-21
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */

#include "Features.h"

/************************************************************************
 *                     ___ _____ _  _ ___ ___                           *
 *                    / _ \_   _| || | __| _ \                          *
 *                    | (_) || | | __ | _||   /                         *
 *                    \___/ |_| |_||_|___|_|_\                          *
 *                                                                      *
 ************************************************************************/

__STATIC_INLINE void Delay_us(uint32_t t)
{
    for (size_t i = 0; i < t * 16; i++)
        __NOP();
}

__STATIC_INLINE void GPIOPin_Input_Mode(GPIO_TypeDef *Port, uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Port, &GPIO_InitStructure);
}

__STATIC_INLINE void GPIOPin_Output_Mode(GPIO_TypeDef *Port, uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.Pin = Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(Port, &GPIO_InitStructure);
}

__STATIC_INLINE void GPIOPin_Write(GPIO_TypeDef *Port, uint16_t Pin, GPIO_PinState State)
{
    HAL_GPIO_WritePin(Port, Pin, State);
}

__STATIC_INLINE GPIO_PinState GPIOPin_Read(GPIO_TypeDef *Port, uint16_t Pin)
{
    return HAL_GPIO_ReadPin(Port, Pin);
}

/************************************************************************
 *                _  __           ___                __                 *
 *               | |/ /___ _  _  / __| __ __ _ _ _  / _|                *
 *               | ' </ -_) || | \__ \/ _/ _` | ' \|  _|                *
 *               |_|\_\___|\_, | |___/\__\__,_|_||_|_|                  *
 *                       |__/                                           *
 ************************************************************************/

// Lsat scanf systick
static uint32_t SKeyLastTick = 0;

void Key_Scan(KeyInfo_t *KeyList, uint8_t len, KeyCallback Callback)
{
    uint32_t NowTick = HAL_GetTick();
    if (NowTick - SKeyLastTick < KEY_SCAN_DELAY)
        return;

    for (size_t i = 0; i < len; i++)
    {
        if (HAL_GPIO_ReadPin(KeyList[i].Port, KeyList[i].Pin))
        {
            if (KeyList[i].Status == KEY_DOWN)
            {
                Callback(i + 1, KEY_UP, KeyList[i].TimCount);
                KeyList[i].TimCount = 0;
            }
            KeyList[i].Status = KEY_UP;
        }
        else
        {
            if (KeyList[i].Status == KEY_DOWN)
            {
                KeyList[i].TimCount += ABS(((int64_t)NowTick - SKeyLastTick));
                Callback(i + 1, KEY_DOWN, KeyList[i].TimCount);
            }
            KeyList[i].Status = KEY_DOWN;
        }
    }

    SKeyLastTick = NowTick;
}

/************************************************************************
 *                   _   ___   ___    ___     _                         *
 *                 /_\ |   \ / __|  / __|___| |_                        *
 *                / _ \| |) | (__  | (_ / -_)  _|                       *
 *               /_/ \_\___/ \___|  \___\___|\__|                       *
 ************************************************************************/

__INLINE uint16_t ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start(hadc);
    uint16_t ret = HAL_ADC_GetValue(hadc);
    HAL_ADC_Stop(hadc);
    return ret;
}

__INLINE float ADC_GetVolt(ADC_HandleTypeDef *hadc)
{
    return ADC_GetValue(hadc) / 4095.0 * BASE_VOLT;
}

/************************************************************************
 *            _   _  _   ___ _____   ___            _                   *
 *           | | | |/_\ | _ \_   _| | _ \___ ____ _(_)__ ___            *
 *           | |_| / _ \|   / | |   |   / -_) _\ V / / _/ -_)           *
 *           \___/_/ \_\_|_\ |_|   |_|_\___\__|\_/|_\__\___|            *
 *                                                                      *
 ************************************************************************/

static UartInfo_t *SUartList = NULL;

void Uart_RxCallback(UartInfo_t *UartList, uint8_t len)
{
    SUartList = UartList;
    for (size_t i = 0; i < 3; i++)
    {
        IRQn_Type UsartIRQ = 0;

        if (UartList[i].huart->Instance == USART1)
            UsartIRQ = USART1_IRQn;
        else if (UartList[i].huart->Instance == USART2)
            UsartIRQ = USART2_IRQn;
        else if (UartList[i].huart->Instance == USART3)
            UsartIRQ = USART3_IRQn;
        else
            return;

        HAL_NVIC_SetPriority(UsartIRQ, 0, 0);
        HAL_NVIC_EnableIRQ(UsartIRQ);
        __HAL_UART_ENABLE_IT(UartList[i].huart, UART_IT_IDLE);
        __HAL_UART_CLEAR_IDLEFLAG(UartList[i].huart);
        HAL_UART_Receive_DMA(UartList[i].huart, UartList[i].Rxbuf, UartList[i].len);
    }
}

void USART1_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(SUartList[0].huart, UART_FLAG_IDLE))
    {
        // Clear IDLE Flag
        __HAL_UART_CLEAR_IDLEFLAG(SUartList[0].huart);
        HAL_UART_DMAStop(SUartList[0].huart);
        RxCallback();
        memset((void *)SUartList[0].Rxbuf, 0, SUartList[0].len);
        HAL_UART_Receive_DMA(SUartList[0].huart, SUartList[0].Rxbuf, SUartList[0].len);
    }
    HAL_UART_IRQHandler(SUartList[0].huart);
}

void USART2_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(SUartList[1].huart, UART_FLAG_IDLE))
    {
        // Clear IDLE Flag
        __HAL_UART_CLEAR_IDLEFLAG(SUartList[1].huart);
        HAL_UART_DMAStop(SUartList[1].huart);
        RxCallback();
        memset((void *)SUartList[1].Rxbuf, 0, SUartList[1].len);
        HAL_UART_Receive_DMA(SUartList[1].huart, SUartList[1].Rxbuf, SUartList[1].len);
    }
    HAL_UART_IRQHandler(SUartList[1].huart);
}

void USART3_IRQHandler(void)
{
    if (__HAL_UART_GET_FLAG(SUartList[2].huart, UART_FLAG_IDLE))
    {
        // Clear IDLE Flag
        __HAL_UART_CLEAR_IDLEFLAG(SUartList[2].huart);
        HAL_UART_DMAStop(SUartList[2].huart);
        RxCallback();
        memset((void *)SUartList[2].Rxbuf, 0, SUartList[2].len);
        HAL_UART_Receive_DMA(SUartList[2].huart, SUartList[2].Rxbuf, SUartList[2].len);
    }
    HAL_UART_IRQHandler(SUartList[2].huart);
}

/************************************************************************
 *                 ___       __ _     ___ ___ ___                       *
 *                / __| ___ / _| |_  |_ _|_ _/ __|                      *
 *                \__ \/ _ \  _|  _|  | | | | (__                       *
 *                |___/\___/_|  \__| |___|___\___|                      *
 *                                                                      *
 ************************************************************************/

#define IIC_SDA_OUTPUT_MODE(x) GPIOPin_Output_Mode(x->SDAPort, x->SDAPin)
#define IIC_SDA_INPUT_MODE(x) GPIOPin_Input_Mode(x->SDAPort, x->SDAPin)
#define IIC_SCL_OUTPUT_MODE(x) GPIOPin_Output_Mode(x->SCLPort, x->SCLPin)
#define IIC_SCL_INPUT_MODE(x) GPIOPin_Input_Mode(x->SCLPort, x->SCLPin)

#define IIC_SDA_SET(x) GPIOPin_Write(x->SDAPort, x->SDAPin, GPIO_PIN_SET)
#define IIC_SDA_RESET(x) GPIOPin_Write(x->SDAPort, x->SDAPin, GPIO_PIN_RESET)
#define IIC_SCL_SET(x) GPIOPin_Write(x->SCLPort, x->SCLPin, GPIO_PIN_SET)
#define IIC_SCL_RESET(x) GPIOPin_Write(x->SCLPort, x->SCLPin, GPIO_PIN_RESET)

#define IIC_SDA_WRITE(x, s) GPIOPin_Write(x->SDAPort, x->SDAPin, s)

#define IIC_SDA_READ(x) GPIOPin_Read(x->SDAPort, x->SDAPin)

void IIC_Init(const IICInfo_t *iic)
{
    IIC_SDA_OUTPUT_MODE(iic);
    IIC_SCL_OUTPUT_MODE(iic);
}

void IIC_Start(const IICInfo_t *iic)
{
    IIC_SDA_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SDA_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
}

void IIC_Stop(const IICInfo_t *iic)
{
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SDA_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SDA_SET(iic);
    Delay_us(IIC_DELAY_TIME);
}

uint8_t IIC_WaitAck(const IICInfo_t *iic)
{
    uint16_t Timout = 5;

    IIC_SDA_INPUT_MODE(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_SET(iic);
    Delay_us(IIC_DELAY_TIME);

    while (IIC_SDA_READ(iic))
    {
        Timout--;
        Delay_us(IIC_DELAY_TIME);
        if (Timout == 0)
        {
            IIC_SDA_OUTPUT_MODE(iic);
            IIC_Stop(iic);
            return ERROR;
        }
    }
    IIC_SDA_OUTPUT_MODE(iic);
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    return SUCCESS;
}

void IIC_SendAck(const IICInfo_t *iic)
{
    IIC_SDA_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
}

void IIC_SendNotAck(const IICInfo_t *iic)
{
    IIC_SDA_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_SET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
}

void IIC_SendByte(const IICInfo_t *iic, uint8_t data)
{
    for (size_t i = 0; i < 8; i++)
    {
        IIC_SCL_RESET(iic);
        Delay_us(IIC_DELAY_TIME);
        IIC_SDA_WRITE(iic, data & (0x80 >> i));
        Delay_us(IIC_DELAY_TIME);
        Delay_us(IIC_DELAY_TIME);
        IIC_SCL_SET(iic);
        Delay_us(IIC_DELAY_TIME);
    }

    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
}

uint8_t I2C_ReceiveByte(const IICInfo_t *iic)
{
    uint8_t data = 0x00;

    IIC_SDA_INPUT_MODE(iic);
    for (size_t i = 0; i < 8; i++)
    {
        IIC_SCL_RESET(iic);
        Delay_us(IIC_DELAY_TIME);
        Delay_us(IIC_DELAY_TIME);
        IIC_SCL_SET(iic);
        Delay_us(IIC_DELAY_TIME);
        data |= IIC_SDA_READ(iic);
        data <<= 1;
    }
    IIC_SCL_RESET(iic);
    Delay_us(IIC_DELAY_TIME);
    IIC_SDA_OUTPUT_MODE(iic);
}

/************************************************************************
 *     ___                _      ___           _                        *
 *    |_ _|_ _  _ __ _  _| |_   / __|__ _ _ __| |_ _ _ _  _ ___         *
 *     | || ' \| '_ \ || |  _| | (__/ _` | '_ \  _| '_| || / -_)        *
 *    |___|_||_| .__/\_,_|\__|  \___\__,_| .__/\__|_|  \_,_\___|        *
 *            |_|                       |_|                             *
 ************************************************************************/

static ICInfo_t *SICList = NULL;
static uint8_t SICListLen = 0;

void IC_Init(const ICInfo_t *ICInfoList, uint8_t len)
{
    SICList = ICInfoList;
    SICListLen = len;
    for (size_t i = 0; i < len; i++)
    {
        __HAL_TIM_SET_PRESCALER(ICInfoList[i].htim, 80 - 1);
        HAL_TIM_IC_Start_IT(ICInfoList[i].htim, ICInfoList[i].Channel);
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    uint32_t NowTick = 0;
    for (size_t i = 0; i < SICListLen; i++)
    {
        if (SICList[i].htim == htim)
        {
            NowTick = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            SICList[i].Period = ABS((long)NowTick - (long)SICList[i].LastTick);
            SICList[i].LastTick = NowTick;
        }
    }
}

/************************************************************************
 *                   ___  ___ _ ___ ___ ___ __                          *
 *                  |   \/ __/ ( _ ) _ )_  )  \                         *
 *                  | |) \__ \ / _ \ _ \/ / () |                        *
 *                  |___/|___/_\___/___/___\__/                         *
 *                                                                      *
 ************************************************************************/


