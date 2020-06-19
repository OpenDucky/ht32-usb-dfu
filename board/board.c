/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio
                        (C) 2018 Charlie Waters

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

#include "ch.h"
#include "hal.h"

#include <string.h>
/* ============ Private Defines ===================== */

/* ============ Function Prototypes ================== */
#define PBIT(PORT, LINE) ((PAL_PORT(LINE) == PORT) ? (1 << PAL_PAD(LINE)) : 0)
#define PAFIO_L(PORT, LINE, AF) (((PAL_PORT(LINE) == PORT) && (PAL_PAD(LINE) < 8)) ? (AF << (PAL_PAD(LINE) << 2)) : 0)
#define PAFIO_H(PORT, LINE, AF) (((PAL_PORT(LINE) == PORT) && (PAL_PAD(LINE) >= 8)) ? (AF << ((PAL_PAD(LINE) - 8) << 2)) : 0)
#define PAFIO(PORT, N, LINE, AF) ((N) ? PAFIO_H(PORT, LINE, AF) : PAFIO_L(PORT, LINE, AF))

#define OPEN_DRAIN(PORT) (\
    PBIT(PORT, LED_MASTER_DISABLE) | \
    PBIT(PORT, LED_DRV_GCLK) | \
    PBIT(PORT, LED_DRV_DATA) | \
    PBIT(PORT, LED_DRV_DCLK) | \
    PBIT(PORT, LED_DRV_DLE) | \
    PBIT(PORT, LED_DRV_Q1) | \
    PBIT(PORT, LED_DRV_Q2) | \
    PBIT(PORT, LED_DRV_Q3) | \
    PBIT(PORT, LED_DRV_Q4) | \
    PBIT(PORT, LED_DRV_Q5) | \
    PBIT(PORT, LED_DRV_Q6) | \
    PBIT(PORT, LED_DRV_Q7) | \
    PBIT(PORT, LED_DRV_Q8) | \
0)

#define OUT_BITS(PORT) (\
    PBIT(PORT, LINE_ROW1 ) | \
    PBIT(PORT, LINE_ROW2 ) | \
    PBIT(PORT, LINE_ROW3 ) | \
    PBIT(PORT, LINE_ROW4 ) | \
    PBIT(PORT, LINE_ROW5 ) | \
    PBIT(PORT, LINE_ROW6 ) | \
    PBIT(PORT, LINE_ROW7 ) | \
    PBIT(PORT, LINE_ROW8 ) | \
    PBIT(PORT, LINE_ROW9 ) | \
    PBIT(PORT, LINE_ROW10) | \
    PBIT(PORT, LINE_ROW11) | \
    PBIT(PORT, LINE_ROW12) | \
    PBIT(PORT, LINE_ROW13) | \
    PBIT(PORT, LINE_ROW14) | \
    PBIT(PORT, LINE_ROW15) | \
    PBIT(PORT, LED_MASTER_DISABLE) | \
    PBIT(PORT, LED_DRV_GCLK) | \
    PBIT(PORT, LED_DRV_DATA) | \
    PBIT(PORT, LED_DRV_DCLK) | \
    PBIT(PORT, LED_DRV_DLE) | \
    PBIT(PORT, LED_DRV_Q1) | \
    PBIT(PORT, LED_DRV_Q2) | \
    PBIT(PORT, LED_DRV_Q3) | \
    PBIT(PORT, LED_DRV_Q4) | \
    PBIT(PORT, LED_DRV_Q5) | \
    PBIT(PORT, LED_DRV_Q6) | \
    PBIT(PORT, LED_DRV_Q7) | \
    PBIT(PORT, LED_DRV_Q8) | \
0)

#define IN_BITS(PORT) (\
    PBIT(PORT, LINE_COL1) | \
    PBIT(PORT, LINE_COL2) | \
    PBIT(PORT, LINE_COL3) | \
    PBIT(PORT, LINE_COL4) | \
    PBIT(PORT, LINE_COL5) | \
    PBIT(PORT, LINE_COL6) | \
    PBIT(PORT, LINE_COL7) | \
    PBIT(PORT, LINE_COL8) | \
0)

// Alternate Functions
#define AF_BITS(PORT, N) (\
    PAFIO(PORT, N, LINE_ROW1,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW2,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW3,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW4,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW5,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW6,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW7,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW8,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW9,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW10,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW11,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW12,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW13,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW14,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_ROW15,    AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL1,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL2,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL3,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL4,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL5,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL6,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL7,     AFIO_GPIO) | \
    PAFIO(PORT, N, LINE_COL8,     AFIO_GPIO) | \
    PAFIO(PORT, N, LED_MASTER_DISABLE, AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_GCLK,  AFIO_TM) | \
    PAFIO(PORT, N, LED_DRV_DATA,  AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_DCLK,  AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_DLE,   AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q1,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q2,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q3,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q4,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q5,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q6,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q7,    AFIO_GPIO) | \
    PAFIO(PORT, N, LED_DRV_Q8,    AFIO_GPIO) | \
0)



/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
const PALConfig pal_default_config = {
    // GPIO A
    .setup[0] = {
        .DIR = OUT_BITS(IOPORTA),
        .INE = IN_BITS(IOPORTA),
        .PU = IN_BITS(IOPORTA),
        .PD = 0x0000,
        .OD = OPEN_DRAIN(IOPORTA),
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTA, 0),
        .CFG[1] = AF_BITS(IOPORTA, 1),
    },
    // GPIO B
    .setup[1] = {
        .DIR = OUT_BITS(IOPORTB),
        .INE = IN_BITS(IOPORTB),
        .PU = IN_BITS(IOPORTB),
        .PD = 0x0000,
        .OD = OPEN_DRAIN(IOPORTB),
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTB, 0),
        .CFG[1] = AF_BITS(IOPORTB, 1),
    },
    // GPIO C
    .setup[2] = {
        .DIR = OUT_BITS(IOPORTC),
        .INE = IN_BITS(IOPORTC),
        .PU = IN_BITS(IOPORTC),
        .PD = 0x0000,
        .OD = OPEN_DRAIN(IOPORTC),
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTC, 0),
        .CFG[1] = AF_BITS(IOPORTC, 1),
    },
    // GPIO D
    .setup[3] = {
        .DIR = OUT_BITS(IOPORTD),
        .INE = IN_BITS(IOPORTD),
        .PU = IN_BITS(IOPORTD),
        .PD = 0x0000,
        .OD = OPEN_DRAIN(IOPORTD),
        .DRV = 0x0000,
        .LOCK = 0x0000,
        .OUT = 0x0000,
        .CFG[0] = AF_BITS(IOPORTD, 0),
        .CFG[1] = AF_BITS(IOPORTD, 1),
    },
    .ESSR[0] = 0x00000000,
    .ESSR[1] = 0x00000000,
};

void __early_init(void) {
    ht32_clock_init();
}

/**
 * @brief   Board-specific initialization code.
 * @todo    Add your board-specific code, if any.
 */
void boardInit(void) {
}
