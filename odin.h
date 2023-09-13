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

#define ADDRESS_READ_OFFSET 19
#define ADDRESS_WRITE_OFFSET 18
#define ADDRESS_CMD_OFFSET 16
#define ADDRESS_CMD_WIDTH 2
#define ADDRESS_ADDRESS_OFFSET 0
#define ADDRESS_ADDRESS_WIDTH 16

typedef struct{
	u8 read;
	u8 write;
	u8 cmd;
	u16 address;
} SPIAddressField;
typedef struct{
	u8 mask; // Used to mask which bits of the data byte are written to memory.
	u8 data;
}SPIDataField;

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


void odin_spi_activateSPIGateActivity(SPI_Interface *spi, XGpio *gpio);
void odin_spi_deactivateSPIGateActivity(SPI_Interface *spi, XGpio *gpio);
void odin_spi_enableOpenLoop(SPI_Interface *spi, XGpio *gpio);
void odin_spi_disableOpenLoop(SPI_Interface *spi, XGpio *gpio);
u32 odin_addressToSPIBitStream(SPIAddressField address);
u32 odin_dataToSPIBitStream(SPIDataField datafield);

SynapseAddress odin_getSynapseAddress(u8 preneuron, u8 postneuron);
u32 odin_convertSynapseAddressToSPIFormat(SynapseAddress synapse);
void odin_spi_writeSynapseMemory(SPI_Interface *spi, XGpio *gpio, u8 preneuron_index, u8 postneuron_index, u8 synapse_value);
u32 odin_spi_readSynapseMemory(SPI_Interface *spi, XGpio *gpio, u8 preneuron_index, u8 postneuron_index);
Synapse odin_getSynapse(SPI_Interface *spi, XGpio *gpio, u8 preneuron, u8 postneuron);

void printNeuron(UARTInterface *uart, Neuron neuron);
u16 odin_convertNeuronAddressToSPIFormat(u8 neuron_index, u8 byte_address);
Neuron odin_spi_readNeuronMemory(SPI_Interface *spi, XGpio *gpio, u8 neuron_index, Neuron *neuron);
u32 odin_getMemoryRepresentationOfNeuron(Neuron neuron);
void odin_spi_writeNeuronMemory(SPI_Interface *spi, XGpio *gpio, Neuron neuron);


#endif /* SRC_ODIN_H_ */
