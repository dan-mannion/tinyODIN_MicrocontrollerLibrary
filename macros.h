/*
 * macros.h
 *
 *  Created on: 5 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_MACROS_H_
#define SRC_MACROS_H_
#include "xbasic_types.h";

void bitset(u32 *reg, char bit);
void bitreset(u32 *reg, char bit);

void bitset_u16(u16 *reg, char bit);
void bitreset_u16(u16 *reg, char bit);

#endif /* SRC_MACROS_H_ */
