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
void SPI_initInterface(SPI_Interface *spi, XGpio *gpio, unsigned char sclk_pin, unsigned char mosi_pin, unsigned char miso_pin,
		unsigned char write_width, unsigned char read_width){
		spi->sclk_pin = sclk_pin;
		spi->mosi_pin = mosi_pin;
		spi->miso_pin = miso_pin;
		spi->write_width = write_width;
		spi->read_width = read_width;

		// Set SCLK and MOSI to outputs.
		gpio_setPinDirection(gpio, spi->sclk_pin, OUTPUT);
		gpio_setPinDirection(gpio, spi->mosi_pin, OUTPUT);

		// Set MISO to input.
		gpio_setPinDirection(gpio, spi->miso_pin, INPUT);

		//Write idle values
		gpio_writeToPin(gpio, spi->sclk_pin, LOW);
		gpio_writeToPin(gpio, spi->mosi_pin, LOW);

}
/*
 * SPI_write: to write a single transaction. The length of the write
 * is defined during initialisation.
 */
void SPI_write(SPI_Interface *spi, XGpio *gpio, u32 to_write){
	signed int i;
	u32 masked_write;

	for(i=spi->write_width-1;i>=0;i--){
		gpio_writeToPin(gpio, spi->sclk_pin, LOW);
		//Write value then write high to clock. Delay then write low.
		 //Mask bit assume SPI message is MSB first.
		masked_write = to_write & (1<<i);
		if(masked_write>0)
			gpio_writeToPin(gpio, spi->mosi_pin, HIGH);
		else
			gpio_writeToPin(gpio, spi->mosi_pin, LOW);
		delay();
		gpio_writeToPin(gpio, spi->sclk_pin, HIGH);
		delay();
	}
	gpio_writeToPin(gpio, spi->sclk_pin, LOW);
	delay();

}
/*
 * SPI_read: reads from the SPI slave.
 */
u32 SPI_read(SPI_Interface *spi, XGpio *gpio){
	int i;
	u32 output_buffer = 0x00;
	gpio_writeToPin(gpio, spi->sclk_pin, LOW);
		delay();
		for(i=spi->read_width-1;i>=0;i--){
			//Send clock high, delay, read miso, send clock low.
			gpio_writeToPin(gpio, spi->sclk_pin, HIGH);
			if(gpio_readFromPin(gpio, spi->miso_pin)==HIGH){
				output_buffer = output_buffer | (0x01<<i);
			}else{
				output_buffer = output_buffer & ~(1<<i);
			}
			delay();
			gpio_writeToPin(gpio, spi->sclk_pin, LOW);
			/* Wait a small amount of time for SPI slave to update output */
			delay();
		}
		return output_buffer;
}
/*
 * SPI_writeRead: Completes a single write transaction followed by a read transaction
 */
u32 SPI_writeRead(SPI_Interface *spi, XGpio *gpio, u32 to_write){
	SPI_write(spi, gpio, to_write);
	return  SPI_read(spi, gpio);
}


