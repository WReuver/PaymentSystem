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

Pin motorPin1 = Pin::D5;	// Eventual: D5		A5
Pin motorPin2 = Pin::D0;	//			 D0		A3	
Pin motorPin3 = Pin::D6;	//			 D6		A6
Pin motorPin4 = Pin::D1;	//			 D1		A4
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
	SetPinDirection(Pin::B0, Dir::Output);
	SetPinDirection(Pin::A3, Dir::Output);
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
	Stepper stepper = Stepper(512, Pin::D5, Pin::D0, Pin::D6, Pin::D1);
	SetPinValue(Pin::B0, Value::High);
	stepper.step(512);
	Pin rotationSensorPins[1] = { Pin::B1 };
	RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);
	
    while (1) 
    {
		if (rotationSensor->getData() == 0b00000001 ){
		}
		else {
			;
		}
		
		
    }
}

