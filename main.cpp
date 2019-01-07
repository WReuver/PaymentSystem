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
#include "Sensors/RotationSensor.h"
#include "Hardware/SystemClock.h"
#include "SPI/SPI.h"
#include "RFID/MFRC522.h"

using namespace Hardware;
using namespace Gpio; 
using namespace Sensors;

#define F_CPU 32000000UL

Pin motorPin1 = Pin::D5;	// Eventual: D5		A5
Pin motorPin2 = Pin::D0;	//			 D0		A3	
Pin motorPin3 = Pin::D6;	//			 D6		A6
Pin motorPin4 = Pin::D1;	//			 D1		A4

void initialize(void)
{
	SetPinDirection(motorPin1, Dir::Output);
	SetPinDirection(motorPin2, Dir::Output);
	SetPinDirection(motorPin3, Dir::Output);
	SetPinDirection(motorPin4, Dir::Output);
	
	SetPinDirection(Pin::B5, Dir::Input);
}

int main(void)
{
	initialize();
	// System Clock
	
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
	
	// Stepper Motor
	Stepper stepper = Stepper(512, Pin::D5, Pin::D0, Pin::D6, Pin::D1);
	
	// IR sensor
	Pin rotationSensorPins[1] = { Pin::B5 };
	RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);
	
	// SPI
	SPI spi = SPI();
	spi.settings(SPI::Prescaler::DIV8, SPI::BitOrder::MSB_FIRST, SPI::Mode::Mode0);
	
	// RFID
	MFRC522 mfrc522 = MFRC522();
	mfrc522.PCD_Init();
	
	// Debug Vars
	uint8_t msg = SPIC_CTRL;
	uint8_t byte1 = 0;
	
    while (1) 
    {
		/*PORTC.OUTCLR = 0x10; // Assert slave
		spi.transfer(msg);		
		PORTC.OUTSET = 0x10; // Deassert slave
		_delay_ms(1000);*/
				
		if (rotationSensor->getData() == 0b00000001 ){
			Gpio::SetPinValue(Pin::D0, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::D0, Value::Low);
		}
		if ( ! mfrc522.PICC_IsNewCardPresent())
		{
			Gpio::SetPinValue(Pin::D5, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::D5, Value::Low);
		}
		
		if ( ! mfrc522.PICC_ReadCardSerial())
		{
			Gpio::SetPinValue(Pin::D6, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::D6, Value::Low);
			byte1 = mfrc522.uid.uidByte[0];
		}
		
    }
}

