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
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
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
#include "hal/gpio_types.h"
#include "hci_dump.h"
#include "hci_dump_embedded_stdout.h"
#include <FreeRTOSConfig.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include "rom/gpio.h"
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
  
  // turn on standby so motor controller always takes input
  gpio_set_level(GPIO_NUM_23, 1);


	xTaskCreate(initRobot, "BTINPUT", 30000, NULL, 2, NULL);
  return 0;
}	
