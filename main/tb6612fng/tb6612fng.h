#include "driver/gpio.h"
#include "driver/mcpwm.h"

typedef struct {
  gpio_num_t input1;
  gpio_num_t input2;
  gpio_num_t pwm;
  mcpwm_unit_t mcpwm_unit;
  mcpwm_timer_t mcpwm_timer;
	mcpwm_io_signals_t mcpwm_io_sig;
  mcpwm_operator_t mcpwm_op;
} tb6612_motor_t;

tb6612_motor_t init_tb6612(gpio_num_t input1, gpio_num_t input2, gpio_num_t pwm,
                           mcpwm_unit_t mcpwm_unit, mcpwm_timer_t mcpwm_timer,
                           mcpwm_io_signals_t mcpwm_io_sig, mcpwm_operator_t mcpwm_op);

void set_speed(tb6612_motor_t *motor, float speed);

void brake(tb6612_motor_t *motor);
void stop(tb6612_motor_t *motor);
