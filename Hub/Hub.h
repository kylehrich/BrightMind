/*
 * Hub.h
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#ifndef SRC_HUB_H_
#define SRC_HUB_H_

#include "XBEE.h"
#include "Display.h"
#include "Typedefs.h"
#include "stm32l4xx_hal.h"

//#define __DEBUG__

#define __SLEEP_MODE__
#define	SLEEP_TIMEOUT	750000 // ~37.5s unlocked, ~25s locked; 20000 ~=~ 1s unlocked, 30000 ~=~ 1s locked

void hub_main(SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart);

void hub_control_loop();
void hub_control();
void check_next_unit();

State key_to_state(char key);
Unit get_unit_for_update();

void init_new_unit();
void request_unit_state(unsigned int unit_num);
void update_unit_state(unsigned int unit_num, State new_state);

#endif /* SRC_HUB_H_ */
