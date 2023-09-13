/*
 * odin.c
 *
 *  Created on: 5 Sep 2023
 *      Author: danjo
 */
#include "odin.h"
#include "macros.h"
#include "odin_spi.h"
#include "uart.h"



u32 odin_addressToSPIBitStream(SPIAddressField address){
	int i;
	u32 reg = 0;

	if(address.read == 1){
		bitset(&reg, ADDRESS_READ_OFFSET);
	}else{
		bitset(&reg, ADDRESS_READ_OFFSET);
	}

	if(address.write == 1){
			bitset(&reg, ADDRESS_WRITE_OFFSET);
	}else{
			bitreset(&reg, ADDRESS_WRITE_OFFSET);
	}

	for(i=ADDRESS_CMD_WIDTH-1;i>=0;i--){
		if((address.cmd & (1<<(i)))==0){
			bitreset(&reg, ADDRESS_CMD_OFFSET+i);
		}else{
			bitset(&reg, ADDRESS_CMD_OFFSET+i);
		}
	}
	for(i=ADDRESS_ADDRESS_WIDTH-1;i>=0;i--){
			if((address.address & (1<<(i)))==0){
				bitreset(&reg, ADDRESS_ADDRESS_OFFSET+i);
			}else{
				bitset(&reg, ADDRESS_ADDRESS_OFFSET+i);
			}
		}
	return reg;
}

void odin_spi_activateSPIGateActivity(SPI_Interface *spi, XGpio *gpio){
	u32 address = 0;
	u32 data = 0;

	SPIAddressField addr;
	addr.read = 0;
	addr.write = 0;
	addr.cmd = 0b00;
	addr.address = 0;
	address = odin_addressToSPIBitStream(addr);

	bitset(&data, 0); // Set to gate network activity.

	SPI_write(spi, gpio, address);
	SPI_write(spi, gpio, data);
}
void odin_spi_deactivateSPIGateActivity(SPI_Interface *spi, XGpio *gpio){
	u32 address = 0;
	u32 data = 0;

	SPIAddressField addr;
	addr.read = 0;
	addr.write = 0;
	addr.cmd = 0b00;
	addr.address = 0;
	address = odin_addressToSPIBitStream(addr);

	bitreset(&data, 0); // Set to gate network activity.

	SPI_write(spi, gpio, address);
	SPI_write(spi, gpio, data);
}
void odin_spi_enableOpenLoop(SPI_Interface *spi, XGpio *gpio){
	u32 address = 0;
	u32 data = 0;

	SPIAddressField addr;
	addr.read = 0;
	addr.write = 0;
	addr.cmd = 0b00;
	addr.address = 1;
	address = odin_addressToSPIBitStream(addr);

	bitset(&data, 0); // Set to gate network activity.

	SPI_write(spi, gpio, address);
	SPI_write(spi, gpio, data);
}
void odin_spi_disableOpenLoop(SPI_Interface *spi, XGpio *gpio){
	u32 address = 0;
	u32 data = 0;

	SPIAddressField addr;
	addr.read = 0;
	addr.write = 0;
	addr.cmd = 0b00;
	addr.address = 1;
	address = odin_addressToSPIBitStream(addr);

	bitreset(&data, 0); // Set to gate network activity.

	SPI_write(spi, gpio, address);
	SPI_write(spi, gpio, data);
}
SynapseAddress odin_getSynapseAddress(u8 preneuron, u8 postneuron){
	SynapseAddress synapse;

	u16 post_neuron_masked_bits_7_3 = (((u16) postneuron)&(0xF8))>>3;
	u16 post_neuron_masked_bits_2_1 = (((u16) postneuron)&(0x06))>>1;
	u8 post_neuron_masked_bit_0 = (u8) (postneuron&0x01);

	synapse.word_address = 0;
	synapse.word_address |= post_neuron_masked_bits_7_3;
	synapse.word_address |= ((u16) preneuron)<<5;

	synapse.byte_address = 0;
	synapse.byte_address |= post_neuron_masked_bits_2_1;
	synapse.upper_lower_nibble = post_neuron_masked_bit_0;

	return synapse;
}
u32 odin_convertSynapseAddressToSPIFormat(SynapseAddress synapse){
	u32 synapse_address_spi_format = 0;
	synapse_address_spi_format |= synapse.word_address;
	synapse_address_spi_format |= synapse.byte_address << 8;
	return synapse_address_spi_format;
}

u32 odin_dataToSPIBitStream(SPIDataField datafield){
	u32 datafield_spi = 0;
	datafield_spi |= datafield.mask << 8;
	datafield_spi |= datafield.data;
	return datafield_spi;
}
void odin_spi_writeSynapseMemory(SPI_Interface *spi, XGpio *gpio, u8 preneuron_index, u8 postneuron_index, u8 synapse_value){
	u32 address=0;
	u32 data_spi_format = 0;

	SynapseAddress synapse_address = odin_getSynapseAddress(preneuron_index, postneuron_index);
	u32 synapse_address_spi_format = odin_convertSynapseAddressToSPIFormat(synapse_address);

	SPIAddressField addr;
	addr.read = 0;
	addr.write = 1;
	addr.cmd = 0b10;
	addr.address = synapse_address_spi_format;
	address = odin_addressToSPIBitStream(addr);

	SPIDataField datafield;
	if(synapse_address.upper_lower_nibble==0){
		datafield.mask = 0xF0;
		datafield.data = synapse_value;
	}else{
		datafield.mask = 0x0F;
		datafield.data = synapse_value<<4;
	}

	data_spi_format = odin_dataToSPIBitStream(datafield);


	SPI_write(spi,gpio, address);
	SPI_write(spi,gpio, data_spi_format);
}
u32 odin_spi_readSynapseMemory(SPI_Interface *spi, XGpio *gpio, u8 preneuron_index, u8 postneuron_index){
	u32 address_bitstream=0;
	u32 received = 0;

	SynapseAddress synapse_address = odin_getSynapseAddress(preneuron_index, postneuron_index);
	u32 synapse_address_spi_format = odin_convertSynapseAddressToSPIFormat(synapse_address);

	SPIAddressField addr;
	addr.read = 1;
	addr.write = 0;
	addr.cmd = 0b10;
	addr.address = synapse_address_spi_format;

	address_bitstream = odin_addressToSPIBitStream(addr);

	received = SPI_writeRead(spi, gpio, address_bitstream);
	// Now mask received depending on whether upper or lower nibble.
	u8 mask;
	if(synapse_address.upper_lower_nibble==0){
		mask = 0x0F;
		received = received & mask;
	}else{
		mask = 0xF0;
		received = (received & mask)>>4;
	}
	return received;
}
Synapse odin_getSynapse(SPI_Interface *spi, XGpio *gpio, u8 preneuron, u8 postneuron){
	Synapse synapse;
	synapse.preneuron = preneuron;
	synapse.postneuron = postneuron;
	synapse.memory_address = odin_getSynapseAddress(preneuron, postneuron);
	synapse.value = odin_spi_readSynapseMemory(spi, gpio, preneuron, postneuron);
	return synapse;
}

void printNeuron(UARTInterface *uart, Neuron neuron){
	uart_print(uart, "\tMembrane Potential: ");
	    		uart_printu16(uart, neuron.membrane_potential);
	    		uart_printLine(uart, "");
	    		uart_print(uart, "\tThreshold: ");
	    		uart_printu16(uart, neuron.threshold);
	    		uart_printLine(uart, "");
	    		uart_print(uart, "\tLeakage: ");
	    		uart_printu8(uart, neuron.leakage_value);
	    		uart_printLine(uart, "");
	    		uart_print(uart, "\tDisabled: ");
	    		uart_printu8(uart, neuron.disabled);
	    		uart_printLine(uart, "");
}

u16 odin_convertNeuronAddressToSPIFormat(u8 neuron_index, u8 byte_address){
	u32 spi_format = 0;
	spi_format |= neuron_index;
	spi_format |= byte_address<<8;
	return spi_format;
}
Neuron odin_spi_readNeuronMemory(SPI_Interface *spi, XGpio *gpio, u8 neuron_index, Neuron *neuron){
	u32 address_bitstream = 0;
	u32 received = 0;
	u32 raw_neuron_memory=0;

	neuron->index = neuron_index;
	SPIAddressField addr;
	addr.read = 1;
	addr.write = 0;
	addr.cmd = 0b01;
	u8 current_byte;
	for(current_byte=0; current_byte<4; current_byte++){
		// Need to read all four bytes;

		addr.address = odin_convertNeuronAddressToSPIFormat(neuron_index, current_byte);

		address_bitstream = odin_addressToSPIBitStream(addr);
		received = SPI_writeRead(spi, gpio, address_bitstream);
		raw_neuron_memory |= ((received&(0xff))<<(current_byte*8));
	}

	neuron->membrane_potential = raw_neuron_memory&(0xfff);
	neuron->threshold = (raw_neuron_memory&(0xfff << 12))>>12;
	neuron->leakage_value = (raw_neuron_memory&(0x7f << 24))>>24;
	neuron->disabled = (raw_neuron_memory & (0x01 << 31))>>31;
}
u32 odin_getMemoryRepresentationOfNeuron(Neuron neuron){
	u32 neuron_entry = 0;
	neuron_entry |= neuron.disabled << 31;
	neuron_entry |= neuron.leakage_value << 24;
	neuron_entry |= neuron.threshold << 12;
	neuron_entry |= neuron.membrane_potential;
	return neuron_entry;
}
void odin_spi_writeNeuronMemory(SPI_Interface *spi, XGpio *gpio, Neuron neuron){
	u32 address_bitstream=0;
	SPIAddressField addr;
	addr.read = 0;
	addr.write = 1;
	addr.cmd = 0b01;

	SPIDataField datafield;
	u32 data_spi_format;
	datafield.mask = 0;
	u32 neuron_memory_format = odin_getMemoryRepresentationOfNeuron(neuron);
	u8 current_byte;
	for(current_byte=0; current_byte<4; current_byte++){
		// Need to read all four bytes;
		addr.address = odin_convertNeuronAddressToSPIFormat(neuron.index, current_byte);
		address_bitstream = odin_addressToSPIBitStream(addr);
		datafield.data = (neuron_memory_format & (0xff << (8*current_byte))) >> (8*current_byte);
		data_spi_format = odin_dataToSPIBitStream(datafield);

		SPI_write(spi, gpio, address_bitstream);
		SPI_write(spi, gpio, data_spi_format);
	}
}


