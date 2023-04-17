/*
 * Hub.c
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#include "Hub.h"

void hub_main(SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart) {
	  init_keypad();
	  init_xbee(huart);
	  init_screen(hspi);

	  hub_control_loop();
}

void hub_control_loop() {
	// start by waiting for the password
#ifndef	__DEBUG__
	wait_for_password();
#endif

	while (1) {
		hub_control();
		check_next_unit();
	}
}

void hub_control() {
	char key_in = key_buf_read();

	// no button was pressed
	if (key_in == 0) {
		if (!is_sleep()) {
			increment_sleep_counter();
		}
		return;
	}
	else {
		reset_sleep_counter();
		if (is_sleep()) {
			wake_up();
			// skip this input
			return;
		}
	}

	display_key_in(key_in);

	// page up
	if (key_in == '2' || key_in == '6') {
		clear_keys_in();
		page_up();
	}
	// page down
	else if (key_in == '8' || key_in == '4') {
		clear_keys_in();
		page_down();
	}
	// add new unit
	else if (key_in == 'A') {
		init_new_unit();
		clear_keys_in();
	}
	// update unit
	else if (key_in == 'B') {
		Unit unit = get_unit_for_update();
		if (!Unit_EQ(unit,get_err_unit())) {
			update_unit_state(unit.num, unit.state);
		}
		clear_keys_in();
	}
	// display unit number
	else if (key_in == '#') {
		display_unit(read_unit_num());
		clear_keys_in();
	}
	// remove unit
	else if (key_in == '*') {
		remove_unit(read_unit_num());
		clear_keys_in();
	}
	// quit and lock
	else if (key_in == 'D') {
		clear_keys_in();
		set_locked();
		display_lock_screen();
		wait_for_password();
	}
	// clear invalid inputs
	else {
		clear_keys_in();
	}
}

void check_next_unit() {
	static unsigned int unit_idx = 0;
	Unit *units = get_units();
	Unit temp_unit;

	// make sure unit_idx is valid
	if (unit_idx >= get_tail_idx()) {
		unit_idx = 0;
		return;
	}

	request_unit_state(units[unit_idx].num);

	// increment unit_idx
	unit_idx++;
	if (unit_idx >= get_tail_idx()) {
		unit_idx = 0;
	}
}

State key_to_state(char key) {
	switch (key) {
	case '0':
		return Off;
	case '1':
		return On;
	default:
		return Err;
	}
}

Unit get_unit_for_update() {
	char key_in = 0;
	State new_state = Err;
	Unit unit;
	// get unit number
	unit.num = read_unit_num();

	// get new state
	key_in = read_key();
	if (!isdigit(key_in)) {
		// invalid input
		return get_err_unit();
	}
	new_state = key_to_state(key_in);
	// get ending non-digit
	key_in = read_key();
	if (isdigit(key_in)) {
		// invalid ending non-digit
		return get_err_unit();
	}

	// check that the new state is valid
	if (new_state == Err) {
		return get_err_unit();
	}
	else {
		// set unit for update's state to the user input
		unit.state = new_state;
	}

	return unit;
}

void init_new_unit() {
	unsigned int unit_num = get_unused_unit();
	// request data
	Unit req_data = request_data(unit_num, Init);

	// init failed
	if (Unit_EQ(req_data, get_err_unit())) {
		add_unused_unit(unit_num);
	}
	// init succeeded --> update display
	else {
		update_unit(&req_data);
	}
}

void request_unit_state(unsigned int unit_num) {
	// request data
	Unit req_data = request_data(unit_num, Req);

	// if data request failed, update unit's state to Err
	if (Unit_EQ(req_data, get_err_unit())) {
		req_data.num = unit_num;
		req_data.state = Err;
	}
	// update display
	update_unit(&req_data);
}

void update_unit_state(unsigned int unit_num, State new_state) {
	// ask unit to update its state
	if(!send_data(unit_num, Update)) {
		return;
	}
	// listen for unit's acknowledgment
	if (!is_ack_received(unit_num)) {
		return;
	}
	// tell unit what to state to update to
	if (!send_data(unit_num, new_state)) {
		return;
	}
	// listen for unit's acknowledgment
	if (!is_ack_received(unit_num)) {
		return;
	}
}
