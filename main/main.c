/*
 * Copyright (C) 2020 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' ANDANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 *  main.c
 *
 *  Minimal main application that initializes BTstack, prepares the example and enters BTstack's Run Loop.
 *
 *  If needed, you can create other threads. Please note that BTstack's API is not thread-safe and can only be
 *  called from BTstack timers or in response to its callbacks, e.g. packet handlers.
 */

#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"
#include "hci_dump_embedded_stdout.h"
#include <FreeRTOSConfig.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include "tb6612fng/tb6612fng.h"
//#include "drive_control/drive_control.h"
#include "ps4/ps4_parser.h"

#include <stddef.h>

#define LED 2

extern int btstack_main(int argc, const char *argv[]);

// NOTE BTSTACK IS NOT THREAD SAFE
// only call btstack functions from within this thread;
// Otherwise use mutexes, queues semaphores etc.
void initRobot()
{


    // optional: enable packet logger
    //hci_dump_init(hci_dump_embedded_stdout_get_instance());
   
		// THIS ALSO INITS DRIVERS
		// eventually we want to switch to our own bluetooth implementation
		// but since we change btstack_run_loop_execute() to have a driver input
		// this will do
		btstack_init();
    btstack_main(0, NULL);

    // Enter run loop, does not return
    btstack_run_loop_execute();
}

void test() {

}

int app_main(void)
{
	//esp_task_wdt_init(30, false);

	
	gpio_pad_select_gpio(GPIO_NUM_23);
	gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);

	gpio_set_level(GPIO_NUM_23, 1);


	gpio_num_t input1 = GPIO_NUM_19;
	gpio_num_t input2 = GPIO_NUM_21;
	gpio_num_t pwm = GPIO_NUM_22;
	mcpwm_unit_t mcpwm_unit = MCPWM_UNIT_0;
	mcpwm_timer_t mcpwm_timer = MCPWM_TIMER_2;
	mcpwm_io_signals_t mcpwm_io_sig = MCPWM2A;


  gpio_pad_select_gpio(input1);
  gpio_set_direction(input1, GPIO_MODE_OUTPUT);

	gpio_set_level(input1, 1);

  gpio_pad_select_gpio(input2);
  gpio_set_direction(input2, GPIO_MODE_OUTPUT);

	gpio_set_level(input2, 0);

  mcpwm_gpio_init(mcpwm_unit, mcpwm_io_sig, pwm);

  mcpwm_config_t config;
  config.frequency = 10000;
  config.cmpr_a = 0;
  config.cmpr_b = 0;
  config.counter_mode = MCPWM_UP_COUNTER;
  config.duty_mode = MCPWM_DUTY_MODE_0;

  mcpwm_init(mcpwm_unit, mcpwm_timer, &config);

	mcpwm_set_duty(mcpwm_unit, mcpwm_timer, MCPWM_OPR_A, 99.9); 

//	xTaskCreate(initRobot, "BTINPUT", 30000, NULL, 2, NULL);
  return 0;
}	
