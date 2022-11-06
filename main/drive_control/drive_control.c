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
      init_tb6612(GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_27, MCPWM_UNIT_0,
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

  // Deadlock zone
  if (fabs(x) < 2.0) {
    x = 0.0;
  }

  if (fabs(y) < 2.0) {
    y = 0.0;
  }

  // we use pythag to figure out the length of the vector
  double_t speed = sqrt((x * x) + (y * y));


  double_t leftDec = 1;
  double_t rightDec = 1;

  if (speed != 0) {
    double_t dot_prod = 0;


		// If we are in the upper half of the joystick 
    if (((x >= 0 && y >= 0) || (x < 0 && y >= 0))) {
      dot_prod = (x * 0 + (y)*1);
    } else {

      dot_prod = (x * 0 + (y) * -1);
    }

		direction_t direction = NORTHEAST;

    // arccos(a dot b/ |a| |b|) = angle
		if ((x >= 0 && y >= 0)) {
			direction = NORTHEAST;
    } else if (x < 0 && y >= 0) {
			direction = NORTHWEST;
    } else if (x < 0 && y < 0) {
			direction = SOUTHWEST;
    } else {
			direction = SOUTHEAST;
		}


		switch(direction) {
			case NORTHEAST :
				speed = speed * -1;
				break;
			case NORTHWEST :
				break;
			case SOUTHWEST :
				speed = speed * -1;
				break;
			case SOUTHEAST :
				break;
		}

		/*if ((x >= 0 && y >= 0)) {

      rightDec = (90 - angle) / 90.0;
    } else if (x < 0 && y >= 0) {
      leftDec = (90 - angle) / 90.0;
    } else if (x < 0 && y < 0) {
      leftDec = ((90 - angle) / 90.0);
			speed = speed * -1;
    } else {
      rightDec = (-1) * ((90 - angle) / 90.0);
    	speed = speed * -1;
		}*/
	
		double_t angle = acos((dot_prod / speed)) * 180 / 3.14;


  }
 	// printf("leftDec : %lf    rightDec : %lf\n", speed * leftDec,
  //       speed * rightDec);
  //	printf("x : %lf   y : %lf   rightDec \n", x, y);

  set_speed(&driver_control->driveMotorLeft, speed * leftDec);
//  set_speed(&driver_control->driveMotorRight, speed * rightDec);
}

void control_ps4(controller_state *ps4_controller,
                 robot_driver_control_t *driver_control) {

   printf("%x\n", ps4_controller->rButton);

  if (ps4_controller->rButton) {
    _drive_break(driver_control);
  } else if(ps4_controller->circle_button) {
		
		set_speed(&driver_control->driveMotorLeft, 100);
		set_speed(&driver_control->driveMotorRight, 100);

	}else {
		
		_drive_robot(ps4_controller, driver_control);
  }
}
