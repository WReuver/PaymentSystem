/*
 * SPI.h
 *
 * Created: 19/12/2018 11:52:22
 *  Author: Wesley
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "../Includes.h"
#include "../Hardware/Gpio.h"

class SPI {
	public:
		SPI();
		uint8_t rx_byte;
		uint8_t tx_byte = 0;
		
		void transmit(uint8_t message);
};



#endif /* SPI_H_ */