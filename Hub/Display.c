/*
 * Display.c
 *
 *  Created on: Mar 30, 2023
 *      Author: User
 */

#include "Display.h"

/* Globals */

unsigned int MAX_PAGES = 0;

Unit units[MAX_UNITS_PLUS_ONE];
unsigned int unused_units[MAX_UNITS];
unsigned int tail_idx = 0;
unsigned int curr_page = 0;

int num_keys_in = 0;

unsigned int sleep_counter = 0;
unsigned int is_asleep = 0;

int max_line = 0; // deprecated (used for clear_page())

/* Helpers */

void init_screen(SPI_HandleTypeDef *hspi) {
	// init the MAX_PAGES global (must happen first)
	init_MAX_PAGES();

	// init hspi (must happen before lcd_init())
	init_hspi(hspi);

	// initialize the display
	lcd_init();

	init_unused_units();

	// turn on back light
	wake_up();
	reset_sleep_counter();

	// display screen
#ifdef __DEBUG__
	display_main_screen();
#else
	display_lock_screen();
#endif
}

void init_MAX_PAGES() {
	MAX_PAGES = ceil((double)(MAX_UNITS) / (double)(PAGE_SIZE));
}

unsigned int get_MAX_PAGES() {
	return (MAX_PAGES);
}

Unit *get_units() {
	return units;
}

unsigned int get_tail_idx() {
	return tail_idx;
}

unsigned int get_curr_page() {
	return curr_page;
}

// increments curr_page
void page_up() {
	if (curr_page < MAX_PAGES - 1) {
		// update display
		print_page(curr_page + 1);
	}
}

// decrements curr_page
void page_down() {
	if (curr_page > 0) {
		// update display
		print_page(curr_page - 1);
	}
}

// returns the page idx is on
unsigned int get_page_from_idx(const unsigned int idx) {
	return idx / (PAGE_SIZE);
}

// returns the line idx will be printed to
unsigned int get_line_from_idx(const unsigned int idx) {
	return idx % (PAGE_SIZE);
}

unsigned int get_idx_from_page_and_line(const unsigned int page, const unsigned int line) {
	return page * (PAGE_SIZE) + line;
}

// returns unit num's index in units array, else returns MAX_UNITS
unsigned int find_unit(const unsigned int num) {
	for (unsigned int i = 0; i < tail_idx; ++i) {
		if (units[i].num == num) {
			return i;
		}
	}

	return MAX_UNITS;
}

void display_lock_screen() {
	// fill in the background
	fill_rectangle(0, 0, WIDTH, HEIGHT, LOCK_BG_COLOR);
	draw_string((WELCOME_X), (WELCOME_Y), LOCK_TEXT_COLOR, 2, "Welcome!");
	draw_string((INSERT_PASSWORD_X), (INSERT_PASSWORD_Y), LOCK_TEXT_COLOR, 1, "Insert password");
}

void clear_password() {
	fill_rectangle((PASSWORD_X), (PASSWORD_Y), (PASSWORD_X) + CHAR_WIDTH * 8 * 2, (PASSWORD_Y) + CHAR_HEIGHT * 2, LOCK_BG_COLOR);
}

void print_password_char(unsigned int char_num) {
	// char_num is 1 indexed
	draw_char((PASSWORD_X) + (CHAR_WIDTH * (char_num - 1) * 2 * 2), (PASSWORD_Y), '_', LOCK_TEXT_COLOR, 2);
}

void wrong_password() {
	clear_password();
	draw_fast_string((PASSWORD_X) + (CHAR_WIDTH * (14-5)/2), (PASSWORD_Y) + CHAR_HEIGHT, LOCK_TEXT_COLOR, LOCK_BG_COLOR, "WRONG");
}

void correct_password() {
	clear_password();
	draw_fast_string((PASSWORD_X) + (CHAR_WIDTH * (14-5)/2), (PASSWORD_Y) + CHAR_HEIGHT, LOCK_TEXT_COLOR, LOCK_BG_COLOR, "CORRECT");
}

void display_main_screen() {
	// fill in the background
	fill_rectangle(0, 0, WIDTH, HEIGHT, MAIN_BG_COLOR);
	// draw titles
	draw_fast_string(1, 1, MAIN_TEXT_COLOR, MAIN_BG_COLOR, "UNIT #  STATE  PAGE:");
	draw_fast_string(MAIN_X2_BORDER + 2, MAIN_Y1_BORDER + 2, MAIN_TEXT_COLOR, MAIN_BG_COLOR, "Keys in:");
	display_curr_page_num();
	// draw borders
	fill_rectangle(0, MAIN_Y1_BORDER, WIDTH, MAIN_Y1_BORDER, MAIN_TEXT_COLOR); // horizontal
	fill_rectangle(MAIN_X1_BORDER, 0, MAIN_X1_BORDER, HEIGHT, MAIN_TEXT_COLOR); // vertical 1
	fill_rectangle(MAIN_X2_BORDER, 0, MAIN_X2_BORDER, HEIGHT, MAIN_TEXT_COLOR); // vertical 2

	// main screen starts at page 0
	max_line = -1;
	force_print_page(0);
}

void display_curr_page_num() {
	char page_str[10];
	// convert page number to string
	sprintf(page_str, "%d", curr_page + 1);
	// clear previous page number
	fill_rectangle(PAGE_NUM_X, PAGE_NUM_Y, PAGE_NUM_X + (CHAR_WIDTH * 3), PAGE_NUM_Y + CHAR_HEIGHT - 1, MAIN_BG_COLOR);
	// print new page number
	draw_fast_string(PAGE_NUM_X, PAGE_NUM_Y, MAIN_TEXT_COLOR, MAIN_BG_COLOR, page_str);
}

void clear_line(unsigned int line) {
	unsigned int line_y = (MAIN_Y1_BORDER + 1) + (line * CHAR_HEIGHT);
	// clear state
	fill_rectangle(MAIN_X1_BORDER + 1, line_y, MAIN_X2_BORDER - 1, line_y + CHAR_HEIGHT - 1, MAIN_BG_COLOR);
	// clear unit number
	fill_rectangle(0, line_y, MAIN_X1_BORDER - 1, line_y + CHAR_HEIGHT - 1, MAIN_BG_COLOR);

	// decrement max_line
	max_line--;
}

void clear_page() {
	for (int line = max_line; line >= 0; --line) {
		clear_line(line);
	}
}

void sprint_num(char *str, const Unit *unit) {
	sprintf(str, "%d", unit->num);
}

void sprint_state(char *str, const Unit *unit) {
	switch (unit->state) {
	case Err:
		sprintf(str, "Err");
		break;
	case On:
		sprintf(str, "On");
		break;
	case Off:
		sprintf(str, "Off");
		break;
	default:
		sprintf(str, "Err");
		break;
	}
}

void print_unit(const Unit *unit, unsigned int line) {
	// ensure this unit

	char str[64];

	// get y value to write to based on line number
	unsigned int line_y = (MAIN_Y1_BORDER + 1) + line * CHAR_HEIGHT;

	// print unit number
	sprint_num(str, unit);
	draw_fast_string(1, line_y, MAIN_TEXT_COLOR, MAIN_BG_COLOR, str);

	// print state
	sprint_state(str, unit);
	draw_fast_string(MAIN_X1_BORDER + 1, line_y, MAIN_TEXT_COLOR, MAIN_BG_COLOR, str);
}

unsigned int max_uint(unsigned int lhs, unsigned int rhs) {
	if (lhs > rhs) {
		return lhs;
	}
	else {
		return rhs;
	}
}

void print_line(unsigned int line) {
	clear_line(line);

	// check if this line has a valid unit
	if (get_idx_from_page_and_line(curr_page, line) >= tail_idx) {
		return;
	}

	print_unit(units + (curr_page * (PAGE_SIZE)) + line, line);

	max_line = max_uint(line, max_line);
}

// print page without checking if the page changed
void force_print_page(unsigned int page) {
	// check if this page has any data
	if (tail_idx <= page * (PAGE_SIZE)) {
		return;
	}

	// update curr_page
	curr_page = page;
	display_curr_page_num();

	// print all units on this page
	for (int line = 0; line < (PAGE_SIZE); ++line) {
		print_line(line);
	}
}

void print_page(unsigned int page) {
	// check if the page changed
	if (page == curr_page) {
		return;
	}
	// check if this page has any data
	if (tail_idx <= page * (PAGE_SIZE)) {
		return;
	}

	// update curr_page
	curr_page = page;
	display_curr_page_num();

	// print all units on this page
	for (int line = 0; line < (PAGE_SIZE); ++line) {
		print_line(line);
	}
}

void insert_unit_ptr(Unit *unit) {
	for (int i = 0; i < MAX_UNITS; ++i) {
		if (i == tail_idx) {
			// insert unit
			units[i] = *unit;
			// increment tail index
			tail_idx++;
			// print unit if it is added to current page
			if (get_page_from_idx(i) == curr_page) {
				print_line(get_line_from_idx(i));
			}
			break;
		}
	}
}

// returns 0 on success, 1 on error
unsigned int insert_unit(uint8_t num, State state) {
	// check for duplicate
	if (find_unit(num) < MAX_UNITS) {
		return 1;
	}

	Unit temp;
	temp.num = num;
	temp.state = state;
	insert_unit_ptr(&temp);

	return 0;
}

void swap_units(unsigned int idx1, unsigned int idx2) {
	Unit temp;
	temp = units[idx1];
	units[idx1] = units[idx2];
	units[idx2] = temp;
}

void remove_unit(unsigned int unit_num) {
	unsigned int idx = find_unit(unit_num);
	// check that idx is valid
	if (idx >= tail_idx) {
		return;
	}

	// update tail_idx
	tail_idx--;
	// update unused units
	add_unused_unit(units[idx].num);
	// shift all later units over to fill the gap
	for (int i = idx; i < MAX_UNITS; ++i) {
		swap_units(i, i + 1);
	}

	// update display
	if (get_page_from_idx(idx) == curr_page) {
		for (int i = get_line_from_idx(idx); i < (PAGE_SIZE); ++i) {
			print_line(i);
		}
	}
}

// returns 0 on success, 1 on error
unsigned int update_num(unsigned int old_num, unsigned int new_num) {
	// check for duplicate of new_num
	if (find_unit(new_num) < MAX_UNITS) {
		return 1;
	}

	// get index for old_num
	unsigned int idx = find_unit(old_num);

	// old_num does not exist
	if (idx == MAX_UNITS) {
		return 1;
	}

	// update
	units[idx].num = new_num;
	if (get_page_from_idx(idx) == curr_page) {
		print_line(get_line_from_idx(idx));
	}

	return 0;
}

// returns 0 on success, 1 on error
unsigned int update_state(unsigned int unit_num, State state) {
	// find this unit
	unsigned int idx = find_unit(unit_num);

	// this unit does not exist
	if (idx == MAX_UNITS) {
		return 1;
	}

	// update
	units[idx].state = state;
	if (get_page_from_idx(idx) == curr_page) {
		print_line(get_line_from_idx(idx));
	}

	return 0;
}

void update_unit(Unit *unit) {
	// find this unit
	unsigned int idx = find_unit(unit->num);

	// insert new unit
	if (idx == MAX_UNITS) {
		insert_unit_ptr(unit);
		idx = tail_idx - 1;
	}
	// update pre-existing unit
	else {
		// check if data changed
		if (units[idx].state == unit->state) {
			// data was unchanged so don't update anything
			return;
		}
		else {
			// update data
			units[idx].state = unit->state;
		}
	}

	// update display if this unit is on screen
	if (get_page_from_idx(idx) == curr_page) {
		print_line(get_line_from_idx(idx));
	}
}

void display_unit(unsigned int num) {
	unsigned int idx = find_unit(num);
	if (idx != MAX_UNITS) {
		print_page(get_page_from_idx(idx));
	}
}

void init_unused_units() {
	for (int i = 0; i < MAX_UNITS; ++i) {
		unused_units[i] = Number_of_states + i;
	}
}

unsigned int get_unused_unit() {
	// get next unused unit
	unsigned int unused_unit = unused_units[0];

	// move everything up to top of stack
	for (int i = 0; i < MAX_UNITS - 1; ++i) {
		unused_units[i] = unused_units[i + 1];
	}

	return unused_unit;
}

void add_unused_unit(unsigned int unit_num) {
	unused_units[(MAX_UNITS - 1) - tail_idx] = unit_num;
}

void display_key_in(char key_in) {
	if (num_keys_in == 10) {
		clear_keys_in();
	}
	draw_fast_char(MAIN_X2_BORDER + 2 + (num_keys_in * (CHAR_WIDTH + 1)), MAIN_Y1_BORDER + 2 + (CHAR_HEIGHT + 1), key_in, MAIN_TEXT_COLOR, MAIN_BG_COLOR);
	num_keys_in++;
}

void clear_keys_in() {
	fill_rectangle(MAIN_X2_BORDER + 2, MAIN_Y1_BORDER + 2 + (CHAR_HEIGHT + 1), WIDTH, MAIN_Y1_BORDER + 2 + ((CHAR_HEIGHT + 1) * 2), MAIN_BG_COLOR);
	num_keys_in = 0;
}

void enter_sleep() {
#ifdef __SLEEP_MODE__
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	is_asleep = 1;
#endif
}

void wake_up() {
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	is_asleep = 0;
}

void increment_sleep_counter() {
	sleep_counter++;
	if (sleep_counter >= SLEEP_TIMEOUT) {
		reset_sleep_counter();
		enter_sleep();
	}
}

void reset_sleep_counter() {
	sleep_counter = 0;
}

unsigned int is_sleep() {
	return is_asleep;
}
