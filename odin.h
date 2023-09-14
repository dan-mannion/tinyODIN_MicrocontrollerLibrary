/*
 * odin.h
 *
 *  Created on: 5 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_ODIN_H_
#define SRC_ODIN_H_

#include "platform.h"
#include "gpio.h"
#include "odin_spi.h"
#include "uart.h"
#include "odin_typedefs.h"
#include "odin_spi_commands.h"
#include "odin_aer.h"

typedef struct{
	GPIO_Interface gpio_interface;
	XGpio *gpio;
	SPI_Interface *spi;
	u8 reset_pin;
	AER_Interface *aer_in;
	AER_Interface *aer_out;
}Odin;

void odin_initChip(Odin *odin, XGpio *gpio,
		void (*set_pin_direction_function) (u8, u8),
		void (*write_to_pin_function) (u8, u8),
		u8 (*read_from_pin_function) (u8));
void odin_enableChip(Odin *odin);
void odin_disableChip(Odin *odin);
void odin_enableOpenLoop(Odin *odin);
void odin_disableOpenLoop(Odin *odin);
int odin_isEventAtOutput(Odin *odin);
u8 odin_readEventAtOutput(Odin *odin);
int odin_stimulateNeuron(Odin *odin, u8 neuron_index, u8 synapse_value);
int odin_triggerPresynapticEvent(Odin *odin, u8 presynaptic_neuron_index);
int odin_triggerGlobalLeakageEvent(Odin *odin);
int odin_triggerLeakageEventForNeuron(Odin *odin, u8 neuron_index);

void printNeuron(UARTInterface *uart, Neuron neuron);
Synapse odin_getSynapse(Odin *odin, u8 preneuron, u8 postneuron);
void odin_setSynapseValue(Odin *odin, u8 preneuron, u8 postneuron, u8 synapse_value);
Neuron odin_getNeuron(Odin *odin, u8 neuron_index);
void odin_setNeuronProperties(Odin *odin, Neuron neuron_to_write);
#endif /* SRC_ODIN_H_ */
