/**
 * @file M4Bsp.h
 * @author RealEyex (none)
 * @brief M4 board function declaration
 * @version 0.1
 * @date 2021-04-22
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */

#pragma once

#include "M4Bsp_conf.h"
#include "Features.h"
#include "stdio.h"
#include "string.h"
#include "i2c.h"

// Math function
#define ABS(x) ((x < 0) ? -x : x)

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
void M4_Key_Scan_Callback(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim);

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

#ifdef M4_UART_ENABLE

#ifndef M4_UART_RECV_SIZE
#define M4_UART_RECV_SIZE 32
#endif

typedef void (*UartCallback)(void);

void M4_Uart_RxCallback(UartCallback callback);

/**
 * @brief Uart data transmit
 * 
 * @param data Data buffer pointer
 * @param len Data length
 * 
 * @return void
 */
void M4_Uart_Transmit(uint8_t *data, uint32_t len);

#endif

#ifdef M4_EEPROM_ENABLE

#define M4_IIC_EEPROM_DEVICE_ADDRESS_READ 0xA1
#define M4_IIC_EEPROM_DEVICE_ADDRESS_WRITE 0xA0

/**
 * @brief Read data from the specified address of EEPROM(M24C02)
 * 
 * @param addr Start address to be read
 * @param buf Read buff pointer
 * @param size Size to be read
 * 
 * @return void
 */
void M4_EEPROM_Read(uint8_t addr, uint8_t *buf, uint8_t size);

/**
 * @brief Write data to the specified address of EEPROM(M24C02)
 * 
 * @param addr Start address to be written
 * @param buf Written buff pointer
 * @param size Size to be Written
 * 
 * @return void
 */
void M4_EEPROM_Write(uint8_t addr, uint8_t *buf, uint8_t size);

#endif

#ifdef M4_RES_ENABLE

#define M4_IIC_RES_DEVICE_ADDRESS_READ 0x5F
#define M4_IIC_RES_DEVICE_ADDRESS_WRITE 0x5E

/**
 * @brief Read the resistance of the digital resistor(MCP4017T)
 * 
 * @return uint8_t digital resistor(MCP4017T) resistance value
 *         value range: [1, 3, 7, 15, 31, 63, 127]
 */
uint8_t M4_Res_Read(void);

/**
 * @brief Write the resistance of the digital resistor(MCP4017T)
 * 
 * @param value Resistance value to be written
 *              value range [0~127]
 * 
 * @return void
 */
void M4_Res_Write(uint8_t value);

#endif

#ifdef M4_R39IC_ENABLE

/**
 * @brief Start input capture for R39 controlled XL555 pulse
 * 
 * @return void
 */
void M4_R39IC_Start(void);

/**
 * @brief Get R39 controlled XL555 pulse period(us)
 * 
 * @return uint16_t pulse freq (Hz)
 */
uint16_t M4_R39IC_GetPeriod(void);

#endif

#ifdef M4_R40IC_ENABLE

/**
 * @brief Start input capture for R40 controlled XL555 pulse
 * 
 * @return void
 */
void M4_R40IC_Start(void);

/**
 * @brief Get R40 controlled XL555 pulse period(us)
 * 
 * @return uint16_t pulse freq (Hz)
 */
uint16_t M4_R40IC_GetPeriod(void);

#endif

#if (defined(M4_R39IC_ENABLE) || defined(M4_R39IC_ENABLE))

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

#endif

#ifdef M4_EX_ADCKEY_ENABLE

// ADC keys max sum
#define M4_EX_ADCKEY_SUM_MAX 8

#define M4_EX_ADCKEY_S1 0x01
#define M4_EX_ADCKEY_S2 0x02
#define M4_EX_ADCKEY_S3 0x03
#define M4_EX_ADCKEY_S4 0x04
#define M4_EX_ADCKEY_S5 0x05
#define M4_EX_ADCKEY_S6 0x06
#define M4_EX_ADCKEY_S7 0x07
#define M4_EX_ADCKEY_S8 0x08

#define M4_EX_ADCKEY_S1_ADCVALUE 0
#define M4_EX_ADCKEY_S2_ADCVALUE 200  // 140
#define M4_EX_ADCKEY_S3_ADCVALUE 600  // 540
#define M4_EX_ADCKEY_S4_ADCVALUE 1000 // 960
#define M4_EX_ADCKEY_S5_ADCVALUE 1400 // 1320
#define M4_EX_ADCKEY_S6_ADCVALUE 1800 // 1760
#define M4_EX_ADCKEY_S7_ADCVALUE 2300 // 2230
#define M4_EX_ADCKEY_S8_ADCVALUE 2800 // 2780

#define M4_EX_ADCKEY_UP 0x00
#define M4_EX_ADCKEY_DOWN 0x01

#define M4_EX_ADCKEY_SCAN_DELAY 15

struct ADCKey
{
    uint8_t Status;
    uint32_t TimCount;
};

typedef struct ADCKey ADCKey_t;

void M4_EX_ADCKey_Scan(void);

/**
 * @brief Rewrite by user, this function in M4_EX_ADCKey_Scan() call
 * 
 * @param KeyID Key id, value can is M4_KEY_B1
 * @param KeyStatus Now keys's status
 * @param DownTim The number of milliseconds(ms) to press the button
 * 
 * @return void
 */
void M4_EX_ADCKey_Scan_Callback(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim);

#endif

#ifdef M4_EX_SEG_ENABLE

#define M4_EX_SEG_TABLE_MAX 17

#define M4_EX_SEG_SER(b) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, (b))
#define M4_EX_SEG_SRCLK(b) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (b))
#define M4_EX_SEG_RCLK(b) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, (b))

/**
 * @brief Set M4 extensions board seg
 * 
 * @param seg1 Seg1 value in SegTable
 * @param seg2 Seg2 value in SegTable
 * @param seg3 Seg3 Value in SegTable
 * 
 * @return void
 */
void M4_EX_Seg_Set(uint8_t seg1, uint8_t seg2, uint8_t seg3);

#endif

#ifdef M4_EX_TS_ENABLE

#define M4_EX_DS18B20_GPIO_PORT GPIOA
#define M4_EX_DS18B20_GPIO_PIN GPIO_PIN_6

#define M4_EX_DS18B20_COMMAND_SKIP_ROM 0xCC
#define M4_EX_DS18B20_COMMAND_CONVERT_TEMP 0x44
#define M4_EX_DS18B20_COMMAND_READ_SCRTCHPAD 0xBE

/**
 * @brief Start Temperature Sensor DS18B20 Temperature conversion
 * 
 * @return void
 */
void M4_EX_TS_Convert(void);

/**
 * @brief Read temperature sensor DS18B20 convered value
 * 
 * @return float Temperature value (Resolution: 0.0625, Celsius)
 */
float M4_EX_TS_Read(void);

#endif

#ifdef M4_EX_THS_ENABLE

#define M4_EX_DHT11_GPIO_PORT GPIOA
#define M4_EX_DHT11_GPIO_PIN GPIO_PIN_7

/**
 * @brief Read Temperature and humidity sensor DHT11 value
 * 
 * @param temp DHT11 temperature value
 * @param humi DHT11 humidity value
 * 
 * @return void
 */
void M4_EX_THS_Read(uint8_t *temp, uint8_t *humi);

#endif

#ifdef M4_EX_LS_ENABLE

#ifndef M4_EX_LS_BASE_VOLT
#define M4_EX_LS_BASE_VOLT 3.3
#endif

#define M4_EX_LS_GPIO_PORT GPIOA
#define M4_EX_LS_GPIO_PIN GPIO_PIN_3

/**
 * @brief Get Light sensor adc analog value
 * 
 * @param void
 * 
 * @return uint16_t Light sensor analog value in 0 ~ M4_EX_LS_BASE_VOLT
 */
uint16_t M4_EX_LS_GetValueA(void);

/**
 * @brief Get Light sensor adc digital value
 * 
 * @param void
 * 
 * @return GPIO_PinState Light sensor analog value in 0 or 1
 */
GPIO_PinState M4_EX_LS_GetValueD(void);

#endif
