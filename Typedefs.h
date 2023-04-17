/*
 * Typedefs.h
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#ifndef SRC_TYPEDEFS_H_
#define SRC_TYPEDEFS_H_

#include "stdio.h"

typedef enum State_t {
	Err,	// 0
	On,		// 1
	Off, 	// 2
	Req,	// 3
	Init,	// 4
	Ack,	// 5
	Update,	// 6
	Number_of_states
} State;

typedef struct Unit_t {
	uint8_t num;
	State state;
} Unit;

// returns 1 if lhs == rhs, 0 if lhs != rhs
int Unit_EQ(Unit lhs, Unit rhs);

#endif /* SRC_TYPEDEFS_H_ */
