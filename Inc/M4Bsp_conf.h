/**
 * @file M4Bsp_conf.h
 * @author RealEyex (none)
 * @brief This content of this file is the macro definition related to the board
 * @version 0.1
 * @date 2021-04-22
 * 
 * @copyright Copyright (c) 2021 CCIT Embedded System Studio
 * 
 * @SPDX-License-Identifier Apache-2.0
 */

/*********** Main board function enable ***********/
/******* LED function(LD1,LD2,LD3,LD4,LD5,LD6,LD7,LD8) *******/
#define M4_LED_ENABLE
/******* Key function(B1,B2,B3,B4) *******/
#define M4_KEY_ENABLE
/******* R37 resistance function *******/
#define M4_R37_ENABLE 
/******* R38 resistance function *******/
#define M4_R38_ENABLE
/******* UART function *******/
#define M4_UART_ENABLE
/******* EEPROM M24C02 function *******/
#define M4_EEPROM_ENABLE
/******* Digital resistance MCP4017T function *******/
#define M4_RES_ENABLE
/******* R39 controlled XL555 input capture function *******/
#define M4_R39IC_ENABLE
/******* R40 controlled XL555 input capture function *******/
#define M4_R40IC_ENABLE

/*********** Expansion board function enable ***********/
/******* ADC Key function(S1,S2,S3,S4,S5,S6,S7,S8) *******/
#define M4_EX_ADCKEY_ENABLE
