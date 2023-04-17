/*
 * Keypad.h
 *
 *  Created on: Mar 30, 2023
 *      Author: User
 */

// TODO: Hook up rows to all different pin numbers
// TODO: Name all GPIO in the .ioc file Row_# and Col_# respectively

#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_

#include "Display.h"
#include "Typedefs.h"
#include "Hub.h"
#include "stdio.h"
#include "ctype.h"
#include "math.h"

/* Macros */

#define NUM_COLS	4
#define NUM_ROWS	4
#define COMBO_LEN	4

#define KEY_BUF_SIZE	64

#define KEY_DELAY		100 // ms
#define KEY_TIMEOUT		100 // in units of KEY_DELAY

/* Helpers */

void init_keypad();
void init_max_unit_digits();

int get_num_chars_in();
unsigned int get_row_pin(unsigned int row);
unsigned int get_row_port(unsigned int row);
unsigned int get_col_pin(unsigned int col);
unsigned int get_col_port(unsigned int col);
unsigned int is_locked();
void set_locked();
void set_unlocked();

void set_all_inputs();
void reset_all_inputs();
GPIO_PinState read_input(unsigned int row);
void set_output(unsigned int col);
void reset_output(unsigned int col);
int find_col(unsigned int row);

unsigned int key_buf_size();
void key_buf_write(char data_in);
char key_buf_read();

void read_button(uint16_t row);

void update_password_display();
void update_combo();
void check_combo();
void wait_for_password();

char read_key();
int read_unit_num();

unsigned int get_EXTI_line(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


#endif /* SRC_KEYPAD_H_ */
