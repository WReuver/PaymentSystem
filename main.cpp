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

using namespace Hardware;
using namespace Gpio; 
using namespace Sensors;

#define F_CPU 32000000UL

Pin motorPin1 = Pin::A5;	// Eventual: D5
Pin motorPin2 = Pin::A3;	//			 D0
Pin motorPin3 = Pin::A6;	//			 D6
Pin motorPin4 = Pin::A4;	//			 D1
const uint16_t delayUS = 2500;
const uint16_t delayMS = 100;

void initialize(void)
{
	SetPinDirection(motorPin1, Dir::Output);
	SetPinDirection(motorPin2, Dir::Output);
	SetPinDirection(motorPin3, Dir::Output);
	SetPinDirection(motorPin4, Dir::Output);
	
	SetPinDirection(Pin::B1, Dir::Input);
	SetPinMode(Pin::B1, Mode::PullDown);
	SetPinDirection(Pin::B2, Dir::Output);
	SetPinDirection(Pin::B3, Dir::Output);
}

void step(uint8_t thisStep)
{
	switch (thisStep) {
		case 0:  // 1010
		SetPinValue(motorPin1, Value::High);
		SetPinValue(motorPin2, Value::Low);
		SetPinValue(motorPin3, Value::High);
		SetPinValue(motorPin4, Value::Low);
		break;
		case 1:  // 0110
		SetPinValue(motorPin1, Value::Low);
		SetPinValue(motorPin2, Value::High);
		SetPinValue(motorPin3, Value::High);
		SetPinValue(motorPin4, Value::Low);
		break;
		case 2:  //0101
		SetPinValue(motorPin1, Value::Low);
		SetPinValue(motorPin2, Value::High);
		SetPinValue(motorPin3, Value::Low);
		SetPinValue(motorPin4, Value::High);
		break;
		case 3:  //1001
		SetPinValue(motorPin1, Value::High);
		SetPinValue(motorPin2, Value::Low);
		SetPinValue(motorPin3, Value::Low);
		SetPinValue(motorPin4, Value::High);
		break;
	}
}

void motor(void)
{
	for(uint8_t i = 0; i < 4; i++){
		step(i);
		_delay_us(delayUS);
		//_delay_ms(delayMS);
	}
	return;
}

int main(void)
{
	initialize();
	//Stepper stepper = Stepper(512, Pin::A5, Pin::A3, Pin::A6, Pin::A4);
	//stepper.step(126);
	Pin rotationSensorPins[1] = { Pin::B1 };
	RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);
	
	//SetPinValue(Pin::B2, Value::Low);
	SetPinValue(Pin::B3, Value::High);
	
    while (1) 
    {
		if (rotationSensor->getData() == 0b00000001 ){
			//SetPinValue(Pin::B3, Value::High);	
		}
		else {
			// SetPinValue(Pin::B3, Value::Low);
		}
		//SetPinValue(Pin::B3, GetPinValue(Pin::B1));
    }
}

