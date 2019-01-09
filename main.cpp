/*
 * StepperTest.cpp
 *
 * Created: 22/11/2018 12:09:03
 * Author : Wesley Reuver
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
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

bool calibrate;
extern bool stopMotor;

Pin motorPin1 = Pin::D2;
Pin motorPin2 = Pin::D1;
Pin motorPin3 = Pin::D3;
Pin motorPin4 = Pin::D0;

ISR(PORTB_INT0_vect){
	if(calibrate){
		SetPinValue(Pin::E5, Value::High);
		stopMotor = true;
		calibrate = false;
	} 
	else {
		SetPinValue(Pin::E5, Value::Low);
		stopMotor = true;
		calibrate = true;
	}
}

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
	
	PORTB.PIN0CTRL = PORT_ISC_RISING_gc;	// IR Sensor pin as interrupt; used with stepper calibration
	PORTB_INTCTRL = (PORT_INT0LVL_HI_gc);	// 
	PORTB_INT0MASK = PIN0_bm;				//
	
	calibrate = true;
	sei();
}

void setup_interrupts(){
	cli();
	CPU_CCP = CCP_IOREG_gc;
	PMIC_CTRL = (PMIC_HILVLEN_bm);// | PMIC_HILVLEN_bm);
	PMIC_INTPRI = 0x00;
}

int main(void)
{
	setup_interrupts();
	initialize();
	// System Clock
	
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
	
	// Stepper Motor
	Stepper stepper = Stepper(512, Pin::D2, Pin::D1, Pin::D3, Pin::D0);
	SetPinValue(Pin::D4, Value::High);
	//Gpio::SetPinValue(Pin::E4, Value::High);
	stepper.step(136);
	
	// IR sensor
	Pin rotationSensorPins[1] = { Pin::C0 };
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
			;
		}
		else {
			;
		}
		
		if ( ! mfrc522.PICC_IsNewCardPresent());
		
		if ( ! mfrc522.PICC_ReadCardSerial());
		
		
		MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
		if ( status != MFRC522::STATUS_OK ){
			Gpio::SetPinValue(Pin::E4, Value::Low);
			//_delay_ms(5000);
		}
		else {
			Gpio::SetPinValue(Pin::E4, Value::High);
			stepper.step(5120);
			_delay_ms(2000);
		}
    }
}

