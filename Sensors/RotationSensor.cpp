/*
* RotationSensor.cpp
*
* Created: 19-Mar-18 14:38:33
* Author: Robin C. Pel, Wesley Reuver
*/

#include "RotationSensor.h"

Sensors::RotationSensor::RotationSensor(Gpio::Pin* sensorPins)
{
	/*for (int i = 0; i < 1; i++)
	{
		pins[i] = sensorPins[i];
		Gpio::SetPinDirection(pins[i], Gpio::Dir::Input);
		Gpio::SetPinMode(pins[i], Gpio::Mode::PullDown);
	}*/
	;
}

uint8_t Sensors::RotationSensor::getData()
{
	uint8_t measurement = 0b00000000;
	
	if (Gpio::GetPinValue(Gpio::Pin::C0) == Gpio::Value::High)
	{
		//SetPinValue(Gpio::Pin::E1, Gpio::Value::Low);
		measurement = 0x00;
	}
	else
	{
		//SetPinValue(Gpio::Pin::E1, Gpio::Value::High);
		measurement = 0x01;
	}
	
	return measurement;
}
