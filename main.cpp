/*
 * StepperTest.cpp
 *
 * Created: 22/11/2018 12:09:03
 * Author : Wesley Reuver
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

Pin motorPin1 = Pin::D2;
Pin motorPin2 = Pin::D1;
Pin motorPin3 = Pin::D3;
Pin motorPin4 = Pin::D0;

void initialize(void)
{
	SetPinDirection(motorPin1, Dir::Output);
	SetPinDirection(motorPin2, Dir::Output);
	SetPinDirection(motorPin3, Dir::Output);
	SetPinDirection(motorPin4, Dir::Output);
	
	SetPinDirection(Pin::B0, Dir::Input);		// IR SENSE 1  IN
	SetPinDirection(Pin::E1, Dir::Output);		// LED 1
	SetPinDirection(Pin::E4, Dir::Output);		// LED 1
	SetPinDirection(Pin::E5, Dir::Output);		// LED 1
	SetPinDirection(Pin::E6, Dir::Output);		// LED 2
	SetPinDirection(Pin::E7, Dir::Output);		// LED 3
	SetPinDirection(Pin::D4, Dir::Output);		// LLC Output Enable
}

int main(void)
{
	initialize();
	// System Clock
	
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
	
	// Stepper Motor
	Stepper stepper = Stepper(512, Pin::D2, Pin::D1, Pin::D3, Pin::D0);
	SetPinValue(Pin::D4, Value::High);
	//Gpio::SetPinValue(Pin::E4, Value::High);
	stepper.step(512);
	
	// IR sensor
	Pin rotationSensorPins[1] = { Pin::B0 };
	RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);
	
	// SPI
	SPI spi = SPI();
	spi.settings(SPI::Prescaler::DIV8, SPI::BitOrder::MSB_FIRST, SPI::Mode::Mode0);
	
	// RFID
	MFRC522 mfrc522 = MFRC522();
	mfrc522.PCD_Init();
	MFRC522::MIFARE_Key key;
	uint8_t trailerBlock = 7;
	for (uint8_t i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}
	
	// Debug Vars
	uint8_t msg = SPIC_CTRL;
	uint8_t byte1 = 0;
		
    while (1) 
    {
		if (rotationSensor->getData() == 0b00000001 ){
			Gpio::SetPinValue(Pin::E5, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::E5, Value::Low);
		}
		
		if ( ! mfrc522.PICC_IsNewCardPresent())
		{
			Gpio::SetPinValue(Pin::E4, Value::High);
		}
		else {
			Gpio::SetPinValue(Pin::E4, Value::Low);
		}
		
		if ( ! mfrc522.PICC_ReadCardSerial())
		{
			//Gpio::SetPinValue(Pin::D6, Value::High);
		}
		else {
			//Gpio::SetPinValue(Pin::D6, Value::Low);
			//byte1 = mfrc522.uid.uidByte[0];
		}
		
		
		MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
		if ( status != MFRC522::STATUS_OK ){
			Gpio::SetPinValue(Pin::E1, Value::Low);
			//_delay_ms(5000);
		}
		else {
			Gpio::SetPinValue(Pin::E1, Value::High);
			stepper.step(512);
			_delay_ms(2000);
		}
    }
}

