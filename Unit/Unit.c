/*
 * Unit.c
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#include "Unit.h"

volatile Unit unit;

void unit_main(UART_HandleTypeDef *huart) {
	set_unit_num(get_err_unit().num);
	set_unit_state(Err);

	init_xbee(huart);

	unit_handler_loop();
}

void unit_handler_loop() {
	while(1) {
		process_request(poll_for_request());
	}
}

Unit poll_for_request() {
	Unit req_data = get_err_unit();
	while (Unit_EQ(req_data, get_err_unit())) {
		req_data = read_data(get_unit_num());
	}

	return req_data;
}

void process_request(Unit req) {
	// unit uninitialized
	if (get_unit_num() == 0) {
		if (req.state == Init) {
			process_init(req.num);
		}
		// this unit cannot handle this request yet
		else {
			return;
		}
	}
	// unit is initialized
	else if (req.num == get_unit_num()) {
		// unit's state was requested
		if (req.state == Req) {
			process_state_request(get_unit_num());
		}
		// unit's state was requested to be updated
		else if (req.state == Update) {
			process_state_update();
		}
		// this is not a valid request
		else {
			return;
		}
	}
	// this request was not for this unit
	else {
		return;
	}
}

// returns 1 on success, 0 on failure
int process_init(unsigned int unit_num) {
	// handle as if this were a regular state request
	if (!process_state_request(unit_num)) {
		return 0;
	}
	// update unit number with the number received
	set_unit_num(unit_num);
	return 1;
}

// returns 1 on success, 0 on failure
int process_state_request(unsigned int unit_num) {
	// send state
	if (!send_data(unit_num, get_unit_state())) {
		return 0;
	}
	// listen for acknowledgment
	if (!is_ack_received(unit_num)) {
		return 0;
	}

	return 1;
}

// returns 1 on success, 0 on failure
int process_state_update() {
	// acknowledge state update request
	if (!send_ack(get_unit_num())) {
		return 0;
	}
	// read data for update
	Unit data = read_data(get_unit_num());
	if (Unit_EQ(data, get_err_unit())) {
		// data was corrupted
		return 0;
	}
	else {
		// update state
		set_unit_state(data.state);
	}
	// send acknowledgment
	if (!send_ack(get_unit_num())) {
		return 0;
	}

	return 1;
}

State get_unit_state() {
	return unit.state;
}

unsigned int get_unit_num() {
	return unit.num;
}

void set_unit_state(State state) {
	unit.state = state;
}

void set_unit_num(unsigned int num) {
	unit.num = num;
}
