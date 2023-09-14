/*
 * spi.c
 *
 *  Created on: 31 Aug 2023
 *      Author: danjo
 */
#include "odin_spi.h"
/*
 * SPI_initInterface: initialises the GPIO pins of the SPI interface.
 */
void SPI_initInterface(SPI_Interface *spi, GPIO_Interface *gpio_interface, u8 sclk_pin, u8 mosi_pin, u8 miso_pin,
		unsigned char write_width, unsigned char read_width){
		spi->gpio_interface = gpio_interface;
		spi->sclk_pin = sclk_pin;
		spi->mosi_pin = mosi_pin;
		spi->miso_pin = miso_pin;
		spi->write_width = write_width;
		spi->read_width = read_width;

		// Set SCLK and MOSI to outputs.
		spi->gpio_interface->set_pin_direction(spi->sclk_pin, OUTPUT);
		spi->gpio_interface->set_pin_direction(spi->mosi_pin, OUTPUT);

		// Set MISO to input.
		spi->gpio_interface->set_pin_direction(spi->miso_pin, INPUT);

		//Write idle values
		spi->gpio_interface->write_to_pin(spi->sclk_pin, LOW);
		spi->gpio_interface->write_to_pin(spi->mosi_pin, LOW);
}
/*
 * SPI_write: to write a single transaction. The length of the write
 * is defined during initialisation.
 */
void SPI_write(SPI_Interface *spi, u32 to_write){
	signed int i;
	u32 masked_write;

	for(i=spi->write_width-1;i>=0;i--){
		spi->gpio_interface->write_to_pin(spi->sclk_pin, LOW);
		//Write value then write high to clock. Delay then write low.
		 //Mask bit assume SPI message is MSB first.
		masked_write = to_write & (1<<i);
		if(masked_write>0)
			spi->gpio_interface->write_to_pin(spi->mosi_pin, HIGH);
		else
			spi->gpio_interface->write_to_pin(spi->mosi_pin, LOW);
		delay();
		spi->gpio_interface->write_to_pin(spi->sclk_pin, HIGH);
		delay();
	}
	spi->gpio_interface->write_to_pin(spi->sclk_pin, LOW);
	delay();
}

/*
 * SPI_read: reads from the SPI slave.
 */
u32 SPI_read(SPI_Interface *spi){
	int i;
	u32 output_buffer = 0x00;
	spi->gpio_interface->write_to_pin(spi->sclk_pin, LOW);
		delay();
		for(i=spi->read_width-1;i>=0;i--){
			//Send clock high, delay, read miso, send clock low.
			spi->gpio_interface->write_to_pin(spi->sclk_pin, HIGH);
			if(spi->gpio_interface->read_from_pin(spi->miso_pin)==HIGH){
				output_buffer = output_buffer | (0x01<<i);
			}else{
				output_buffer = output_buffer & ~(1<<i);
			}
			delay();
			spi->gpio_interface->write_to_pin(spi->sclk_pin, LOW);
			/* Wait a small amount of time for SPI slave to update output */
			delay();
		}
		return output_buffer;
}
/*
 * SPI_writeRead: Completes a single write transaction followed by a read transaction
 */
u32 SPI_writeRead(SPI_Interface *spi, u32 to_write){
	SPI_write(spi, to_write);
	return  SPI_read(spi);
}


