/*
 * hardware_specific.h
 *
 *  Created on: 14 Sep 2023
 *      Author: danjo
 */

#ifndef SRC_HARDWARE_SPECIFIC_H_
#define SRC_HARDWARE_SPECIFIC_H_

/*
 * Pin definitions
 */
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

/*
 * End of pin definitions
 */

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
