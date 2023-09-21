/*
 * odin_spi_commands.h
 *
 *  Created on: 13 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_ODIN_SPI_COMMANDS_H_
#define SRC_ODIN_SPI_COMMANDS_H_

#include "odin_typedefs.h"
#include "odin_spi.h"

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

SynapseAddress spi_getSynapseAddress(u8 preneuron, u8 postneuron);
u32 spi_addressToSPIBitStream(SPIAddressField address);
void spi_activateSPIGateActivity(SPI_Interface *spi);
void spi_deactivateSPIGateActivity(SPI_Interface *spi);
void spi_enableOpenLoop(SPI_Interface *spi);
void spi_disableOpenLoop(SPI_Interface *spi);
void spi_set_SPI_MAX_NEUR(SPI_Interface *spi, u8 max_neurons);

u32 spi_convertSynapseAddressToSPIFormat(SynapseAddress synapse);
u32 spi_dataToSPIBitStream(SPIDataField datafield);
void spi_writeSynapseMemory(SPI_Interface *spi, u8 preneuron_index, u8 postneuron_index, u8 synapse_value);
u32 spi_readSynapseMemory(SPI_Interface *spi, u8 preneuron_index, u8 postneuron_index);

u32 spi_getMemoryRepresentationOfNeuron(Neuron neuron);
u16 spi_convertNeuronAddressToSPIFormat(u8 neuron_index, u8 byte_address);
void spi_readNeuronMemory(SPI_Interface *spi, u8 neuron_index, Neuron *neuron);
void spi_writeNeuronMemory(SPI_Interface *spi, Neuron neuron);

#endif /* SRC_ODIN_SPI_COMMANDS_H_ */
