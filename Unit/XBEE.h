/*
 * XBEE.h
 *
 *  Created on: Mar 31, 2023
 *      Author: User
 */

#ifndef SRC_XBEE_H_
#define SRC_XBEE_H_

#include "Typedefs.h"
#include "stm32l4xx_hal.h"

#define XBEE_BUF_SIZE		2
#define	XBEE_TIMEOUT_MS		1000
#define WAIT_TIME_MS		2000
#define MAX_DATA_ATTEMPTS	(WAIT_TIME_MS / XBEE_TIMEOUT_MS)

void init_xbee(UART_HandleTypeDef *huart);
void init_err_unit();

Unit get_err_unit();
int is_valid_num(unsigned int num);
int is_valid_state(State state);
int is_valid_data(unsigned int unit_num, State state, unsigned int expected_unit_num, State expected_state);

int send_data(unsigned int unit_num, State state);
Unit receive_data(unsigned int unit_num, State expected_state);
Unit read_data(unsigned int unit_num);

int send_ack(unsigned int unit_num);
int is_ack_received(unsigned int unit_num);

Unit request_data(unsigned int unit_num, State req);

#endif /* SRC_XBEE_H_ */
