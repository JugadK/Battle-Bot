#include "driver/gpio.h"
#include "driver/mcpwm.h"

#define AI2 32
#define AI1 23
#define BI1 22
#define BI2 33

typedef struct {
  gpio_num_t input1;
  gpio_num_t input2;
  gpio_num_t pwm;
  mcpwm_unit_t mcpwm_unit;
  mcpwm_timer_t mcpwm_timer;
  mcpwm_operator_t mcpwm_op;
} tb6612_motor_t;

// 1 2 PWM
// H H H/L Short Brake
// L H H   CCW
// L H L   Short Brake
// H L H   CW
// H L L   Short Brake
// L L H   Stop

tb6612_motor_t init_tb6612(gpio_num_t input1, gpio_num_t input2, gpio_num_t pwm,
                           mcpwm_unit_t mcpwm_unit, mcpwm_timer_t mcpwm_timer,
                           mcpwm_operator_t mcpwm_op) {

  // TODO Error checking for multiple of the same pins being used

  gpio_pad_select_gpio(input1);
  gpio_set_direction(input1, GPIO_MODE_OUTPUT);

  gpio_pad_select_gpio(input2);
  gpio_set_direction(input2, GPIO_MODE_OUTPUT);

  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pwm);

  mcpwm_config_t config;
  config.frequency = 10000;
  config.cmpr_a = 0.0;
  config.cmpr_b = 0.0;
  config.counter_mode = MCPWM_UP_COUNTER;
  config.duty_mode = MCPWM_DUTY_MODE_0;

  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &config);

  return (tb6612_motor_t){input1,     input2,      pwm,
                          mcpwm_unit, mcpwm_timer, mcpwm_op};
}

void set_speed(tb6612_motor_t *motor, float speed) {

  if (speed >= 0) {

    gpio_set_level(motor->input1, 1);
    gpio_set_level(motor->input2, 0);

    mcpwm_set_duty(motor->mcpwm_unit, motor->mcpwm_timer, motor->mcpwm_op,
                   speed);
  } else {

    gpio_set_level(motor->input1, 0);
    gpio_set_level(motor->input2, 1);

    mcpwm_set_duty(motor->mcpwm_unit, motor->mcpwm_timer, motor->mcpwm_op,
                   -speed);
  }
}

void brake(tb6612_motor_t *motor) {
  gpio_set_level(motor->input1, 1);
  gpio_set_level(motor->input2, 1);
}

void stop(tb6612_motor_t *motor) {
  gpio_set_level(motor->input1, 0);
  gpio_set_level(motor->input2, 0);
}
