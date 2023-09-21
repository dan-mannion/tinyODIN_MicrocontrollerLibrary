/*
 * odin.c
 *
 *  Created on: 5 Sep 2023
 *      Author: danjo
 */
#include "odin.h"
#include "macros.h"
#include "uart.h"
#include "hardware_specific.h"


void odin_initChip(Odin *odin, XGpio *gpio,
		void (*set_pin_direction_function) (u8, u8),
		void (*write_to_pin_function) (u8, u8),
		u8 (*read_from_pin_function) (u8)){
	odin->gpio_interface.set_pin_direction = set_pin_direction_function;
	odin->gpio_interface.write_to_pin= write_to_pin_function;
	odin->gpio_interface.read_from_pin = read_from_pin_function;
	odin->gpio = gpio;

	odin->reset_pin = RESET_PIN;
	odin->gpio_interface.set_pin_direction(odin->reset_pin, OUTPUT);
	odin_disableChip(odin);

	u8 write_width = 20;
	u8 read_width = 20;
	SPI_initInterface(odin->spi, &(odin->gpio_interface), SPI_SCLK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, write_width, read_width);

	odin->aer_in = aer_initInputInterface(odin->aer_in, &(odin->gpio_interface));
	odin->aer_out = aer_initOutputInterface(odin->aer_out, &(odin->gpio_interface));

	odin_enableChip(odin);
	odin_enableOpenLoop(odin);
	odin_setMaxNeuronIndexToBeProcessed(odin, 0xff);
}
void odin_enableChip(Odin *odin){
	odin->gpio_interface.write_to_pin(odin->reset_pin, LOW);
}
void odin_disableChip(Odin *odin){
	odin->gpio_interface.write_to_pin(odin->reset_pin, HIGH);
}
void odin_enableOpenLoop(Odin *odin){
	spi_activateSPIGateActivity(odin->spi);
	spi_enableOpenLoop(odin->spi);
	spi_deactivateSPIGateActivity(odin->spi);
}
void odin_disableOpenLoop(Odin *odin){
	spi_activateSPIGateActivity(odin->spi);
	spi_disableOpenLoop(odin->spi);
	spi_deactivateSPIGateActivity(odin->spi);
}
void odin_setMaxNeuronIndexToBeProcessed(Odin *odin, u8 max_neurons){
	spi_activateSPIGateActivity(odin->spi);
	spi_set_SPI_MAX_NEUR(odin->spi, max_neurons);
	spi_deactivateSPIGateActivity(odin->spi);
}

int odin_isEventAtOutput(Odin *odin){
	if(aer_isRequest(odin->aer_in)){
		return 1;
	}else{
		return 0;
	}
}
u8 odin_readEventAtOutput(Odin *odin){
	u8 address;
	address = aer_readEventFromOutput(odin->aer_in);
	return address;
}
int odin_stimulateNeuron(Odin *odin, u8 neuron_index, u8 synapse_value){
	if(!odin_isEventAtOutput(odin)){
		aer_stimulateNeuron(odin->aer_out, neuron_index, synapse_value);
		return 1;
	}else{
		return 0;
	}

}
int odin_triggerPresynapticEvent(Odin *odin, u8 presynaptic_neuron_index){
	if(!odin_isEventAtOutput(odin)){
			aer_triggerPresynapticEvent(odin->aer_out, presynaptic_neuron_index);
			return 1;
		}else{
			return 0;
		}
}
int odin_triggerGlobalLeakageEvent(Odin *odin){
	if(!odin_isEventAtOutput(odin)){
		aer_triggerGlobalLeakageEvent(odin->aer_out);
		return 1;
	}else{
		return 0;
	}
}
int odin_triggerLeakageEventForNeuron(Odin *odin, u8 neuron_index){
	if(!odin_isEventAtOutput(odin)){
			aer_triggerLeakageEventForNeuron(odin->aer_out, neuron_index);
			return 1;
		}else{
			return 0;
		}
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

Synapse odin_getSynapse(Odin *odin, u8 preneuron, u8 postneuron){
	Synapse synapse;
	synapse.preneuron = preneuron;
	synapse.postneuron = postneuron;
	synapse.memory_address = spi_getSynapseAddress(preneuron, postneuron);
	spi_activateSPIGateActivity(odin->spi);
	synapse.value = spi_readSynapseMemory(odin->spi, preneuron, postneuron);
	spi_deactivateSPIGateActivity(odin->spi);
	return synapse;
}
u8 odin_getSynapseWeight(Odin *odin, u8 preneuron, u8 postneuron){
	spi_activateSPIGateActivity(odin->spi);
	u8 weight = spi_readSynapseMemory(odin->spi, preneuron, postneuron);
	spi_deactivateSPIGateActivity(odin->spi);
	return weight;
}
void odin_setSynapseValue(Odin *odin, u8 preneuron, u8 postneuron, u8 synapse_value){
	spi_activateSPIGateActivity(odin->spi);
	spi_writeSynapseMemory(odin->spi, preneuron, postneuron, synapse_value);
	spi_deactivateSPIGateActivity(odin->spi);
}
Neuron odin_getNeuron(Odin *odin, u8 neuron_index){
	Neuron neuron;
	spi_activateSPIGateActivity(odin->spi);
	spi_readNeuronMemory(odin->spi, neuron_index, &neuron);
	spi_deactivateSPIGateActivity(odin->spi);
	return neuron;
}
u16 odin_getNeuronMembranePotential(Odin *odin, u8 neuron_index){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	return neuron.membrane_potential;
}
u16 odin_getNeuronThreshold(Odin *odin, u8 neuron_index){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	return neuron.threshold;
}
u8 odin_getNeuronLeakageTerm(Odin *odin, u8 neuron_index){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	return neuron.leakage_value;
}
u8 odin_getNeuronDisabledState(Odin *odin, u8 neuron_index){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	return neuron.disabled;
}
void odin_setNeuronProperties(Odin *odin, Neuron neuron_to_write){
	spi_activateSPIGateActivity(odin->spi);
	spi_writeNeuronMemory(odin->spi, neuron_to_write);
	spi_deactivateSPIGateActivity(odin->spi);
}

void odin_setNeuronMembranePotential(Odin *odin, u8 neuron_index, u16 membrane_potential){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	neuron.membrane_potential = membrane_potential;
	odin_setNeuronProperties(odin, neuron);
}
void odin_setNeuronThreshold(Odin *odin, u8 neuron_index, u16 threshold){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	neuron.threshold = threshold;
	odin_setNeuronProperties(odin, neuron);
}
void odin_setNeuronLeakageTerm(Odin *odin, u8 neuron_index, u8 leakage_term){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	neuron.leakage_value = leakage_term;
	odin_setNeuronProperties(odin, neuron);
}
void odin_setNeuronDisabledState(Odin *odin, u8 neuron_index, u8 disabled_state){
	Neuron neuron = odin_getNeuron(odin, neuron_index);
	neuron.disabled = disabled_state;
	odin_setNeuronProperties(odin, neuron);
}



