/*
 * Unit.h
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#ifndef SRC_UNIT_H_
#define SRC_UNIT_H_

#include "XBEE.h"
#include "Typedefs.h"
#include "stm32l4xx_hal.h"

void unit_main(UART_HandleTypeDef *huart);

void unit_handler_loop();
Unit poll_for_request();
void process_request(Unit req);

int process_init(unsigned int unit_num);
int process_state_request(unsigned int unit_num);
int process_state_update();

State get_unit_state();
unsigned int get_unit_num();
void set_unit_state(State state);
void set_unit_num(unsigned int num);

#endif /* SRC_UNIT_H_ */
