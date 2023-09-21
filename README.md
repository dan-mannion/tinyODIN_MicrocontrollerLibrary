# tinyODIN_MicrocontrollerLibrary
This repository contains functions to make interfacing with tinyODIN quick and simple. It is designed to be used within code operating on a microcontroller with direct GPIO connetions to the revelent pins of the FPGA running an instance of tinyODIN. 

# Getting Started
The code is designed to hide the details of interacting with ODIN to make adoption quick and simple. For this reason you should only need to work with the functions contained within odin.c/odin.h. These functions allow you to:
  - Create an instance of the Odin chip.
  - Set configuration registers. 
  - Read/write synapse values.
  - Read/write the properties of neuron cores including membrane potential, threshold value and leakage term.
  - Enable and disable neurons.
  - Generate a virtual spiking event.
  - Trigger a presynaptic neuron spiking event.
  - Trigger a time reference (leakage event) in either a specific neuron or globally.
  - Read output spiking events from ODIN's output AER interface.
An extract from the header file containing these functions is shown below.
## Initial Mandatory Setup
To begin using the provided functions, there are two key tasks we must do before we can being to interact with ODIN. These are:
1. Define the physical connections between the pins of the microcontroller and the pins of tinyODIN.
2. Define functions to do the following: set a GPIO pin's direction, write to a GPIO pin, and read from a GPIO pin.
How to implement these functions will depend on the micrcontroller you are doing. However, below is an example for when using the Zynq 7000 SoC.
### Defining physical pin connections
The pin mappings are defined in [hardware_specific.h](https://github.com/dan-mannion/tinyODIN_MicrocontrollerLibrary/blob/be1a8a89885529c55dd56d775294fd33ca26cbc1/hardware_specific.h#L14C1-L28C1). An example is shown below. Note that ODIN's clock pin is not referenced in this file. This is because the library does not attempt to drive this clock signal. Instead, the user must provide an external clock for ODIN or generate this signal using the microcontroller. As specified in the [tinyODIN repository](https://github.com/ChFrenkel/tinyODIN/tree/main/doc) the clock must be 4 times faster than the clock of the SPI signal. 
```c
#define RESET_PIN 2
#define SPI_SCLK_PIN 0
#define SPI_MOSI_PIN 1
#define SPI_MISO_PIN 3

#define AER_OUT_REQ_PIN 5
#define AER_OUT_ACK_PIN 4
#define AER_OUT_ADDRESS_WIDTH 10
#define AER_OUT_ADDRESS_OFFSET 6

#define AER_IN_REQ_PIN 16
#define AER_IN_ACK_PIN 17
#define AER_IN_ADDRESS_WIDTH 8
#define AER_IN_ADDRESS_OFFSET 18
```
### Defining GPIO functions
So that this library remains largely agnostic to the platform it is being run on, it assumes no knowledge of how manage GPIO pins. For this reason when we initialise the Odin struct using ```odin_initChip``` we must pass pointers to three important functions:
1. A function to set the direction of a GPIO pin.
2. A function to write to a GPIO pin.
3. A function to read from a GPIO pin.

To use this library we must write these functions for our specific microcontroller and pass the pointers to these functions. When writing the functions they should adhere to the following definitions:
```c
void setPinDirection(u8 pin, u8 mode);
void writeToPin(u8 pin, u8 value);
u8 readFromPin(u8 pin);
```
For example, the setPinDirection function should accept two parameters of type u8. The first parameter corresponds to the pin number of the gpio in question and the second refers to whether the pin is being set to an INPUT and an OUTPUT. If the pin is being set as an input the value 1 will be passed; for an output the value 0 will be passed. The code we write must handle these parameters. Below we have given examples for each of these functions if we were running our code on an Arduino based microcontroller. 
```c
// Example GPIO function definitions for Arduino based microcontroller. 
void my_gpioSetPinDirection(u8 pin, u8 mode){
	switch(mode){
		case(0):
			pinMode(pin, OUTPUT);
			break;
		case(1):
			pinMode(pin, INTPUT);
			break;
	}
}
void my_gpioWriteToPin(u8 pin, u8 value){
	switch(value){
		case(0):
			digitalWrite(pin, LOW);
			break;
		case(1):
			digitalWrite(pin, HIGH);
			break;
	}
}
u8 my_gpioReadFromPin(u8 pin){
	return (u8) (digitalRead(pin));
}
```
Once these functions have been defined, we then pass these during the initialisiation of the odin struct as shown below:
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, my_gpioSetPinDirection, my_gpioWriteToPin, my_gpioReadFromPin);
}
```
## Initialising Odin and Setting Configuration Registers
Once the physical pin mappings have been defined, and the custom functions for reading and writing to the GPIOs of your specific chip are defined, we can then instantiate a instance of the Odin chip using the code below. In general, when using a function provided by this library you will need to pass the pointer to the instance of the Odin chip. In this example the ``` odin_enableChip(&odin) ``` function is used to ensure the reset pin is pulled low ensuring ODIN is running and not currently reset. 
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin);
	odin_setMaxNeuronIndexToBeProcessed(&odin, 10); // Neurons unto and including this index will be processed. This should be set to the maximum number of neurons being used. 
}
```

## Reading and Writing Synapse Values
Setting synapses values can be done using the ```odin_setSynapseValue``` function. It takes the indices of the presynaptic and post synaptic neuron which the synapse connects as well as the vlaue you wish to set the weight to. The weights of ODIN are a 4bit signed weight, you should not enter a value outside of this range. 
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin).

	u8 preneuron = 0;
	u8 postneuron = 1;
	u8 synapse_weight = 2;
	odin_setSynapseValue(&odin, preneuron, postneuron, snapse_weight);
}
```
If we want to read the value of a synapse we have to options. We can either use the ```odin_getSynapse``` functions which returns a ```Synapse``` struct or ```odin_getSynapseWeight``` to read only the weight's value. 
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin).

	u8 preneuron = 0;
	u8 postneuron = 1;

	Synapse synapse_read = odin_getSynapse(&odin, preneuron, postneuron);

	// Alternatively, to just read the value of the weight:
	u8 synapse_weight = odin_getSynapseWeight(&odin, preneuron, postneuron);
}
```
## Reading and Writing Neuron Properties
When handling neurons it is useful to use the ```Neuron``` struct typedef. This struct type contains all the properties associated with a neuron within ODIN. The struct is shown below. 
```c
typedef struct{
	u16 index;
	u16 membrane_potential;
	u16 threshold;
	u8 leakage_value;
	u8 disabled;
}Neuron;
```
When reading and writing neurons a struct of this type should be used. In the demonstration below we first write the settings of the neuron at index 0, then read the neuron at index 1. 
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin).

	// Write to neuron at index 0
	Neuron neuron;
	neuron.index = 0;
	neuron.membrane_potential = 0;
	neuron.threshold = 0x8;
	neuron.leakage_value = 1;
	neuron.disabled = 0;

	odin_setNeuronProperties(&odin, neuron);

	// Read from neuron at index 1
	neuron = odin_getNeuron(&odin, 1);
	
}

```
Similar to the synapses, there are functions to write/read only specific properties of the neurons as opposed to defining the entire neuron structure. The prototypes for these functions are shown below. Note that these functions are not more efficient than reading the entire neuron state with the ```odin_getNeuron``` function. Each of these functions call ```odin_getNeuron``` but return only a select element of the ```Neuron``` struct. These functions could be optimised by modifying them to read only a portion of the neuron's memory relevent to the property in question. 
```c
// Functions for reading specific properties of a neuron
u16 odin_getNeuronMembranePotential(Odin *odin, u8 neuron_index);
u16 odin_getNeuronThreshold(Odin *odin, u8 neuron_index);
u8 odin_getNeuronLeakageTerm(Odin *odin, u8 neuron_index);
u8 odin_getNeuronDisabledState(Odin *odin, u8 neuron_index);
// Functions for setting specific properties of a neuron
void odin_setNeuronMembranePotential(Odin *odin, u8 neuron_index, u16 membrane_potential);
void odin_setNeuronThreshold(Odin *odin, u8 neuron_index, u16 threshold);
void odin_setNeuronLeakageTerm(Odin *odin, u8 neuron_index, u8 leakage_term);
void odin_setNeuronDisabledState(Odin *odin, u8 neuron_index, u8 disabled_state);
```

## AER Events: Triggering spikes, leakage events and reading outputs
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin).

	u8 preneuron = 0;
	u8 postneuron = 1;
	u8 synapse_weight = 2;
	odin_setSynapseValue(&odin, preneuron, postneuron, snapse_weight);

	odin_triggerPresynapticEvent(&odin, preneuron);
	odin_triggerGlobalLeakageEvent(&odin);
}
```

# Code Walkthrough
```c
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
```

When using these functions, you will encounter three key struct types: The Odin struct type represents the ODIN chip and stores both the AER and SPI interfaces. You shouldn't need to modify or handle the contents of this type. It is defined in odin.h.
```c
typedef struct{
	GPIO_Interface gpio_interface;
	SPI_Interface *spi;
	u8 reset_pin;
	AER_Interface *aer_in;
	AER_Interface *aer_out;
}Odin;
```
The Neuron and Synapse types represent the neurons and synapses of the chip. The definintions of these structs are found in odin_typedefs.h. 
```c
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
```
