/*
 * odin_typedefs.h
 *
 *  Created on: 13 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_ODIN_TYPEDEFS_H_
#define SRC_ODIN_TYPEDEFS_H_
#include "xbasic_types.h"



typedef struct{
	u16 word_address;
	u8 byte_address;
	u8 upper_lower_nibble; // 0 indicates least significant nibble, 1 indicates most significant nibble.
} SynapseAddress;

typedef struct{
	SynapseAddress memory_address;
	u8 preneuron;
	u8 postneuron;
	u8 value;
}Synapse;

typedef struct{
	u16 index;
	u16 membrane_potential;
	u16 threshold;
	u8 leakage_value;
	u8 disabled;
}Neuron;

#endif /* SRC_ODIN_TYPEDEFS_H_ */
