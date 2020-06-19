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

#define CM3_RESET_VECTOR_OFFSET    4

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

static void jump_to_application(void) __attribute__ ((noreturn));

static void jump_to_application(void) {

    /* Use the application's vector table */
    *((volatile uint32_t*)0xe000ed08) = APP_BASE;

    /* Initialize the application's stack pointer */
    __set_MSP(*((volatile uint32_t*)(APP_BASE)));
    uint32_t target_start = *((volatile uint32_t*)(APP_BASE + CM3_RESET_VECTOR_OFFSET));
    /* Jump to the application entry point */
    __ASM volatile ("bx %0" : : "r" (target_start) : );

    
    while (1);
}


THD_WORKING_AREA(waThread2, 256);
THD_FUNCTION(Thread2, arg) {
  (void)arg;
  led_matrix_data[1]=  0xFFFF;
  while (1)
  {
    if (currentState != STATE_DFU_DNBUSY) {
      chThdSleepMilliseconds(1);
    } else {
      led_matrix_data[2]=  0xFFFF;
      chThdSleepMilliseconds(1);
      for (size_t i = 0; i < flashBufferPointer; i+=sizeof(uint32_t))
      {
        if (currentAddress % FLASH_PAGE_SIZE == 0) {
          // Issue Page Erase
          while (((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_IDLE) && ((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_FINISHED)){
            chThdSleepMilliseconds(1);
          }
          FMC->TADR = currentAddress;
          FMC->OCMR = FMC_OCMR_CMD_PAGE_ERASE;
          FMC->OPCR = FMC_OPCR_OPM_COMMIT;
          while ((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_FINISHED){
            chThdSleepMilliseconds(1);
          }
        }
        while (((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_IDLE) && ((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_FINISHED)){
            chThdSleepMilliseconds(1);
        }
        FMC->TADR = currentAddress;
        FMC->WRDR = flashBuffer[i] | ((uint32_t)flashBuffer[i+1]) << 8 | ((uint32_t)flashBuffer[i+2]) << 16 | ((uint32_t)flashBuffer[i+3]) << 24;
        FMC->OCMR = FMC_OCMR_CMD_WORD_PROGRAM;
        FMC->OPCR = FMC_OPCR_OPM_COMMIT;
        while ((FMC->OPCR & FMC_OPCR_OPM_MASK) != FMC_OPCR_OPM_FINISHED){
            chThdSleepMilliseconds(1);
        }
        currentAddress += sizeof(uint32_t);
      }
      currentState = STATE_DFU_DNLOAD_IDLE;
    }
  }
  led_matrix_data[1]=  0x00;
}

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

  
  palClearLine(LINE_ROW1);
  palSetLine(LINE_ROW2);
  palSetLine(LINE_ROW3);
  palSetLine(LINE_ROW4);
  palSetLine(LINE_ROW5);
  palSetLine(LINE_ROW6);
  palSetLine(LINE_ROW7);
  palSetLine(LINE_ROW8);
  palSetLine(LINE_ROW9);
  palSetLine(LINE_ROW10);
  palSetLine(LINE_ROW11);
  palSetLine(LINE_ROW12);
  palSetLine(LINE_ROW13);
  palSetLine(LINE_ROW14);
  palSetLine(LINE_ROW15);
  __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  if (!palReadLine(LINE_COL1)) {
    goto force_bootloader;
  }

  if (*((volatile uint32_t*)APP_BASE) != 0xFFFFFFFF) {
    jump_to_application();
  }

force_bootloader:
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  
  /* This is now the idle thread loop, you may perform here a low priority
     task but you must never try to sleep or wait in this loop. Note that
     this tasks runs at the lowest priority level so any instruction added
     here will be executed after all other tasks have been started.*/
  while (true) {
  }
}
