/*
 * Display.h
 *
 *  Created on: Mar 30, 2023
 *      Author: User
 */

// include guard
#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

// include ILI9225 Github
#include "ILI9225.h"
#include "Keypad.h"
#include "Typedefs.h"
#include "Hub.h"
#include "math.h"
#include "stdio.h"

/* Macros */

#define MAX_UNITS 			(256 - Number_of_states)
#define MAX_UNITS_PLUS_ONE	(MAX_UNITS + 1)

#define LOCK_BG_COLOR	COLOR_NAVY
#define LOCK_TEXT_COLOR	COLOR_YELLOW

#define MAIN_BG_COLOR	COLOR_LIGHTGRAY
#define MAIN_TEXT_COLOR	COLOR_BLACK

#define MAIN_X1_BORDER	(CHAR_WIDTH * 8)
#define MAIN_X2_BORDER	(CHAR_WIDTH * 16)
#define MAIN_Y1_BORDER	(CHAR_HEIGHT + 1)

#define PAGE_NUM_X		(CHAR_WIDTH * 23 + 1)
#define	PAGE_NUM_Y		1
#define WELCOME_X			((WIDTH - (CHAR_WIDTH * 8 * 2)) / 2)
#define WELCOME_Y			((HEIGHT - (CHAR_HEIGHT * 2)) / 3)
#define INSERT_PASSWORD_X	((WIDTH - (CHAR_WIDTH * 15)) / 2)
#define INSERT_PASSWORD_Y	((WELCOME_Y) + (CHAR_HEIGHT * 2) + CHAR_HEIGHT)
#define PASSWORD_X			((WIDTH - (CHAR_WIDTH * 7 * 2)) / 2)
#define PASSWORD_Y			((INSERT_PASSWORD_Y) + CHAR_HEIGHT)

#define PAGE_SIZE	((HEIGHT - (MAIN_Y1_BORDER + 1)) / CHAR_HEIGHT)

/* Helpers */

void init_screen(SPI_HandleTypeDef *hspi);
void init_MAX_PAGES();

unsigned int get_MAX_PAGES();
Unit *get_units();
unsigned int get_tail_idx();
unsigned int get_curr_page();

void page_up();
void page_down();

unsigned int get_page_from_idx(const unsigned int idx);
unsigned int get_line_from_idx(const unsigned int idx);
unsigned int get_idx_from_page_and_line(const unsigned int page, const unsigned int line);

unsigned int find_unit(const unsigned int num);

void display_lock_screen();
void clear_password();
void print_password_char(unsigned int char_num);
void wrong_password();
void correct_password();

void display_main_screen();
void display_curr_page_num();
void clear_line(unsigned int line);
void clear_page();

void sprint_num(char *str, const Unit *unit);
void sprint_state(char *str, const Unit *unit);

void print_unit(const Unit *unit, unsigned int line);
unsigned int max_uint(unsigned int lhs, unsigned int rhs);
void print_line(unsigned int line);
void force_print_page(unsigned int page);
void print_page(unsigned int page);

void insert_unit_ptr(Unit *unit);
unsigned int insert_unit(uint8_t num, State state);
void swap_units(unsigned int idx1, unsigned int idx2);
void remove_unit(unsigned int unit_num);

unsigned int update_num(unsigned int old_num, unsigned int new_num);
unsigned int update_state(unsigned int unit_num, State state);
void update_unit(Unit *unit);
void display_unit(unsigned int num);

void init_unused_units();
unsigned int get_unused_unit();
void add_unused_unit(unsigned int unit_num);

void display_key_in(char key_in);
void clear_keys_in();

void enter_sleep();
void wake_up();
void increment_sleep_counter();
void reset_sleep_counter();
unsigned int is_sleep();

#endif /* SRC_DISPLAY_H_ */
