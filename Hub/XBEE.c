/*
 * XBEE.c
 *
 *  Created on: Mar 31, 2023
 *      Author: User
 */

#include "XBEE.h"

uint8_t xbee_read_data[XBEE_BUF_SIZE];
uint8_t xbee_write_data[XBEE_BUF_SIZE];
UART_HandleTypeDef *huart_ptr;
Unit ERR_UNIT;

// set the internal huart pointer and setup err_unit global
void init_xbee(UART_HandleTypeDef *huart) {
	huart_ptr = huart;
	init_err_unit();
}

void init_err_unit() {
	ERR_UNIT.num = 0;
	ERR_UNIT.state = Number_of_states;
}

Unit get_err_unit() {
	return ERR_UNIT;
}

// boolean error check for unit number
int is_valid_num(unsigned int num) {
	if (num < Number_of_states) {
		return 0;
	}
	else {
		return 1;
	}
}

// boolean error check for state
int is_valid_state(State state) {
	if (state < Number_of_states) {
		return 1;
	}
	else {
		return 0;
	}
}

// boolean error check for Unit data
int is_valid_data(unsigned int unit_num, State state, unsigned int expected_unit_num, State expected_state) {
	// invalid data
	if (!is_valid_num(unit_num) || !is_valid_state(state)) {
		return 0;
	}
	// there exists an expected unit number, and data's unit number does not match
	if (expected_unit_num != ERR_UNIT.num && unit_num != expected_unit_num) {
		return 0;
	}
	// there exists a single expected state, and data's state does not match
	if (expected_state != ERR_UNIT.state && state != expected_state) {
		return 0;
	}

	// valid data
	return 1;
}

// returns 1 on success, 0 on failure
int send_data(unsigned int unit_num, State state) {
	HAL_StatusTypeDef ret_val = HAL_ERROR;
	int attempts = 0;

	// setup data to send
	xbee_write_data[0] = unit_num;
	xbee_write_data[1] = state;

	// keep trying to send data until it goes through or we try too many times
	while (ret_val != HAL_OK) {
		// check if we've attempted to send data too many times
		if (attempts >= MAX_DATA_ATTEMPTS) {
			return 0;
		}
		// try sending data
		ret_val = HAL_UART_Transmit(huart_ptr, xbee_write_data, 2, XBEE_TIMEOUT_MS);
		// increment number of attempts
		attempts++;
	}

	// data was successfully sent
	return 1;
}

// returns data received on success, ERR_UNIT on failure
Unit receive_data(unsigned int unit_num, State expected_state) {
	HAL_StatusTypeDef ret_val = HAL_ERROR;
	int attempts = 0;

	// keep trying to receive data until we get valid data or we try too many times
	while (ret_val != HAL_OK // data wasn't received,...
		|| !is_valid_data(xbee_read_data[0], xbee_read_data[1], unit_num, expected_state) // ...or data is invalid
	) {
		// check if we've attempted to receive data too many times
		if (attempts >= MAX_DATA_ATTEMPTS) {
			// return data with erroneous values
			return ERR_UNIT;
		}
		// try receiving data
		ret_val = HAL_UART_Receive(huart_ptr, xbee_read_data, 2, XBEE_TIMEOUT_MS);
		// increment number of attempts
		attempts++;
	}

	// return received data
	Unit data;
	data.num = xbee_read_data[0];
	data.state = xbee_read_data[1];
	return data;
}

Unit read_data(unsigned int unit_num) {
	return receive_data(unit_num, ERR_UNIT.state);
}

// returns 1 on success, 0 on failure
int send_ack(unsigned int unit_num) {
	return send_data(unit_num, Ack);
}

// returns 1 if ack was received, 0 if it was not
int is_ack_received(unsigned int unit_num) {
	if (Unit_EQ(receive_data(unit_num, Ack), ERR_UNIT)) {
		return 0;
	}
	else {
		return 1;
	}
}

Unit request_data(unsigned int unit_num, State req) {
	// send request
	if (!send_data(unit_num, req)) {
		// request did not go through --> return err
		return ERR_UNIT;
	}

	// receive response
	Unit data = receive_data(unit_num, ERR_UNIT.state);
	if (Unit_EQ(data, ERR_UNIT)) {
		// data was not received --> return err
		return ERR_UNIT;
	}

	// send acknowledgment that the data was received
	if (!send_ack(unit_num)) {
		// ack did not go through --> return err
		return ERR_UNIT;
	}

	// transaction complete
	return data;
}
