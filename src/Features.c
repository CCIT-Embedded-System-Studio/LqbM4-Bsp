#include "Features.h"

volatile uint8_t TimeList[TIME_LIST_MAX][TIME_SIZE] = {};

volatile uint8_t NowTime[TIME_SIZE] = {};

volatile uint8_t Status = STATUS_STANDBY;

// 0~2 TIME_H TIME_M TIME_S
volatile uint8_t NowChoose = TIME_H;
// 0~4 Timelist
volatile uint8_t NowListItem = 0;

volatile uint8_t KeyClick = 0x00;

volatile uint32_t tim_ms = 0;
volatile uint32_t KeyTim = 0;
volatile uint32_t LEDTim = 0;

extern TIM_HandleTypeDef htim3;

const char status_str[STATUS_SUM_MAX][20] = {
    "      Running     ",
    "      Standby     ",
    "       Pause      ",
    "      Setting     "};

struct KeyInfo KeyList[KEY_SUM_MAX] = {
    {GPIOB, GPIO_PIN_0, KEY_UP, 0x00},
    {GPIOB, GPIO_PIN_1, KEY_UP, 0x00},
    {GPIOB, GPIO_PIN_2, KEY_UP, 0x00},
    {GPIOA, GPIO_PIN_0, KEY_UP, 0x00}};

Callback KeyCallback[KEY_SUM_MAX * 2] = {
    B1_short,
    B2_short,
    B3_short,
    B4_short,
    B1_long,
    B2_long,
    B3_long,
    B4_long};

void LED_Set(uint8_t led)
{
    HAL_GPIO_WritePin(GPIOC, LED_ALL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC, led << 8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}

void LED_Flash(void)
{
    if (Status != STATUS_RUNNING)
    {
        LED_Set(0x00);
        return;
    }
    if (LEDTim > LED_FLASH_INTERVAL * 2)
        LEDTim = 0;
    LED_Set((LEDTim < LED_FLASH_INTERVAL) ? 0x01 : 0x00);
}

void LCD_UInit(void)
{
    LCD_Init();
    LCD_Clear(White);
    LCD_SetBackColor(White);
    LCD_SetTextColor(Black);
}

void Tim_Update(void)
{
    if (tim_ms < 1000)
        return;
    tim_ms = tim_ms % 1000;
    if (NowTime[TIME_S] == 0)
    {
        if (NowTime[TIME_M] == 0)
        {
            if (NowTime[TIME_H] == 0)
            {
                Status = STATUS_STANDBY;
            }
            else
            {
                --NowTime[TIME_H];
                NowTime[TIME_M] = 59;
                NowTime[TIME_S] = 59;
            }
        }
        else
        {
            --NowTime[TIME_M];
            NowTime[TIME_S] = 59;
        }
    }
    else
    {
        --NowTime[TIME_S];
    }
}

void LCD_Show(void)
{
    char str[30] = "   No 1           ";
    str[6] += NowListItem;
    LCD_DisplayStringLine(Line1, (u8 *)str);

    uint8_t *p = (Status == STATUS_RUNNING || Status == STATUS_PAUSE) ? (uint8_t *)NowTime : (uint8_t *)TimeList[NowListItem];

    sprintf(str, "     %02d:%02d:%02d     ", p[TIME_H], p[TIME_M], p[TIME_S]);

    u32 i = 0;
    u16 refcolumn = 319; //319;
    char *ptr = str;

    while ((*ptr != 0) && (i < 20)) //	20
    {
        if (i == 5 + 3 * NowChoose && Status == STATUS_SETTING)
        {
            LCD_SetBackColor(Black);
            LCD_SetTextColor(White);
        }
        LCD_DisplayChar(Line4, refcolumn, *ptr);
        refcolumn -= 16;
        ptr++;
        i++;
        if (i == 7 + 3 * NowChoose && Status == STATUS_SETTING)
        {
            LCD_SetBackColor(White);
            LCD_SetTextColor(Black);
        }
    }

    LCD_DisplayStringLine(Line6, (u8 *)status_str[Status]);
}

uint8_t M24c02_Read(uint8_t addr)
{
    uint8_t val = 0;
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();

    I2CSendByte(addr);
    I2CWaitAck();

    I2CStart();
    I2CSendByte(0xA1);
    I2CWaitAck();
    val = I2CReceiveByte();
    I2CWaitAck();
    I2CStop();

    return val;
}

void M24c02_Write(uint8_t addr, uint8_t val)
{
    I2CStart();
    I2CSendByte(0xA0);
    I2CWaitAck();

    I2CSendByte(addr);
    I2CWaitAck();
    I2CSendByte(val);
    I2CWaitAck();
    I2CStop();

    HAL_Delay(5);
}

void Get_TimeList(uint8_t item)
{
    for (size_t i = 0; i < 3; i++)
        TimeList[item][i] = M24c02_Read(item * 3 + i);
}

void Set_TImeList(uint8_t item)
{
    for (size_t i = 0; i < 3; i++)
        M24c02_Write(item * 3 + i, TimeList[item][i]);
}

void PWM_Output(void)
{
    if (Status == STATUS_RUNNING)
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    else
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

void Key_Scan(void)
{
    if (KeyTim < 20)
        return;
    KeyTim = 0;
    for (size_t i = 0; i < KEY_SUM_MAX; i++)
    {
        if (HAL_GPIO_ReadPin(KeyList[i].port, KeyList[i].pin) == GPIO_PIN_SET)
        {
            if (KeyList[i].status == KEY_DOWN)
            {
                if (KeyList[i].count < KEY_LONG_DOWN_MAX)
                {
                    // Short
                    KeyClick = 0x01 << i;
                }
                else
                {
                    // Long
                    // B2
                    if (i != 2)
                        KeyClick = 0x10 << i;
                }
                KeyList[i].status = KEY_UP;
                KeyList[i].count = 0;
            }
        }
        else
        {
            if (KeyList[i].count <= KEY_LONG_DOWN_MAX)
            {
                ++KeyList[i].count;
            }
            else
            {
                // B3
                if (i == 2)
                    KeyClick = 0x10 << i;
            }

            KeyList[i].status = KEY_DOWN;
        }
    }
}

void Key_Handle(void)
{
    for (size_t i = 0; i < KEY_SUM_MAX * 2; i++)
    {
        if (KeyClick == 0x01 << i)
            KeyCallback[i]();
    }

    KeyClick = 0x00;
}

void B1_short(void)
{
    NowListItem = (NowListItem + 1) % 5;
}

void B2_short(void)
{
    if (Status == STATUS_SETTING)
    {
        NowChoose = (NowChoose + 1) % 3;
    }
    else
    {
        Status = STATUS_SETTING;
        NowChoose = TIME_H;
    }
}

void B3_short(void)
{
    if (Status != STATUS_SETTING)
        return;
    TimeList[NowListItem][NowChoose] = (TimeList[NowListItem][NowChoose] < ((NowChoose > TIME_H) ? 59 : 23)) ? TimeList[NowListItem][NowChoose] + 1 : 0;
}

void B4_short(void)
{
    if (Status == STATUS_SETTING || Status == STATUS_STANDBY)
    {
        NowTime[TIME_H] = TimeList[NowListItem][TIME_H];
        NowTime[TIME_M] = TimeList[NowListItem][TIME_M];
        NowTime[TIME_S] = TimeList[NowListItem][TIME_S];
    }

    Status = (Status == STATUS_RUNNING) ? STATUS_PAUSE : STATUS_RUNNING;
}

void B1_long(void)
{
    ;
}

void B2_long(void)
{
    if (Status == STATUS_SETTING)
    {
        Set_TImeList(NowListItem);
        Status = STATUS_STANDBY;
    }
}

void B3_long(void)
{
    B3_short();
}

void B4_long(void)
{
    Status = STATUS_STANDBY;
}
