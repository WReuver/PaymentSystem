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
#include "Communication/USART.h"
#include "Master/RaspberryPi.h"
#include "State/Payment.h"

using namespace Hardware;
using namespace Gpio; 
using namespace Sensors;
using namespace Communication;
using namespace Master;
using namespace Payment;

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
const uint16_t motorActionDelay = 500; // Delay of the stepper motor in ms before each action is started

// IR sensor
Pin rotationSensorPins[1] = { Pin::C0 };
RotationSensor* rotationSensor = new RotationSensor(rotationSensorPins);

// SPI
SPI spi = SPI();

// RFID
MFRC522 mfrc522 = MFRC522();
MFRC522::MIFARE_Key key;
uint8_t trailerBlock = 7;

//Raspberry Pi
RaspberryPi* raspberryPi;                                                                                   // The Raspberry Pi object
Usart::RxTx raspberrySerialPort = Usart::RxTx::C2_C3;                                                       // The Raspberry Pi's serial port pins

// State
State state = Payment::State::AwaitingCoin;

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
	
	//
	raspberryPi = new RaspberryPi(raspberrySerialPort);
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

void test_reject(uint8_t n){
	for(uint8_t i = 0; i < n; i++)
	{
		rejectCoin();
		calibrateMotor();
	}
}

void test_acceptReject(uint8_t n){
	acceptCoin();
	calibrateMotor();
	rejectCoin();
	calibrateMotor();
}

// Unlock all functionalities
void executeAcceptCommand(uint8_t* response)
{
	acceptCoin();  
	calibrateMotor();                                                                                                  // Unlock the micro controller so it will be able to execute all commands again
	response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Accept);                    // Add the equivalent command response
	response[1] = 0x00;                                                                                                 // Add the amount of parameters
}

// Unlock all functionalities
void executeRejectCommand(uint8_t* response)
{
	rejectCoin();    
	calibrateMotor();                                                                                                // Unlock the micro controller so it will be able to execute all commands again
	response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Reject);                    // Add the equivalent command response
	response[1] = 0x00;                                                                                                 // Add the amount of parameters
}

// Unlock all functionalities
void executeDemoCommand(uint8_t* response)
{
	test_acceptReject(0);                                                                                                    // Unlock the micro controller so it will be able to execute all commands again
	response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Demo);                    // Add the equivalent command response
	response[1] = 0x00;                                                                                                 // Add the amount of parameters
}

// Unlock all functionalities
void executeCalibrateCommand(uint8_t* response)
{
	calibrateMotor();                                                                                                    // Unlock the micro controller so it will be able to execute all commands again
	response[0] = (uint8_t) raspberryPi->getEquivalentCommandResponse(RaspberryPi::Command::Calibrate);                    // Add the equivalent command response
	response[1] = 0x00;                                                                                                 // Add the amount of parameters
}

// Execute a command
void executecommand(uint8_t* response, uint8_t* receivedCommand)
{
	switch ((RaspberryPi::Command) receivedCommand[0])
	{
		case RaspberryPi::Command::Calibrate:           executeCalibrateCommand(response);                              break;      // Received a sense command
		case RaspberryPi::Command::Accept:				executeAcceptCommand(response);									break;
		case RaspberryPi::Command::Reject:				executeRejectCommand(response);									break;
		case RaspberryPi::Command::Demo:				executeDemoCommand(response);									break;      // Received a sense command
		default:                                                                                                        break;      // Impossible
	}
}

void runRoutine(void)
{
	SetPinValue(Pin::E1, Value::Low);
	SetPinValue(Pin::E4, Value::Low);
	SetPinValue(Pin::E5, Value::High);
	while (1)
	{
		uint8_t operationStatus = raspberryPi->waitForNextCommand();        // 0 = success, 1 = command does not exist, 2 = timeout
		uint8_t* receivedCommand = raspberryPi->getCommand();               // Get the location to the received command
		uint8_t response[6] = { 0 };                                        // The response will never be larger than six bytes
				
		switch (operationStatus)
		{
			case 0:     // Everything went fine, the command is recognized and there was no timeout
			SetPinValue(Pin::E5, Value::Low);
			executecommand(response, receivedCommand);
			break;
			
			
			case 1:     // Command does not exist
			//SetPinValue(Pin::E4, Value::High);
			response[0] = (uint8_t) RaspberryPi::ComException::Unknown;     // Add the "Command Unknown" Exception as command response
			response[1] = 0x00;                                             // Zero parameters
			break;
			
			
			case 2:     // Timeout
			SetPinValue(Pin::E1, Value::High);
			response[0] = (uint8_t) RaspberryPi::ComException::TimeOut;     // Add the "Serial timeout" Exception as command response
			response[1] = 0x00;                                             // Zero parameters
			break;
		}
		
		raspberryPi->returnResponse(response);                              // Return the response
	}
}

uint8_t* getCommandFormat(uint8_t* cmdFormat, RaspberryPi::Command cmd)
{
	switch(cmd)
	{
		case RaspberryPi::Command::Calibrate:
			cmdFormat[0] = (uint8_t) RaspberryPi::Command::Calibrate;
			cmdFormat[1] = 0x00;
			break;
		case RaspberryPi::Command::Accept:
			cmdFormat[0] = (uint8_t) RaspberryPi::Command::Accept;
			cmdFormat[1] = 0x00;
			break;
		case RaspberryPi::Command::Reject:
			cmdFormat[0] = (uint8_t) RaspberryPi::Command::Reject;
			cmdFormat[1] = 0x00;
			break;
		case RaspberryPi::Command::Demo:
			cmdFormat[0] = (uint8_t) RaspberryPi::Command::Demo;
			cmdFormat[1] = 0x00;
			break;
		default:	
			break;
	}
	
	return cmdFormat;
}

int main(void)
{
	setup_interrupts();
	initialize();	
	setup_RFID();
			
	uint8_t cmdFormat[6] = { 0 };
		
    while (1) 
    {
		raspberryPi->returnResponse(getCommandFormat(cmdFormat, RaspberryPi::Command::Demo));
		_delay_ms(2000);
    }
}

