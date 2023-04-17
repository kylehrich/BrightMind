/*
 * Keypad.c
 *
 *  Created on: Mar 30, 2023
 *      Author: User
 */

#include "Keypad.h"
#include "main.h"

/* Globals */

unsigned int COLS[NUM_COLS][2] =	{{Col_1_GPIO_Port, Col_1_Pin},
									{Col_2_GPIO_Port, Col_2_Pin},
									{Col_3_GPIO_Port, Col_3_Pin},
									{Col_4_GPIO_Port, Col_4_Pin}};

unsigned int ROWS[NUM_ROWS][2] =	{{Row_1_GPIO_Port, Row_1_Pin},
									{Row_2_GPIO_Port, Row_2_Pin},
									{Row_3_GPIO_Port, Row_3_Pin},
									{Row_4_GPIO_Port, Row_4_Pin}};

char CHARS[NUM_ROWS][NUM_COLS] =	{{'1', '2', '3', 'A'},
									{'4', '5', '6', 'B'},
									{'7', '8', '9', 'C'},
									{'*', '0', '#', 'D'}};

char COMBO[COMBO_LEN] =	{'A', 'B', 'C', 'D'};

int MAX_UNIT_DIGITS = 0;

char key_buf[KEY_BUF_SIZE];
unsigned int key_head = 0;
unsigned int key_tail = 0;
char combo_in[COMBO_LEN] = {0};
unsigned int combo_idx = 0;
unsigned int locked = 1;
int num_chars_in = -1;

/* Helpers */

void init_keypad() {
	/* TODO: Change interrupts to the ones for
	   	   the pins you hooked up the rows to */
	  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 5);
	  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 5);
	  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 5);

	  init_max_unit_digits();

	  reset_all_inputs();
	  set_all_inputs();
}

void init_max_unit_digits() {
	MAX_UNIT_DIGITS = ceil(log10(MAX_UNITS));
}

int get_num_chars_in() {
	return num_chars_in;
}

unsigned int get_row_pin(unsigned int row) {
	if (row >= NUM_ROWS) {
		return 0;
	}

	return ROWS[row][1];
}

unsigned int get_row_port(unsigned int row) {
	if (row >= NUM_ROWS) {
		return 0;
	}

	return ROWS[row][0];
}

unsigned int get_col_pin(unsigned int col) {
	if (col >= NUM_COLS) {
		return 0;
	}

	return COLS[col][1];
}

unsigned int get_col_port(unsigned int col) {
	if (col >= NUM_COLS) {
		return 0;
	}

	return COLS[col][0];
}

unsigned int is_locked() {
	return locked;
}

void set_locked() {
	locked = 1;
}

void set_unlocked() {
	locked = 0;
}

void set_all_inputs() {
	for (int i = 0; i < NUM_COLS; ++i) {
		HAL_GPIO_WritePin(COLS[i][0], COLS[i][1], GPIO_PIN_SET);
	}
}

void reset_all_inputs() {
	for (int i = 0; i < NUM_COLS; ++i) {
		HAL_GPIO_WritePin(COLS[i][0], COLS[i][1], GPIO_PIN_RESET);
	}
}

GPIO_PinState read_input(unsigned int row) {
	return HAL_GPIO_ReadPin(ROWS[row][0], ROWS[row][1]);
}

void set_output(unsigned int col) {
	HAL_GPIO_WritePin(COLS[col][0], COLS[col][1], GPIO_PIN_SET);
}

void reset_output(unsigned int col) {
	HAL_GPIO_WritePin(COLS[col][0], COLS[col][1], GPIO_PIN_SET);
}

// returns the column of the button pressed, otherwise returns -1
int find_col(unsigned int row) {
	for (int col = 0; col < NUM_COLS; ++col) {
		// power column
		set_output(col);
		// check if row is powered
		if (read_input(row) == GPIO_PIN_SET) {
			// if row is powered, return column
			return col;
		} else {
			// otherwise clear this column and try the next one
			reset_output(col);
		}
	}

	return -1;
}

unsigned int key_buf_size() {
	// tail > head || head == tail (normal calculation is fine)
	if (key_head <= key_tail) {
		return key_tail - key_head;
	}
	// tail < head (difference is unused space, so the remainder is the size)
	else {
		return KEY_BUF_SIZE - (key_head - key_tail);
	}
}

void key_buf_write(char key_in) {
	// data_buf is full
	if (key_buf_size() == KEY_BUF_SIZE - 1) {
		return;
	}

	// add data to tail
	key_buf[key_tail] = key_in;

	// increment tail
	key_tail = (key_tail + 1) % KEY_BUF_SIZE;
}

char key_buf_read() {
	// data_buf is empty --> return null character
	if (key_buf_size() == 0) {
		return 0;
	}

	// save data at head of buffer
	char key_out = key_buf[key_head];

	// increment head
	key_head = (key_head + 1) % KEY_BUF_SIZE;

	return key_out;
}

/**
 * @param	row is the row number (0-3) for the button pressed
 * @retval 	returns the character on the button pressed.
 * 		   	returns the null character when no button was pressed.
 * @modify	data_buf
 */
void read_button(uint16_t row) {
	// unpower all columns to start
	reset_all_inputs();

	// find column that was pressed
	unsigned int col = find_col(row);

	// update data_buf based on row and col
	key_buf_write(CHARS[row][col]);

	// repower all columns
	set_all_inputs();

	// clear pending interrupts for half a second for button debounce
	__HAL_GPIO_EXTI_CLEAR_IT(get_row_pin(row));
	HAL_Delay(500);
	__HAL_GPIO_EXTI_CLEAR_IT(get_row_pin(row));
}

void update_password_display() {
	// if this is the first character ever, increment since we start at -1
	// and skip clearing the wrong password message
	if (num_chars_in == 0) {
		num_chars_in++;
	}
	// if this is the first character in the password, clear the wrong password message first
	else if (num_chars_in == 1) {
		clear_password();
	}

	print_password_char(num_chars_in);
}

// update combo with most recent char in data_buf
void update_combo() {
	// left-shift combo_in and put key_in in rightmost slot
	for (int i = 0; i < COMBO_LEN - 1; ++i) {
		combo_in[i] = combo_in[i + 1];
	}
	combo_in[COMBO_LEN - 1] = key_buf_read();

	// increment num_chars_in
	num_chars_in++;

	// print char to screen
	update_password_display();
}

void check_combo() {
	// don't do anything if there is no new data
	if (key_buf_size() == 0) {
		if (!is_sleep()) {
			increment_sleep_counter();
		}
		return;
	}
	else {
		reset_sleep_counter();
		if (is_sleep()) {
			wake_up();
			// throw away this input
			key_buf_read();
			return;
		}
	}

	// read in the next character in data_buf into combo_in
	update_combo();

	// don't check the combo, we haven't read enough character
	if (num_chars_in < COMBO_LEN) {
		return;
	}

	// check that combo_in matches the password
	for (int i = 0; i < COMBO_LEN; ++i) {
		if (combo_in[i] != COMBO[i]) {
			/* wrong combo */

			// lock
			set_locked();
			// display wrong combo message
			wrong_password();
			// reset num_chars_in
			num_chars_in = 0;
			return;
		}
	}

	/* correct combo */

	// unlock
	set_unlocked();
	// display correct combo message
	correct_password();
	// fully reset num_chars_in
	num_chars_in = -1;
}

void wait_for_password() {
	while (1) {
		// check if correct password was input
		check_combo();

		// if combo is unlocked, display the main screen
		if (!is_locked()) {
			display_main_screen();
			return;
		}
	}
}

/////////////////////////////////////////////////////////////

// waits KEY_DELAY * KEY_TIMEOUT for a key to be pressed
// returns the pressed key's character on success, 0 on timeout
char read_key() {
	char key_in = 0;
	int counter = 0;

	while (1) {
		key_in = key_buf_read();

		// no data input
		if (key_in == 0) {
			// user was unresponsive
			if (++counter == KEY_TIMEOUT) {
				return 0;
			}

			// wait
			HAL_Delay(KEY_DELAY);
			continue;
		}
		// display key read in and return
		else {
			display_key_in(key_in);
			return key_in;
		}
	}
}

// returns unit number, or -1 failure
int read_unit_num() {
	// TODO: unit_num size is 1 more than MAX_UNIT_DIGITS to include sentinel char
	char unit_num[4] = {0};
	char key_in = 0;

	for (int i = 0; i <= MAX_UNIT_DIGITS; ++i) {
		key_in = read_key();

		// user waited too long to press a key
		if (key_in == 0) {
			return - 1;
		}

		// user finished typing unit number
		if (!isdigit(key_in)){
			break;
		}
		// user has typed too many digits
		else if (i == MAX_UNIT_DIGITS) {
			return -1;
		}

		// add key_in as digit in unit_num
		unit_num[i] = key_in;
	}

	// make sure unit_num is a valid unit number
	if (atoi(unit_num) >= MAX_UNITS) {
		return -1;
	}

	return atoi(unit_num);
}

unsigned int get_EXTI_line(uint16_t GPIO_Pin) {
	switch(GPIO_Pin) {
	case 1<<0:
		return EXTI_LINE_0;
	case 1<<1:
		return EXTI_LINE_1;
	case 1<<2:
		return EXTI_LINE_2;
	case 1<<3:
		return EXTI_LINE_3;
	case 1<<4:
		return EXTI_LINE_4;
	case 1<<5:
		return EXTI_LINE_5;
	case 1<<6:
		return EXTI_LINE_6;
	case 1<<7:
		return EXTI_LINE_7;
	case 1<<8:
		return EXTI_LINE_8;
	case 1<<9:
		return EXTI_LINE_9;
	case 1<<10:
		return EXTI_LINE_10;
	case 1<<11:
		return EXTI_LINE_11;
	case 1<<12:
		return EXTI_LINE_12;
	case 1<<13:
		return EXTI_LINE_13;
	case 1<<14:
		return EXTI_LINE_14;
	case 1<<15:
		return EXTI_LINE_15;
	}
}

// REQUIRES: Rows are all on different pins
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// check if this is one of the rows
	for (int row = 0; row < NUM_ROWS; ++row) {
		if (GPIO_Pin == get_row_pin(row)) {
			// read the button pressed into data_buf
			read_button(row);
			return;
		}
	}

	__NOP();
}
