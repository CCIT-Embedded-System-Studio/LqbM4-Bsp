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

// Lsat scanf systick
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

__INLINE float M4_R37_GetValue(void)
{
    HAL_ADC_Start(&hadc2);
    float ret = (HAL_ADC_GetValue(&hadc2) / 4095.0) * M4_R37_BASE_VOLT;
    HAL_ADC_Stop(&hadc2);
    return ret;
}

#endif

#ifdef M4_R38_ENABLE

extern ADC_HandleTypeDef hadc1;

__INLINE float M4_R38_GetValue(void)
{
    HAL_ADC_Start(&hadc1);
    float ret = (HAL_ADC_GetValue(&hadc1) / 4095.0) * M4_R38_BASE_VOLT;
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

__INLINE void M4_Uart_Transmit(uint8_t *data, uint32_t len)
{
    HAL_UART_Transmit(&huart1, data, len, 10);
}

#endif

#ifdef M4_EEPROM_ENABLE

void M4_EEPROM_Read(uint8_t addr, uint8_t *buf, uint8_t size)
{
    I2CStart();
    I2CSendByte(M4_IIC_EEPROM_DEVICE_ADDRESS_WRITE);
    I2CWaitAck();

    I2CSendByte(addr);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(M4_IIC_EEPROM_DEVICE_ADDRESS_READ);
    I2CWaitAck();

    while (1)
    {
        *buf = I2CReceiveByte();
        if (size == 0)
            break;
        else
            I2CSendAck();
        --size;
        ++buf;
    }
    I2CSendNotAck();
    I2CStop();
}

void M4_EEPROM_Write(uint8_t addr, uint8_t *buf, uint8_t size)
{
    I2CStart();
    I2CSendByte(M4_IIC_EEPROM_DEVICE_ADDRESS_WRITE);
    I2CWaitAck();

    I2CSendByte(addr);
    I2CWaitAck();
    while (size != 0 && (*buf != '\0'))
    {
        I2CSendByte(*buf);
        I2CWaitAck();
        --size;
        ++buf;
    }

    I2CStop();
    HAL_Delay(5);
}

#endif

#ifdef M4_RES_ENABLE

__INLINE uint8_t M4_Res_Read(void)
{
    I2CStart();
    I2CSendByte(M4_IIC_RES_DEVICE_ADDRESS_READ);
    I2CWaitAck();

    uint8_t value = I2CReceiveByte();
    I2CSendNotAck();
    I2CStop();

    return value;
}

__INLINE void M4_Res_Write(uint8_t value)
{
    I2CStart();
    I2CSendByte(M4_IIC_RES_DEVICE_ADDRESS_WRITE);
    I2CWaitAck();

    I2CSendByte(value);
    I2CWaitAck();
    I2CStop();
}

#endif

#ifdef M4_R39IC_ENABLE

extern TIM_HandleTypeDef htim16;

static uint32_t R39ICLastTick = 0;
static uint16_t R39ICPeriod = 0;

__INLINE void M4_R39IC_Start(void)
{
    __HAL_TIM_SET_PRESCALER(&htim16, 80 - 1);
    HAL_TIM_IC_Start_IT(&htim16, TIM_CHANNEL_1);
}

__INLINE void M4_R39IC_Stop(void)
{
    HAL_TIM_IC_Stop_IT(&htim16, TIM_CHANNEL_1);
}

__INLINE uint16_t M4_R39IC_GetPeriod(void)
{
    return R39ICPeriod;
}

#endif

#ifdef M4_R40IC_ENABLE

extern TIM_HandleTypeDef htim8;

static uint32_t R40ICLastTick = 0;
static uint16_t R40ICPeriod = 0;

__INLINE void M4_R40IC_Start(void)
{
    __HAL_TIM_SET_PRESCALER(&htim8, 80 - 1);
    HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_1);
}

__INLINE void M4_R40IC_Stop(void)
{
    HAL_TIM_IC_Stop_IT(&htim8, TIM_CHANNEL_1);
}

__INLINE uint16_t M4_R40IC_GetPeriod(void)
{
    return R40ICPeriod;
}

#endif

#if (defined(M4_R39IC_ENABLE) || defined(M4_R39IC_ENABLE))

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    uint32_t NowTick = 0;
#ifdef M4_R39IC_ENABLE
    if (htim == &htim16)
    {
        NowTick = HAL_TIM_ReadCapturedValue(&htim16, TIM_CHANNEL_1);
        R39ICPeriod = ABS((long)NowTick - (long)R39ICLastTick);
        R39ICLastTick = NowTick;
    }
#endif
#ifdef M4_R40IC_ENABLE
    if (htim == &htim8)
    {
        NowTick = HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_1);
        R40ICPeriod = ABS((long)NowTick - (long)R40ICLastTick);
        R40ICLastTick = NowTick;
    }
#endif
}

#endif

#ifdef M4_EX_ADCKEY_ENABLE

uint16_t ADCKeyValueArr[M4_EX_ADCKEY_SUM_MAX] = {
    M4_EX_ADCKEY_S1_ADCVALUE,
    M4_EX_ADCKEY_S2_ADCVALUE,
    M4_EX_ADCKEY_S3_ADCVALUE,
    M4_EX_ADCKEY_S4_ADCVALUE,
    M4_EX_ADCKEY_S5_ADCVALUE,
    M4_EX_ADCKEY_S6_ADCVALUE,
    M4_EX_ADCKEY_S7_ADCVALUE,
    M4_EX_ADCKEY_S8_ADCVALUE};

ADCKey_t ADCKeyList[M4_EX_ADCKEY_SUM_MAX] = {0};

// Lsat scanf systick
static uint32_t ADCKeyLastTick = 0;

__STATIC_INLINE uint16_t M4_EX_ADCKey_GetValue(void)
{

    HAL_ADC_Start(&hadc2);
    uint16_t ret = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_Stop(&hadc2);
    return ret;
}

void M4_EX_ADCKey_Scan(void)
{
    uint32_t ADCKeyNowTick = HAL_GetTick();
    uint16_t ADCKeyValue = M4_EX_ADCKey_GetValue();

    if (ADCKeyNowTick - ADCKeyLastTick < M4_EX_ADCKEY_SCAN_DELAY)
        return;

    for (size_t i = 0; i < M4_EX_ADCKEY_SUM_MAX; i++)
    {
        if (ADCKeyValue <= ADCKeyValueArr[i])
        {
            // Now key down
            if (ADCKeyList[i].Status == M4_EX_ADCKEY_DOWN)
            {
                ADCKeyList[i].TimCount += ABS(ADCKeyNowTick - ADCKeyLastTick);
                M4_EX_ADCKey_Scan_Callback(i + 1, M4_EX_ADCKEY_DOWN, ADCKeyList[i].TimCount);
            }
            ADCKeyList[i].Status = M4_EX_ADCKEY_DOWN;
            ADCKeyValue = 0xffff;
        }
        else
        {
            // Now key up
            if (ADCKeyList[i].Status == M4_EX_ADCKEY_DOWN)
            {
                M4_EX_ADCKey_Scan_Callback(i + 1, M4_EX_ADCKEY_UP, ADCKeyList[i].TimCount);
                ADCKeyList[i].TimCount = 0;
            }
            ADCKeyList[i].Status = M4_EX_ADCKEY_UP;
        }
    }
    ADCKeyLastTick = ADCKeyNowTick;
}

__weak void M4_EX_ADCKey_Scan_Callback(uint8_t KeyID, uint8_t KeyStatus, uint32_t DownTim){};

#endif

#ifdef M4_EX_SEG_ENABLE

static uint8_t SegTable[17] =
    {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};

void M4_EX_Seg_Set(uint8_t seg1, uint8_t seg2, uint8_t seg3)
{
    uint32_t seg = (SegTable[seg3 % M4_EX_SEG_TABLE_MAX] << 16) |
                   (SegTable[seg2 % M4_EX_SEG_TABLE_MAX] << 8) |
                   (SegTable[seg1 % M4_EX_SEG_TABLE_MAX]);

    for (size_t i = 0; i < 24; i++)
    {
        // SER 1bit data
        M4_EX_SEG_SER((seg & (0x800000U >> i)) ? 1 : 0);
        // SRCLK rising edge
        M4_EX_SEG_SRCLK(0);
        M4_EX_SEG_SRCLK(1);
    }
    // RCLK rising edge
    M4_EX_SEG_RCLK(0);
    M4_EX_SEG_RCLK(1);
}

#endif

#if (defined(M4_EX_TS_ENABLE) || defined(M4_EX_THS_ENABLE))

__STATIC_INLINE void Delay_us(uint32_t t)
{
    for (size_t i = 0; i < t * 16; i++)
        __NOP();
}

__STATIC_INLINE void OneWire_Gpio_In(GPIO_TypeDef *Port, uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = Pin;
    HAL_GPIO_Init(Port, &GPIO_InitStruct);
}

__STATIC_INLINE void OneWire_Gpio_Out(GPIO_TypeDef *Port, uint16_t Pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = Pin;
    HAL_GPIO_Init(Port, &GPIO_InitStruct);
}

__STATIC_INLINE void OneWire_Gpio_Write(GPIO_TypeDef *Port, uint16_t Pin, GPIO_PinState State)
{
    HAL_GPIO_WritePin(Port, Pin, State);
}

__STATIC_INLINE GPIO_PinState OneWire_Gpio_Read(GPIO_TypeDef *Port, uint16_t Pin)
{
    return HAL_GPIO_ReadPin(Port, Pin);
}

#endif

#ifdef M4_EX_TS_ENABLE

#define DS18B20_GPIO_IN() OneWire_Gpio_In(M4_EX_DS18B20_GPIO_PORT, M4_EX_DS18B20_GPIO_PIN)
#define DS18B20_GPIO_OUT() OneWire_Gpio_Out(M4_EX_DS18B20_GPIO_PORT, M4_EX_DS18B20_GPIO_PIN)

#define DS18B20_GPIO_READ() OneWire_Gpio_Read(M4_EX_DS18B20_GPIO_PORT, M4_EX_DS18B20_GPIO_PIN)
#define DS18B20_GPIO_WRITE(x) OneWire_Gpio_Write(M4_EX_DS18B20_GPIO_PORT, M4_EX_DS18B20_GPIO_PIN, x)

__STATIC_INLINE void DS18B20_Reset(void)
{
    uint8_t Timout = 10;
    DS18B20_GPIO_OUT();
    DS18B20_GPIO_WRITE(GPIO_PIN_RESET);
    Delay_us(480);
    DS18B20_GPIO_WRITE(GPIO_PIN_SET);
    // Wait ACK
    DS18B20_GPIO_IN();
    Delay_us(60);
    while (DS18B20_GPIO_READ() != GPIO_PIN_RESET)
    {
        Delay_us(1);
        --Timout;
        if (Timout == 0)
            return;
    }

    Delay_us(270);
}

__STATIC_INLINE void DS18B20_Write_Byte(uint8_t data)
{
    DS18B20_GPIO_OUT();
    for (size_t i = 0; i < 8; i++)
    {
        DS18B20_GPIO_WRITE(GPIO_PIN_RESET);
        Delay_us(15);
        DS18B20_GPIO_WRITE(
            (data & (0x01 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        Delay_us(15);
        DS18B20_GPIO_WRITE(GPIO_PIN_SET);
    }
}

__STATIC_INLINE uint8_t DS18B20_Read_Byte(void)
{
    uint8_t data = 0x00;

    for (size_t i = 0; i < 8; i++)
    {
        DS18B20_GPIO_OUT();
        DS18B20_GPIO_WRITE(GPIO_PIN_RESET);
        Delay_us(1);
        DS18B20_GPIO_WRITE(GPIO_PIN_SET);
        DS18B20_GPIO_IN();
        Delay_us(15);
        data |= (DS18B20_GPIO_READ() << i);
        Delay_us(60);
    }

    return data;
}

void M4_EX_TS_Convert(void)
{
    DS18B20_Reset();
    DS18B20_Write_Byte(M4_EX_DS18B20_COMMAND_SKIP_ROM);
    DS18B20_Write_Byte(M4_EX_DS18B20_COMMAND_CONVERT_TEMP);
}

float M4_EX_TS_Read(void)
{
    DS18B20_Reset();
    DS18B20_Write_Byte(M4_EX_DS18B20_COMMAND_SKIP_ROM);
    DS18B20_Write_Byte(M4_EX_DS18B20_COMMAND_READ_SCRTCHPAD);

    uint8_t low = DS18B20_Read_Byte();
    uint8_t high = DS18B20_Read_Byte();

    float ret = (high << 8 | low) * 0.0625;
    return ret;
}

#endif

#ifdef M4_EX_THS_ENABLE

#define DHT11_GPIO_IN() OneWire_Gpio_In(M4_EX_DHT11_GPIO_PORT, M4_EX_DHT11_GPIO_PIN)
#define DHT11_GPIO_OUT() OneWire_Gpio_Out(M4_EX_DHT11_GPIO_PORT, M4_EX_DHT11_GPIO_PIN)

#define DHT11_GPIO_READ() OneWire_Gpio_Read(M4_EX_DHT11_GPIO_PORT, M4_EX_DHT11_GPIO_PIN)
#define DHT11_GPIO_WRITE(x) OneWire_Gpio_Write(M4_EX_DHT11_GPIO_PORT, M4_EX_DHT11_GPIO_PIN, x)

__STATIC_INLINE void DHT11_Reset(void)
{
    uint16_t Timout = 1000;

    DHT11_GPIO_OUT();
    // DHT11_GPIO_WRITE(GPIO_PIN_SET);
    DHT11_GPIO_WRITE(GPIO_PIN_RESET);
    HAL_Delay(18);
    DHT11_GPIO_WRITE(GPIO_PIN_SET);
    Delay_us(20);
    // Check DHT11 response
    DHT11_GPIO_IN();
    while (DHT11_GPIO_READ() != GPIO_PIN_RESET)
    {
        Delay_us(1);
        --Timout;
        if (Timout == 0)
            return;
    }
    Timout = 1000;
    while (DHT11_GPIO_READ() != GPIO_PIN_SET)
    {
        Delay_us(1);
        --Timout;
        if (Timout == 0)
            return;
    }
    Timout = 1000;
    while (DHT11_GPIO_READ() != GPIO_PIN_RESET)
    {
        Delay_us(1);
        --Timout;
        if (Timout == 0)
            return;
    }
}

__STATIC_INLINE uint8_t DHT11_Read_Byte(void)
{
    uint16_t Timout = 1000;
    uint8_t data = 0x00;

    for (size_t i = 0; i < 8; i++)
    {
        while (DHT11_GPIO_READ() != GPIO_PIN_SET)
        {
            Delay_us(1);
            --Timout;
            if (Timout == 0)
                return 0x00;
        }
        // Check data
        uint32_t count = 0;
        while (DHT11_GPIO_READ() != GPIO_PIN_RESET)
            ++count;
        // Recvice 1 bit data
        if (count > 300)
            data |= (0x80 >> i);
    }

    return data;
}

void M4_EX_THS_Read(uint8_t *temp, uint8_t *humi)
{
    uint8_t data[4] = {0};

    DHT11_Reset();
    for (size_t i = 0; i < 4; i++)
        data[i] = DHT11_Read_Byte();
    *humi = data[0];
    *temp = data[2];
}

#endif

#ifdef M4_EX_LS_ENABLE

extern ADC_HandleTypeDef hadc2;

__INLINE uint16_t M4_EX_LS_GetValueA(void)
{
    HAL_ADC_Start(&hadc2);
    uint16_t ret = (HAL_ADC_GetValue(&hadc2) / 4095.0) * M4_R37_BASE_VOLT;
    HAL_ADC_Stop(&hadc2);
    return ret;
}

__INLINE GPIO_PinState M4_EX_LS_GetValueD(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = M4_EX_LS_GPIO_PIN;
    HAL_GPIO_Init(M4_EX_LS_GPIO_PORT, &GPIO_InitStruct);
    return HAL_GPIO_ReadPin(M4_EX_LS_GPIO_PORT, M4_EX_LS_GPIO_PIN);
}

#endif
