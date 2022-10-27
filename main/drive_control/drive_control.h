#include "../tb6612fng/tb6612fng.h"
#include "../ps4/ps4_parser.h"

typedef struct {
	tb6612_motor_t driveMotorLeft;
	tb6612_motor_t driveMotorRight;
} robot_driver_control_t;

robot_driver_control_t init_drive_control();

void control_ps4(controller_state* ps4_controller, robot_driver_control_t* driver_control);
