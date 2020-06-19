/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

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

#include "hal.h"
#include "ch.h"
#include "board/board.h"
#include "string.h"
#include "mbi5042.h"
#include "usbdfu.h"

uint16_t led_matrix_data [8*16] = {
    0x7FF,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static mbi5042_t led_controller;
static PWMConfig pwmcfg = {
  20000000,                                /* 10kHz PWM clock frequency.     */
  20,                                    /* Initial PWM period 1S.         */
  NULL,                                     /* Period callback.               */
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},          /* CH0 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL},             /* CH1 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL},             /* CH2 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL}              /* CH3 mode and callback.         */
  }
};
/*
 * Thread 1.
 */
THD_WORKING_AREA(waThread1, 128);
THD_FUNCTION(Thread1, arg) {

  (void)arg;

    // LED_MASTER_EN
  palSetLine(LED_MASTER_DISABLE);
  mbi5042_init(&led_controller, LED_DRV_DCLK, LED_DRV_DLE, LED_DRV_DATA);
  palClearLine(LED_DRV_Q1);
  palSetLine(LED_DRV_Q2);
  palSetLine(LED_DRV_Q3);
  palSetLine(LED_DRV_Q4);
  palSetLine(LED_DRV_Q5);
  palSetLine(LED_DRV_Q6);
  palSetLine(LED_DRV_Q7);
  palSetLine(LED_DRV_Q8);
  palClearLine(LED_MASTER_DISABLE);

  mbi5042_configure(&led_controller, 0x86B0);
  // wait_ms(1);
  // mbi5042_flush_data(&led_controller, led_matrix_data);
  // Setup PWM
  pwmStart(&PWMD_GPTM1, &pwmcfg);
  pwmEnableChannel(&PWMD_GPTM1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD_GPTM1, 5000));

  usbDisconnectBus(&USBD1);
  chThdSleepMilliseconds(1500);
  usbStart(&USBD1, &usbcfg);
  usbConnectBus(&USBD1);


  while (true) {
    chThdSleepMilliseconds(200);
    mbi5042_flush_data(&led_controller, led_matrix_data);
    palToggleLine(LED_DRV_Q1);
  }
}


/*
 * Application entry point.
 */
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  
  /* This is now the idle thread loop, you may perform here a low priority
     task but you must never try to sleep or wait in this loop. Note that
     this tasks runs at the lowest priority level so any instruction added
     here will be executed after all other tasks have been started.*/
  while (true) {
  }
}
