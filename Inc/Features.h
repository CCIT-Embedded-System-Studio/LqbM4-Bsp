/**
 * @file Features.h
 * @author RealEyex (none)
 * @brief Reusable function declarations and macros
 * @version 0.1
 * @date 2021-05-21
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */
#include "stm32g4xx_hal.h"

/************************************************************************
 *                _  __           ___                __                 *
 *               | |/ /___ _  _  / __| __ __ _ _ _  / _|                *
 *               | ' </ -_) || | \__ \/ _/ _` | ' \|  _|                *
 *               |_|\_\___|\_, | |___/\__\__,_|_||_|_|                  *
 *                       |__/                                           *
 ************************************************************************/

#define KEY_SCAN_DELAY 20
#define KEY_DOWN GPIO_PIN_SET
#define KEY_UP GPIO_PIN_RESET

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
 * @brief Rewrite by user, this function in Key_Scan() call
 * 
 * @param KeyID Key id number
 * @param KeyStatus Now keys's status
 * @param DownTim The number of milliseconds(ms) to press the button
 * 
 * @return void
 */
typedef void (*KeyCallback)(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim);

/**
 * @brief Key scanf update state
 * 
 * @param KeyList Key info list
 * @param len Key info list length
 * @param Callback Key scanf callback function
 * 
 * @return void
 */
void Key_Scan(KeyInfo_t *KeyList, uint8_t len, KeyCallback Callback);

/************************************************************************
 *                   _   ___   ___    ___     _                         *
 *                 /_\ |   \ / __|  / __|___| |_                        *
 *                / _ \| |) | (__  | (_ / -_)  _|                       *
 *               /_/ \_\___/ \___|  \___\___|\__|                       *
 ************************************************************************/

#define BASE_VOLT 3.3

/**
 * @brief Get adc converted value
 * 
 * @param hadc Adc handle
 * @return uint16_t Adc value 
 */
uint16_t ADC_GetValue(ADC_HandleTypeDef *hadc);

/**
 * @brief Get adc converted voltage
 * 
 * @param hadc Adc handle
 * @return float ADC collected voltage
 */
float ADC_GetVolt(ADC_HandleTypeDef *hadc);

/************************************************************************
 *            _   _  _   ___ _____   ___            _                   *
 *           | | | |/_\ | _ \_   _| | _ \___ ____ _(_)__ ___            *
 *           | |_| / _ \|   / | |   |   / -_) _\ V / / _/ -_)           *
 *           \___/_/ \_\_|_\ |_|   |_|_\___\__|\_/|_\__\___|            *
 *                                                                      *
 ************************************************************************/

#define UART_RECV_SIZE 16

struct UartInfo
{
    UART_HandleTypeDef *huart;
    UartCallback Callback;
    uint8_t *Rxbuf;
    uint8_t len;
};

typedef struct UartInfo UartInfo_t;
typedef void (*UartCallback)(void);

/**
 * @brief Use UartInfo struct initialization uart
 * 
 * @param UartList UartInfo_t pointer
 * @param len UartList length
 * 
 * @return void
 */
void Uart_RxCallback(UartInfo_t *UartList, uint8_t len);

/************************************************************************
 *                 ___       __ _     ___ ___ ___                       *
 *                / __| ___ / _| |_  |_ _|_ _/ __|                      *
 *                \__ \/ _ \  _|  _|  | | | | (__                       *
 *                |___/\___/_|  \__| |___|___\___|                      *
 *                                                                      *
 ************************************************************************/

#define IIC_DELAY_TIME 1

struct IICInfo
{
    GPIO_TypeDef *SDAPort;
    uint16_t SDAPin;
    GPIO_TypeDef *SCLPort;
    uint16_t SCLPin;
};
typedef struct IICInfo IICInfo_t;

/**
 * @brief Initialization IIC
 * 
 * @param IICInfoList IICInfo struct list
 * 
 * @return void
 */
void IIC_Init(const IICInfo_t *IICInfoList);

/**
 * @brief IIC start signal
 * 
 * @param iic IICInfo_t struct
 * 
 * @return void
 */
void IIC_Start(const IICInfo_t *iic);

/**
 * @brief IIC stop signal
 * 
 * @param iic IICInfo_t struct
 * 
 * @return void
 */
void IIC_Stop(const IICInfo_t *iic);

/**
 * @brief IIC wait ACK signal
 * 
 * @param iic IICInfo_t struct
 * 
 * @return uint8_t Status code(ERROR or SUCCESS)
 */
uint8_t IIC_WaitAck(const IICInfo_t *iic);

/**
 * @brief IIC send ACK signal
 * 
 * @param iic IICInfo_t struct
 * 
 * @return void
 */
void IIC_SendAck(const IICInfo_t *iic);

/**
 * @brief IIC send NOTACK signal
 * 
 * @param iic IICInfo_t struct
 * 
 * @return void
 */
void IIC_SendNotAck(const IICInfo_t *iic);

/**
 * @brief IIC send one byte of data
 * 
 * @param iic IICInfo_t struct
 * @param data Want to send data
 * 
 * @return void
 */
void IIC_SendByte(const IICInfo_t *iic, uint8_t data);

/**
 * @brief IIC receive one byte of data
 * 
 * @param iic IICInfo_t struct
 * 
 * @return uint8_t Receive data
 */
uint8_t I2C_ReceiveByte(const IICInfo_t *iic);

/************************************************************************
 *     ___                _      ___           _                        *
 *    |_ _|_ _  _ __ _  _| |_   / __|__ _ _ __| |_ _ _ _  _ ___         *
 *     | || ' \| '_ \ || |  _| | (__/ _` | '_ \  _| '_| || / -_)        *
 *    |___|_||_| .__/\_,_|\__|  \___\__,_| .__/\__|_|  \_,_\___|        *
 *            |_|                       |_|                             *
 ************************************************************************/

struct ICInfo
{
    TIM_HandleTypeDef *htim;
    uint32_t Channel;
    uint32_t LastTick;
    uint16_t Period;
};

typedef struct ICInfo ICInfo_t;

/**
 * @brief Initialization IC 
 * 
 * @param ICInfoList ICInfo_t struct
 * @param len ICInfoList length
 * 
 * @return void
 */
void IC_Init(const ICInfo_t *ICInfoList, uint8_t len);

/**
 * @brief Get the pulse frequency captured by the IC
 * 
 * @return uint16_t pulse frequency (Hz)
 */
__INLINE uint16_t IC_GetPeriod(void);

/************************************************************************
 *                   ___  ___ _ ___ ___ ___ __                          *
 *                  |   \/ __/ ( _ ) _ )_  )  \                         *
 *                  | |) \__ \ / _ \ _ \/ / () |                        *
 *                  |___/|___/_\___/___/___\__/                         *
 *                                                                      *
 ************************************************************************/

struct DS18B20Info
{
    GPIO_TypeDef *DatPort;
    uint16_t DatPin;
};
typedef struct DS18B20Info DS18B20Info_t;

// void DS18B20_Reset(const DS18B20Info_t *ds18b20);

