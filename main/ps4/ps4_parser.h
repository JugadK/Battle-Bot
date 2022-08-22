#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {

	bool square_button;
	bool x_button;
	bool circle_button;
	bool triangle_button;
	bool left_d_pad;
	bool down_d_pad;
	bool right_d_pad;
	bool up_d_pad;

	uint8_t left_trigger_percent;
	uint8_t right_trigger_percent;

	int8_t left_joystick_x_axis;
	int8_t left_joystick_y_axis;
	int8_t right_joystick_x_axis;
	int8_t right_joystick_y_axis;

} controller_state;

void ps4_parse_packet(uint8_t *packet, uint16_t size);

void print_controller_state(controller_state *ps4_state);


