/*
 * spi.h
 *
 *  Created on: 31 Aug 2023
 *      Author: danjo
 */

#ifndef SPI_H_
#define SPI_H_

#include "timer.h"
#include "odin_typedefs.h"
#include "hardware_specific.h"

typedef struct {
	unsigned char sclk_pin;
	unsigned char mosi_pin;
	unsigned char miso_pin;
	GPIO_Interface *gpio_interface;
	unsigned char write_width;
	unsigned char read_width;
} SPI_Interface;

void SPI_initInterface(SPI_Interface *spi, GPIO_Interface *gpio_interface,
		unsigned char sclk_pin, unsigned char mosi_pin, unsigned char miso_pin,
		unsigned char write_width, unsigned char read_width);
void SPI_write(SPI_Interface *spi, u32 to_write);
u32 SPI_read(SPI_Interface *spi);
u32 SPI_writeRead(SPI_Interface *spi, u32 to_write);

#endif /* SPI_H_ */
