/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef BOARD_H
#define BOARD_H

/*
 * Board identifier.
 */
#define MK_NIGHT_TYPIST
#define BOARD_NAME                  "MK_NIGHT_TYPIST"

#define HT32F1654

#define FLASH_SIZE 0x10000      // 64kB

/*
 * I/O
 */

// outputs (rows are pulled low)
#define LINE_ROW1                   PAL_LINE(IOPORTC,  3)
#define LINE_ROW2                   PAL_LINE(IOPORTA, 10)
#define LINE_ROW3                   PAL_LINE(IOPORTC,  4)
#define LINE_ROW4                   PAL_LINE(IOPORTB,  6)
#define LINE_ROW5                   PAL_LINE(IOPORTD,  0)
#define LINE_ROW6                   PAL_LINE(IOPORTB, 11)
#define LINE_ROW7                   PAL_LINE(IOPORTA, 11)
#define LINE_ROW8                   PAL_LINE(IOPORTA,  0)
#define LINE_ROW9                   PAL_LINE(IOPORTA,  1)
#define LINE_ROW10                  PAL_LINE(IOPORTA,  2)
#define LINE_ROW11                  PAL_LINE(IOPORTA,  3)
#define LINE_ROW12                  PAL_LINE(IOPORTB,  1)
#define LINE_ROW13                  PAL_LINE(IOPORTA,  4)
#define LINE_ROW14                  PAL_LINE(IOPORTC, 10)
#define LINE_ROW15                  PAL_LINE(IOPORTC, 12)

// inputs (columns are sampled)
#define LINE_COL1                   PAL_LINE(IOPORTC,  9)
#define LINE_COL2                   PAL_LINE(IOPORTA,  5)
#define LINE_COL3                   PAL_LINE(IOPORTA,  6)
#define LINE_COL4                   PAL_LINE(IOPORTA,  7)
#define LINE_COL5                   PAL_LINE(IOPORTC, 11)
#define LINE_COL6                   PAL_LINE(IOPORTC, 13)
#define LINE_COL7                   PAL_LINE(IOPORTC, 14)
#define LINE_COL8                   PAL_LINE(IOPORTC, 15)

// LED Matrix
#define LED_MASTER_DISABLE                                 PAL_LINE(IOPORTC,  1)
#define LED_DRV_GCLK                                       PAL_LINE(IOPORTC,  0)
#define LED_DRV_DATA                                       PAL_LINE(IOPORTC,  2)
#define LED_DRV_DCLK                                       PAL_LINE(IOPORTA, 14)
#define LED_DRV_DLE                                        PAL_LINE(IOPORTA, 15)
#define LED_DRV_Q1                                         PAL_LINE(IOPORTC,  8)
#define LED_DRV_Q2                                         PAL_LINE(IOPORTC,  7)
#define LED_DRV_Q3                                         PAL_LINE(IOPORTB,  5)
#define LED_DRV_Q4                                         PAL_LINE(IOPORTB,  4)
#define LED_DRV_Q5                                         PAL_LINE(IOPORTB,  3)
#define LED_DRV_Q6                                         PAL_LINE(IOPORTB,  2)
#define LED_DRV_Q7                                         PAL_LINE(IOPORTC,  6)
#define LED_DRV_Q8                                         PAL_LINE(IOPORTC,  5)

// USART (conflicts with LED matrix)
// #define PAD_USART_TX                8

// #define LINE_SEL1                   PAL_LINE(IOPORTA, 8)
// #define LINE_SEL2                   PAL_LINE(IOPORTA, 9)

// SPI
// #define LINE_SPI_SCK                PAL_LINE(IOPORTB, 7)
// #define LINE_SPI_MOSI               PAL_LINE(IOPORTB, 8)
// #define LINE_SPI_MISO               PAL_LINE(IOPORTB, 9)
// #define LINE_SPI_CS                 PAL_LINE(IOPORTB, 10)

// DIP switches
// #define LINE_DIP1_ROW               LINE_ROW9
// #define LINE_DIP1_COL               LINE_COL7
// #define LINE_DIP2_ROW               LINE_ROW9
// #define LINE_DIP2_COL               LINE_COL3
// #define LINE_DIP3_ROW               LINE_ROW9
// #define LINE_DIP3_COL               LINE_COL2
// #define LINE_DIP4_ROW               LINE_ROW9
// #define LINE_DIP4_COL               LINE_COL1

// Wipe signal/switch
// #define HAS_WIPE_SWITCH             1
// #define WIPE_SWITCH_ROW             LINE_DIP4_ROW
// #define WIPE_SWITCH_COL             LINE_DIP4_COL

// #define LINE_TPPWR                  PAL_LINE(IOPORTA, 10)   // !
// #define LINE_BLPWM                  PAL_LINE(IOPORTA, 14)   // !
// #define LINE_BLEN                   PAL_LINE(IOPORTA, 15)   // !
// #define LINE_PB0                    PAL_LINE(IOPORTB, 0)    // !
// #define LINE_LED65                  PAL_LINE(IOPORTB, 1)    // !
// #define LINE_TPREQ                  PAL_LIN`E(IOPORTC, 11)   // !

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
    void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */
