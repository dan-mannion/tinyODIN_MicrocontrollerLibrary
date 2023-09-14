/*
 * hardware_specific.h
 *
 *  Created on: 14 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_HARDWARE_SPECIFIC_H_
#define SRC_HARDWARE_SPECIFIC_H_

#define INPUT 1
#define OUTPUT 0
#define HIGH 1
#define LOW 0

typedef struct{
	void (*set_pin_direction) (u8, u8);
	void (*write_to_pin) (u8, u8);
	u8 (*read_from_pin) (u8);
}GPIO_Interface;

#endif /* SRC_HARDWARE_SPECIFIC_H_ */
