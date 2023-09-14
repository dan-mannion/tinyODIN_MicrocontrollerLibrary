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

https://github.com/dan-mannion/tinyODIN_MicrocontrollerLibrary/blob/63ce51020466300bb6bb27979b2ead378d8fa062/odin.h#L28C1-L47C67


