/*
 * SPI.cpp
 *
 * Created: 19/12/2018 11:52:31
 *  Author: Wesley
 */ 

#include "SPI.h"
#include <avr/io.h>

SPI::SPI()
{
	PORTC.DIR = 0xB0; // configure MOSI, SS, CLK as outputs

				// CLK2X	ENABLE	DORD	MASTER	MODE[1:0]	PRESCALER[1:0]
	SPIC.CTRL = 0x50;    // SPI master, clock idle low, data setup on trailing edge, data sampled on leading edge, double speed mode enabled
	SPIC.INTCTRL = 0x00; // ensure SPI interrupts are disabled;
	PORTC.OUTSET = 0x10;       // de-assert SS pin (active low)
	
	while(SPIC.STATUS & 0x80) {
		rx_byte = SPIC.DATA;   // flush spi receive buffer
	}
}

void SPI::transmit(uint8_t message){
	PORTC.OUTCLR = 0x10; // Assert SS
	SPIC.DATA = message;
	
	while(!(SPIC.STATUS & 0x80)); // Wait for transmit to complete	
	
	
	PORTC.OUTSET = 0x10; // Deassert
	
	tx_byte = SPIC.DATA;
}