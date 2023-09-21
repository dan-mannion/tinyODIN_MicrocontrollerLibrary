/*
 * odin_aer.h
 *
 *  Created on: 13 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_ODIN_AER_H_
#define SRC_ODIN_AER_H_

#include "xbasic_types.h"
#include "hardware_specific.h"

typedef struct{
	GPIO_Interface *gpio_interface;
	u8 req_pin;	//Pin number of the req pin.
	u8 ack_pin;	//Pin number of the ack pin.
	u16 address_pins[];	//Array to store the pin numbers of the address pin.
}AER_Interface;

AER_Interface* aer_initOutputInterface(AER_Interface *aer, GPIO_Interface *gpio_interface);
AER_Interface* aer_initInputInterface(AER_Interface *aer,  GPIO_Interface *gpio_interface);

void aer_writeToAddressLines(AER_Interface *aer, u16 aer_contents);
void aer_setRequest(AER_Interface *aer);
void aer_waitForAckSet(AER_Interface *aer);
void aer_resetRequest(AER_Interface *aer);
void aer_waitForAckReset(AER_Interface *aer);
void aer_writeMessage(AER_Interface *aer, u16 aer_contents);
void aer_stimulateNeuron(AER_Interface *aer, u8 neuron_index, u8 synapse_value);
void aer_triggerPresynapticEvent(AER_Interface *aer, u8 neuron_index);
void aer_triggerGlobalLeakageEvent(AER_Interface *aer);
void aer_triggerLeakageEventForNeuron(AER_Interface *aer, u8 neuron_index);
int aer_isRequest(AER_Interface *aer);
void aer_waitForRequestSet(AER_Interface *aer);
void aer_waitForRequestReset(AER_Interface *aer);
void aer_setAck(AER_Interface *aer);
void aer_resetAck(AER_Interface *aer);
u8 aer_readFromAERAddressLines(AER_Interface *aer);
u8 aer_readEventFromOutput(AER_Interface *aer);

#endif /* SRC_ODIN_AER_H_ */
