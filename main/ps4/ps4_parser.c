#include <FreeRTOSConfig.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ps4_parser.h"
#include <esp_log.h>

#define LED 2

int led_state = 0;

typedef enum {
	LEFT_JOYSTICK_X = 9,
	LEFT_JOYSTICK_Y = 10,
	RIGHT_JOYSTICK_X = 11,
	RIGHT_JOYSTICK_Y = 12,
	QUAD_BUTTONS = 13,
	RL_BUTTONS = 14,
	LEFT_TRIGGER_INDEX = 16,
	RIGHT_TRIGGER_INDEX = 17,
} ps4_packet_index; 

typedef enum {
	LEFT_TRIGGER,
	RIGHT_TRIGGER
} trigger_t;

typedef enum {
	LEFT_JOYSTICK,
	RIGHT_JOYSTICK
} joystick_t;

void _parse_shape_buttons(uint8_t button_byte, controller_state *ps4_state) 
{

	ps4_state->square_button = (button_byte & 0b00010000) != 0 ? true : false; 
	ps4_state->x_button = (button_byte & 0b00100000) != 0 ? true : false; 
	ps4_state->circle_button = (button_byte & 0b01000000) != 0 ? true : false; 
	ps4_state->triangle_button = (button_byte & 0b10000000) != 0 ? true : false; 
};

/*
 * the dpad info is stored in the lower nibble of the bit 
 * it works almost like a clock, where 0 means the upper button is pressed
 * 1 means the upper and the right are pressed and 2 means the right button is pressed 
 */ 

void _parse_d_pad(uint8_t button_byte, controller_state *ps4_state) 
{

	// dpad values are in lower nibble, clear upper nibble to make it easier to parse
	button_byte = button_byte & 0b00001111;

	ps4_state->up_d_pad = (button_byte < 0x2 || button_byte == 0x7) ? true : false;
	ps4_state->right_d_pad = (button_byte > 0x0 && button_byte < 0x4) ? true : false;
	ps4_state-> down_d_pad = (button_byte > 0x2 && button_byte < 0x6) ? true : false;
	ps4_state->left_d_pad = (button_byte > 0x4 && button_byte < 0x8) ? true : false;
}

void _parse_trigger_button(uint8_t trigger_byte, controller_state *ps4_state, trigger_t trigger) {
	
	uint8_t trigger_percent = trigger_byte * 100 / 0xFF;
	
	switch(trigger) {
		case LEFT_TRIGGER :
			ps4_state->left_trigger_percent = trigger_percent;
			break;
		case RIGHT_TRIGGER :
			ps4_state->right_trigger_percent= trigger_percent;
			break;
	}
}

void _parse_joystick(uint8_t y_axis, uint8_t x_axis, controller_state *ps4_state, joystick_t joystick) 
{


	int8_t y_axis_percent = -1 * ((y_axis - 0x80) * 0x64) / 0xFF;
	int8_t x_axis_percent = ((x_axis - 0x80) * 0x64) / 0xFF;

	switch(joystick) {
		case LEFT_JOYSTICK :
			ps4_state->left_joystick_x_axis = x_axis_percent;
			ps4_state->left_joystick_y_axis = y_axis_percent;
			break;
		case RIGHT_JOYSTICK :
			ps4_state->right_joystick_x_axis = x_axis_percent;
			ps4_state->right_joystick_y_axis = y_axis_percent;
			break;
	}
}

void _parse_rl_buttons(controller_state *ps4_state, int8_t rl_state) {

	if(rl_state == 2) {
		ps4_state->rButton = true;
		ps4_state->lButton = false;
	} else if (rl_state == 1) {
		ps4_state->rButton = false;
		ps4_state->lButton = true;
	} else if(rl_state == 3) {
		ps4_state->rButton = true;
		ps4_state->lButton = true; 
	} else {
		ps4_state->rButton = false;
		ps4_state->lButton = false;
	}
}

void blink(int on)
{
	
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

	led_state = on;
    gpio_set_level(LED, on);
}

void print_controller_state(controller_state *ps4_state) {

	// buttons
/*	 printf("square_button : %x x_button : %x circle_button : %x triangle_button : %x \nleft : %x down : %x  right : %x up : %x\n "
			,ps4_state->square_button, ps4_state->x_button, ps4_state->triangle_button, ps4_state->circle_button,
			ps4_state->left_d_pad, ps4_state->down_d_pad, ps4_state->right_d_pad, ps4_state->up_d_pad); 	
*/
	// RL buttons
	printf("r_button : %x l_button : %x", ps4_state->rButton, ps4_state->lButton);
/*
	// triggers
	 printf("right_trigger : %x left_trigger : %x", ps4_state->right_trigger_percent, ps4_state->left_trigger_percent);
	
	// joysticks
	 printf("right joysticks x-axis : %i y-axis : %i", ps4_state->right_joystick_x_axis, ps4_state->right_joystick_y_axis);
	 printf("left joysticks x-axis : %i y-axis : %i", ps4_state->left_joystick_x_axis, ps4_state->left_joystick_y_axis);
*/
 };

// Parse bluetooth packet from PS4 controller, call robot control functions from here
controller_state ps4_parse_packet(uint8_t *packet, uint16_t size)
{

	controller_state ps4_state;
	_parse_shape_buttons(packet[QUAD_BUTTONS], &ps4_state);

	_parse_d_pad(packet[QUAD_BUTTONS], &ps4_state);

	_parse_rl_buttons(&ps4_state, packet[RL_BUTTONS]);

	_parse_trigger_button(packet[RIGHT_TRIGGER_INDEX], &ps4_state, RIGHT_TRIGGER);
	_parse_trigger_button(packet[LEFT_TRIGGER_INDEX], &ps4_state, LEFT_TRIGGER);

	_parse_joystick(packet[LEFT_JOYSTICK_Y], packet[LEFT_JOYSTICK_X], &ps4_state, LEFT_JOYSTICK);
	_parse_joystick(packet[RIGHT_JOYSTICK_Y], packet[RIGHT_JOYSTICK_X], &ps4_state, RIGHT_JOYSTICK);

	if((ps4_state.circle_button || ps4_state.triangle_button || ps4_state.x_button || ps4_state.square_button) && led_state == 0) {
		blink(1);	
	} else if(!(ps4_state.circle_button || ps4_state.triangle_button || ps4_state.x_button || ps4_state.square_button) && led_state == 1) {
		blink(0);
	}
		
	return ps4_state;
	//print_controller_state(&ps4_state);
};

