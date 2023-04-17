/*
 * Typedefs.c
 *
 *  Created on: Apr 9, 2023
 *      Author: User
 */

#include "Typedefs.h"

// returns 1 if lhs == rhs, 0 if lhs != rhs
int Unit_EQ(Unit lhs, Unit rhs) {
	if (lhs.num == rhs.num && lhs.state == rhs.state) {
		return 1;
	}
	else {
		return 0;
	}
}
