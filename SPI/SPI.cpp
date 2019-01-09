/*
 * SPI.cpp
 *
 * Created: 19/12/2018 11:52:31
 *  Author: Wesley Reuver
 */ 

#include "SPI.h"
#include <avr/io.h>

SPI::SPI()
{
	// 0b10110000	
	PORTC.DIR = 0xB0;			// Configure MOSI, SS, CLK as outputs
	SPIC.INTCTRL = 0x00;		// Disable SPI interrupts;
	PORTC.OUTSET = 0x10;		// De-assert SS pin (active low)
	
	while(SPIC.STATUS & 0x80) {
		rx_byte = SPIC.DATA;	// Flush SPI receive buffer
	}
}

void SPI::settings(Prescaler clockDiv, BitOrder bitOrder, Mode dataMode)
{
	// CLK2X	ENABLE	DORD	MASTER	MODE[1:0]	PRESCALER[1:0]
	SPIC.CTRL =  SPI_ENABLE_bm | SPI_MASTER_bm | (uint8_t) clockDiv | (uint8_t) bitOrder | (uint8_t) dataMode; // Set the SPI control register C to the specified settings
	SPIC.INTCTRL = 0x00;		// Disable SPI interrupts;
	PORTC.OUTSET = 0x10;		// De-assert SS pin (active low)
	
	while(SPIC.STATUS & 0x80) {
		rx_byte = SPIC.DATA;	// Flush spi receive buffer
	}
}

uint8_t SPI::transfer(uint8_t data){
	SPIC.DATA = data;	
	while(!(SPIC.STATUS & 0x80)); // Wait for transmit to complete
	
	return SPIC.DATA;
}