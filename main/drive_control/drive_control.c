#include "drive_control.h"
#include <math.h>

typedef enum {
	NORTHEAST,
	NORTHWEST,
	SOUTHEAST,
	SOUTHWEST,
} direction_t; 

robot_driver_control_t init_drive_control() {

  tb6612_motor_t motorRight =
      init_tb6612(GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_26, MCPWM_UNIT_0,
                  MCPWM_TIMER_0, MCPWM0A, MCPWM_OPR_A);
  tb6612_motor_t motorLeft =
      init_tb6612(GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, MCPWM_UNIT_0,
                  MCPWM_TIMER_2, MCPWM2A, MCPWM_OPR_A);

  return (robot_driver_control_t){motorLeft, motorRight};
}

void _drive_break(robot_driver_control_t *robot_driver_control) {

  brake(&robot_driver_control->driveMotorLeft);
  brake(&robot_driver_control->driveMotorRight);
}

void _drive_stop(robot_driver_control_t *robot_driver_control) {

  stop(&robot_driver_control->driveMotorLeft);
  stop(&robot_driver_control->driveMotorRight);
}

void _drive_robot(controller_state *ps4_controller,
                  robot_driver_control_t *driver_control) {

  double_t x = ps4_controller->left_joystick_x_axis;
  double_t y = ps4_controller->left_joystick_y_axis;
	double_t forward_speed = ps4_controller->right_trigger_percent;
	double_t backward_speed = ps4_controller->left_trigger_percent;

  // Deadlock zone
  if (fabs(x) < 2.0 || isnan(x)) {
    x = 0.0;
  }

  if (fabs(y) < 2.0 || isnan(x)) {
    y = 0.0;
  }

  double_t leftSpeed = forward_speed;
  double_t rightSpeed= forward_speed;

  if (forward_speed != 0 || backward_speed != 0) {
	
		double_t speed =  forward_speed - backward_speed;

		leftSpeed = speed;
		rightSpeed = speed;

		if(x <= -25) {
			rightSpeed = speed * ((x + 25)/ (25));
		} else if(x > -25 && x <= 0) {

			rightSpeed = speed * fabs((-25-x)/25);
		} else if (x > 0 && x <= 25) {
			
			leftSpeed = speed * ((25-x) / 25);
		} else {
			leftSpeed = speed * ((x-25) / (-25));
		}


		leftSpeed /= 2;
		rightSpeed /= 2;
  }
	
	
  set_speed(&driver_control->driveMotorLeft, leftSpeed);
  set_speed(&driver_control->driveMotorRight, rightSpeed);
}

void control_ps4(controller_state *ps4_controller,
                 robot_driver_control_t *driver_control) {

  // printf("%x\n", ps4_controller->rButton);

  if (ps4_controller->lButton) {
    _drive_break(driver_control);
  } else if(ps4_controller->circle_button) {
		
		set_speed(&driver_control->driveMotorLeft, 100);
		set_speed(&driver_control->driveMotorRight, 100);

	}else {
		
		_drive_robot(ps4_controller, driver_control);
  }
}
