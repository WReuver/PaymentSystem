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

//#define F_CPU 32000000UL

// Stepper Motor
bool calibrate;
extern bool stopMotor;
Pin motorPin1 = Pin::D2;
Pin motorPin2 = Pin::D1;
Pin motorPin3 = Pin::D3;
Pin motorPin4 = Pin::D0;
Stepper stepper = Stepper(512, motorPin1, motorPin2, motorPin3, motorPin4);	// First argument determines the number of turns per revolution
const int16_t dispenseAngle = 136;	// Angle to turn the motor to dispense a coin inward or outward, expressed in steps. (136/512)*360 = ~96 degrees
const uint16_t motorActionDelay = 100; // Delay of the stepper motor in ms before each action is started

// IR sensor
Pin rotationSensorPins[1] = { Pin::C0 };
RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);

// SPI
SPI spi = SPI();

// RFID
MFRC522 mfrc522 = MFRC522();
MFRC522::MIFARE_Key key;
uint8_t trailerBlock = 7;

ISR(PORTB_INT0_vect)
{
	if(calibrate){
		SetPinValue(Pin::E5, Value::High);
		stopMotor = true;
		calibrate = false;
	} 
	else {
		SetPinValue(Pin::E5, Value::Low);
	}
}

void calibrateMotor()
{
	_delay_ms(motorActionDelay);
	calibrate = true;
	stepper.step(5120);
}

void acceptCoin()
{
	_delay_ms(motorActionDelay);
	stepper.step(dispenseAngle);
}

void rejectCoin()
{
	_delay_ms(motorActionDelay);
	stepper.step(-dispenseAngle);
}

void initialize(void)
{
	// Set the system clock to 32 MHz
	SystemClock::SetClockSource(SystemClock::Source::RC32MHz);
	
	// Set pinouts and enable logic level converter
	SetPinDirection(Pin::B0, Dir::Input);		// IR SENSE 1  IN
	SetPinDirection(Pin::E1, Dir::Output);		// DEBUG LED 1
	SetPinDirection(Pin::E4, Dir::Output);		// DEBUG LED 2
	SetPinDirection(Pin::E5, Dir::Output);		// DEBUG LED 3
	SetPinDirection(Pin::D4, Dir::Output);		// LLC Output Enable Pin
	SetPinValue(Pin::D4, Value::High);			// Enable logic level converter
	
	// Set up rising edge interrupt on pin B0 
	PORTB.PIN0CTRL = PORT_ISC_RISING_gc;	// IR Sensor pin as interrupt; used with stepper calibration
	PORTB_INTCTRL = (PORT_INT0LVL_HI_gc);	// 
	PORTB_INT0MASK = PIN0_bm;				//
	
	// Enable interrupts
	sei();
	_delay_ms(100);	
	
	// Calibrate the stepper motor to its 'zero' position
	calibrateMotor();
}

void setup_interrupts()
{
	cli();
	CPU_CCP = CCP_IOREG_gc;
	PMIC_CTRL = (PMIC_HILVLEN_bm);
	PMIC_INTPRI = 0x00;
}

void setup_RFID()
{
	// Set SPI settings for use with the MFRC522
	spi.settings(SPI::Prescaler::DIV8, SPI::BitOrder::MSB_FIRST, SPI::Mode::Mode0);
	// Initialize the MFRC522
	mfrc522.PCD_Init();
	// Clear the array containing the authentication key
	for (uint8_t i = 0; i < 6; i++) {
		key.keyByte[i] = 0xFF;
	}
}

int main(void)
{
	setup_interrupts();
	initialize();	
	setup_RFID();
			
    while (1) 
    {
		if (rotationSensor->getData() == 0b00000001 )
		{
			;
		}
		else 
		{
			;
		}
		
		if ( ! mfrc522.PICC_IsNewCardPresent());
		
		if ( ! mfrc522.PICC_ReadCardSerial());
		
		
		MFRC522::StatusCode status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
		if ( status == MFRC522::STATUS_OK )
		{
			// Display the succes status by turning on an LED
			Gpio::SetPinValue(Pin::E4, Value::High);
			_delay_ms(1000);
			acceptCoin();
			_delay_ms(1000);
			calibrateMotor();
			_delay_ms(1000);
			rejectCoin();
			_delay_ms(1000);
			calibrateMotor();
		}
		else 
		{
			// Keep or turn the LED off in case of authentication failure
			Gpio::SetPinValue(Pin::E4, Value::Low);
		}
    }
}

