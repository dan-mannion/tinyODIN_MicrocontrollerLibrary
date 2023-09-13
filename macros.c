/*
 * macros.c
 *
 *  Created on: 5 Sep 2023
 *      Author: danjo
 */
#include "macros.h"


void bitset(u32 *reg, char bit){
	*reg = (*reg)|(1<<bit);
}
void bitreset(u32 *reg, char bit){
	*reg = (*reg)&~(1<<bit);
}

void bitset_u16(u16 *reg, char bit){
	*reg = (*reg)|(1<<bit);
}
void bitreset_u16(u16 *reg, char bit){
	*reg = (*reg)&~(1<<bit);
}
