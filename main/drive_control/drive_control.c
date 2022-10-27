#include "drive_control.h"

robot_driver_control_t init_drive_control() {

	tb6612_motor_t motorLeft = init_tb6612(GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_27, MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
	tb6612_motor_t motorRight = init_tb6612(GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_13, MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);

	return (robot_driver_control_t) {motorLeft, motorRight};
}

void _drive_break(robot_driver_control_t* robot_driver_control) {
	
	brake(&robot_driver_control->driveMotorLeft);	
	brake(&robot_driver_control->driveMotorRight);
}

void _drive_stop(robot_driver_control_t* robot_driver_control) {

	stop(&robot_driver_control->driveMotorLeft);
	stop(&robot_driver_control->driveMotorRight);
}

void control_ps4(controller_state* ps4_controller, robot_driver_control_t* driver_control) {

	if(ps4_controller->rButton) {
		_drive_break(driver_control);
	} else {
		_drive_stop(driver_control);	
	}

}

