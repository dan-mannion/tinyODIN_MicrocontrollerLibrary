# tinyODIN_MicrocontrollerLibrary
This repository contains functions to make interfacing with tinyODIN quick and simple. It is designed to be used within code operating within a microcontroller which has direct GPIO connetions to the revelent pins of the FPGA running an instance of tinyODIN. 

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
### Defining GPIO functions
## Initialising Odin and Setting Configuration Registers
Once the physical pin mappings have been defined, and the custom functions for reading and writing to the GPIOs of your specific chip are defined, we can then instantiate a instance of the Odin chip using the code below. In general, when using a function provided by this library you will need to pass the pointer to the instance of the Odin chip. In this example the ``` odin_enableChip(&odin) ``` function is used to ensure the reset pin is pulled low ensuring ODIN is running and not currently reset. 
```c
#include "odin.h"
void main(){
	Odin odin;
	odin_initChip(&odin, setPinDirection, writeToPin, readFromPin);
	odin_enableChip(&odin). 
}
```

## Reading and Writing Synapse Values

## Reading and Writing Neuron Properties

## AER Events: Triggering spikes, leakage events and reading outputs

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

When using these functions, you will encounter three key struct types: The Odin struct type represents the ODIN chip and stores both the AER and SPI interfaces. You should need to modify or handle the contents of this type. It is defined in odin.h.
```c
typedef struct{
	GPIO_Interface gpio_interface;
	XGpio *gpio;
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
