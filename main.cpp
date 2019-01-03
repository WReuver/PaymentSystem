/*
 * StepperTest.cpp
 *
 * Created: 22/11/2018 12:09:03
 * Author : Wesley
 */ 

#include <avr/io.h>
#include <stdint.h>
#include "Includes.h"
#include "Hardware/Gpio.h"
#include "Stepper/Stepper.h"
#include "Hardware/SystemClock.h"
#include "SPI/SPI.h"
#include "RFID/MFRC522.h"

using namespace Hardware;
using namespace Gpio; 

#define F_CPU 32000000UL

Pin motorPin1 = Pin::D5;	// Eventual: D5		A5
Pin motorPin2 = Pin::D0;	//			 D0		A3
Pin motorPin3 = Pin::D6;	//			 D6		A6
Pin motorPin4 = Pin::D1;	//			 D1		A4
const uint16_t delayUS = 292;
const uint16_t delayMS = 100;

void initialize(void)
{
	SetPinDirection(motorPin1, Dir::Output);
	SetPinDirection(motorPin2, Dir::Output);
	SetPinDirection(motorPin3, Dir::Output);
	SetPinDirection(motorPin4, Dir::Output);
}

int main(void)
{
	initialize();
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
	
	SPI spi = SPI();
	spi.settings(SPI::Prescaler::DIV8, SPI::BitOrder::MSB_FIRST, SPI::Mode::Mode0);
	
	uint8_t msg = SPIC_CTRL;
	
    while (1) 
    {
		PORTC.OUTCLR = 0x10; // Assert slave
		spi.transfer(msg);		
		PORTC.OUTSET = 0x10; // Deassert slave
		_delay_ms(1000);
		
		MFRC522 mfrc522 = MFRC522();
		mfrc522.PCD_Init();
		
		if ( ! mfrc522.PICC_IsNewCardPresent())
		{
			Gpio::SetPinValue(Pin::D5, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::D5, Value::Low);
		}
		
		
    }
}

