/*
 * odin_aer.c
 *
 *  Created on: 13 Sep 2023
 *      Author: danjo
 */
#include "odin_aer.h"
#include <stdlib.h>

AER_Interface* aer_initInputInterface(AER_Interface *aer, XGpio *gpio){
	aer = malloc(sizeof(*aer)+sizeof(u16)*AER_IN_ADDRESS_WIDTH);
	aer->gpio = gpio;
	aer->req_pin = AER_IN_REQ_PIN;
	aer->ack_pin = AER_IN_ACK_PIN;
	u8 index;
	for(index=0;index<AER_IN_ADDRESS_WIDTH;index++){
		aer->address_pins[index] = AER_IN_ADDRESS_OFFSET+index;
		gpio_setPinDirection(aer->gpio, aer->address_pins[index], OUTPUT);
	}

	gpio_setPinDirection(aer->gpio, aer->req_pin, OUTPUT);
	gpio_writeToPin(aer->gpio, aer->req_pin, LOW);
	gpio_setPinDirection(aer->gpio, aer->ack_pin, INPUT);

	return aer;
}
AER_Interface* aer_initOutputInterface(AER_Interface *aer, XGpio *gpio){
	aer = malloc(sizeof(*aer)+sizeof(u16)*AER_OUT_ADDRESS_WIDTH);
	aer->gpio = gpio;
	aer->req_pin = AER_OUT_REQ_PIN;
	aer->ack_pin = AER_OUT_ACK_PIN;
	u8 index;
	for(index=0;index<AER_OUT_ADDRESS_WIDTH;index++){
		aer->address_pins[index] = AER_OUT_ADDRESS_OFFSET+index;
		gpio_setPinDirection(aer->gpio, aer->address_pins[index], INPUT);
	}
	gpio_setPinDirection(aer->gpio, aer->req_pin, INPUT);
	gpio_setPinDirection(aer->gpio, aer->ack_pin, OUTPUT);
	gpio_writeToPin(aer->gpio, aer->ack_pin, LOW);

	return aer;
}

void aer_writeToAddressLines(AER_Interface *aer, u16 aer_contents){
	u8 index;
	for(index = 0; index<AER_IN_ADDRESS_WIDTH; index++){
		if((aer_contents & (1<<index))>0){
			gpio_writeToPin(aer->gpio, aer->address_pins[index], HIGH);
		}else{
			gpio_writeToPin(aer->gpio, aer->address_pins[index], LOW);
		}
	}
}
void aer_setRequest(AER_Interface *aer){
	gpio_writeToPin(aer->gpio, aer->req_pin, HIGH);
}
void aer_waitForAckSet(AER_Interface *aer){
	display("Wait ACK set");
	while(gpio_readFromPin(aer->gpio, aer->ack_pin)==LOW){
		;
	}
}
void aer_resetRequest(AER_Interface *aer){
	gpio_writeToPin(aer->gpio, aer->req_pin, LOW);
}
void aer_waitForAckReset(AER_Interface *aer){
	display("Wait ACK reset");
	while(gpio_readFromPin(aer->gpio, aer->ack_pin)==HIGH){
			;
		}
}
void aer_writeMessage(AER_Interface *aer, u16 aer_contents){
	aer_waitForAckReset(aer);
	aer_writeToAddressLines(aer, aer_contents);
	aer_setRequest(aer);
	aer_waitForAckSet(aer);
	aer_resetRequest(aer);
	aer_waitForAckReset(aer);
}
void aer_stimulateNeuron(AER_Interface *aer, u8 neuron_index, u8 synapse_value){
	u16 aer_contents=0;
	//Write AER message to buffer
	bitset_u16(&aer_contents, 9);
	bitreset_u16(&aer_contents, 8);
	aer_contents |= ((u16)synapse_value)<<4;
	aer_contents |= neuron_index;
	aer_writeMessage(aer, aer_contents);
}
void aer_triggerGlobalLeakageEvent(AER_Interface *aer){
	u16 aer_contents = 0;
	bitreset_u16(&aer_contents, 9);
	bitset_u16(&aer_contents, 8);
	u8 index =0;
	for(index=0;index<=7;index++){
		bitset_u16(&aer_contents, index);
	}

	aer_writeMessage(aer, aer_contents);
}
void aer_triggerLeakageEventForNeuron(AER_Interface *aer, u8 neuron_index){
	u16 aer_contents = 0;
	bitreset_u16(&aer_contents, 9);
	bitset_u16(&aer_contents, 8);
	aer_contents |= neuron_index;

	aer_writeMessage(aer, aer_contents);
}
int aer_isRequest(AER_Interface *aer){
	return gpio_readFromPin(aer->gpio, aer->req_pin) == HIGH;
}
void aer_waitForRequestSet(AER_Interface *aer){
	while(gpio_readFromPin(aer->gpio, aer->req_pin)==LOW){
				;
			}
}
void aer_waitForRequestReset(AER_Interface *aer){
	while(gpio_readFromPin(aer->gpio, aer->req_pin)==HIGH){
				;
			}
}
void aer_setAck(AER_Interface *aer){
	gpio_writeToPin(aer->gpio, aer->ack_pin, HIGH);
}
void aer_resetAck(AER_Interface *aer){
	gpio_writeToPin(aer->gpio, aer->ack_pin, LOW);
}
u8 aer_readFromAERAddressLines(AER_Interface *aer){
	u8 address = 0;
	u8 index;
	for(index=0; index<AER_OUT_ADDRESS_WIDTH;index++){
		if(gpio_readFromPin(aer->gpio, aer->address_pins[index])==HIGH){
			address |= (1<<index);
		}
	}
	return address;
}
u8 aer_readEventFromOutput(AER_Interface *aer){
	aer_waitForRequestSet(aer);
	u8 address = aer_readFromAERAddressLines(aer);
	aer_setAck(aer);
	aer_waitForRequestReset(aer);
	aer_resetAck(aer);
	return address;
}
