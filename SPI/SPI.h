/*
 * SPI.h
 *
 * Created: 19/12/2018 11:52:22
 *  Author: Wesley Reuver
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "../Includes.h"
#include "../Hardware/Gpio.h"

class SPI {
	public:
		enum class Prescaler
		{
			DIV4 = 0x00,
			DIV16 = 0x01,
			DIV64 = 0x02,
			DIV128 = 0x03,
			DIV2 = 0x80,
			DIV8 = 0x81,
			DIV32 = 0x82
		};
	
		enum class BitOrder
		{
			MSB_FIRST,
			LSB_FIRST
		};
		
		enum class Mode
		{
			Mode0 = 0x00,
			Mode1 = 0x04,
			Mode2 = 0x08,
			Mode3 = 0x0C
		};
		
		
		SPI();
		uint8_t rx_byte;
		uint8_t tx_byte;
		
		void settings(Prescaler clockDiv, BitOrder bitOrder, Mode dataMode);
		uint8_t transfer(uint8_t data);
};



#endif /* SPI_H_ */