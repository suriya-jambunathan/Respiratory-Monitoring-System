/**
 * EMBEDDED CHALLENGE FALL 2022: JUST BREATHE
 * 
 * Authors:
 * Suriya Prakash Jambunathan	sj3828
 * Ashwath Shankarnarayan  		as16494
 * Prashanth Rebala		   		pr2359
 * Akansha Dhote				avd8874 
 * 
*/


#include <mbed.h>
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include "drivers/LCD_DISCO_F429ZI.h"

const uint8_t SLAVE_ADDR = 0x18;
const uint8_t CMD_BYTE = 0xAA;
const uint8_t CMD_DATA_HIGH = 0x00;
const uint8_t CMD_DATA_LOW = 0x00;
 
int timeSinceLastBreath;
double previousPressureReading;
double pressureDifference;
char display_buf[3][60];

#define I2C_SDA PC_9
#define I2C_SCL PA_8
#define MAXIMUM_PSI 25
#define MINIMUM_PSI 0
#define OUTPUT_MAX 0xE66666
#define OUTPUT_MIN 0x19999A
#define CYCLE_FREQUENCY 100000
#define PRESSURE_THRESHOLD 0.02
#define COUNTER_THRESHOLD 10
#define KPA_FACTOR 6.89475728

#define READ_WAIT_DURATION 5
#define LOOP_WAIT_DURATION 995

#define HAPPY_BACKGROUND 0xFF044024
#define SAD_BACKGROUND 0xFF5E0409
#define BACKGROUND 1
#define FOREGROUND 0
#define MAX_OUTPUT_LENGTH 32
#define DISPLAY_TRANSPARENCY 0x7Fu

DigitalInOut scl(I2C_SCL);
DigitalInOut sda(I2C_SDA);
I2C i2c(I2C_SDA, I2C_SCL);

LCD_DISCO_F429ZI lcd;

// Setting up the LCD Display on STM32
void lcdDisplaySetup()
{
	// Setting up Background
	lcd.SelectLayer(BACKGROUND);
	lcd.Clear(HAPPY_BACKGROUND);
	lcd.SetLayerVisible(BACKGROUND, ENABLE);
	lcd.SetTransparency(BACKGROUND, DISPLAY_TRANSPARENCY);

	// Setting up Foreground
	lcd.SelectLayer(FOREGROUND);
	lcd.Clear(HAPPY_BACKGROUND);
	lcd.SetBackColor(HAPPY_BACKGROUND);
	lcd.SetTextColor(LCD_COLOR_WHITE);
}

// Setting up the I2C Communication Channel on STM32
void i2cSetup()
{
	scl.mode(PullUp);
	sda.mode(PullUp);
	i2c.frequency(CYCLE_FREQUENCY);
}

// Setting up the SEN-16476 Micro Pressure Sensor
void sen16476Setup()
{
	// Write the first I2C write command
	i2c.start();
	i2c.write(SLAVE_ADDR << 1); // shift the slave address left by 1 bit to set the write bit
	i2c.write(CMD_BYTE);
	i2c.write(CMD_DATA_HIGH);
	i2c.write(CMD_DATA_LOW);
	i2c.stop();
}

// Function to stall execution
void wait(int waitTime)
{
	HAL_Delay(waitTime);
}

// Function to convert Pressure to kilo Pascal
double pressureTransferFunction(double reading)
{
	double pressure;
	pressure = (reading - OUTPUT_MIN) * (MAXIMUM_PSI - MINIMUM_PSI);
	pressure = (pressure / (OUTPUT_MAX - OUTPUT_MIN)) + MINIMUM_PSI;
	return pressure * KPA_FACTOR;
}

// Function to get pressure reading from SEN-16476
double getPressureReading()
{
	// Write the second I2C write command
	i2c.start();
	i2c.write(SLAVE_ADDR << 1 | 1); // shift the slave address left by 1 bit and set the read bit
	uint8_t status = i2c.read(1);

	if (status == 0) {
		return 0;
	}

	uint8_t data_high = i2c.read(1);
	uint8_t data_mid = i2c.read(1);
	uint8_t data_low = i2c.read(0);
	i2c.stop();
	double reading = (data_high << 16) | (data_mid << 8) | (data_low);
	return pressureTransferFunction(reading);
}

// Function to display parameters on the STM32 LCD Display
void printToLCD(float pressure)
{
	// Initializing the Dynamic Background
	int DYNAMIC_BACKGROUND = timeSinceLastBreath >= COUNTER_THRESHOLD ? SAD_BACKGROUND : HAPPY_BACKGROUND;

	// Clearing the display on default condition
	if (timeSinceLastBreath == 0 || timeSinceLastBreath == COUNTER_THRESHOLD)
	{
		lcd.SelectLayer(BACKGROUND);
		lcd.Clear(DYNAMIC_BACKGROUND);
		lcd.SelectLayer(FOREGROUND);
		lcd.Clear(DYNAMIC_BACKGROUND);
		lcd.SetBackColor(DYNAMIC_BACKGROUND);
	}

	printf("Pressure: %.3f kPa\tTime elapsed since last breath: %02ds\n", pressure, timeSinceLastBreath);

	snprintf(display_buf[0], MAX_OUTPUT_LENGTH, "Pressure: %.3lf kPa", pressure);
	snprintf(display_buf[1], MAX_OUTPUT_LENGTH, "Time elapsed since");
	snprintf(display_buf[2], MAX_OUTPUT_LENGTH, "last breath: %02ds", timeSinceLastBreath);

	lcd.DisplayStringAt(0, LINE(0), (uint8_t *)"---------------------", CENTER_MODE);
	lcd.DisplayStringAt(0, LINE(2), (uint8_t *)display_buf[0], LEFT_MODE);
	lcd.DisplayStringAt(0, LINE(4), (uint8_t *)display_buf[1], LEFT_MODE);
	lcd.DisplayStringAt(0, LINE(5), (uint8_t *)display_buf[2], LEFT_MODE);
	lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"---------------------", CENTER_MODE);
	lcd.DisplayStringAt(0, LINE(13), (uint8_t *)":)", CENTER_MODE);

	// When the time since last breath exceeded a set threshold
	if (timeSinceLastBreath >= COUNTER_THRESHOLD)
	{
		lcd.DisplayStringAt(0, LINE(9), (uint8_t *)"NO BREATH DETECTED", CENTER_MODE);
		lcd.DisplayStringAt(0, LINE(11), (uint8_t *)"---------------------", LEFT_MODE);
		lcd.DisplayStringAt(0, LINE(13), (uint8_t *)":(", CENTER_MODE);
	}
}

// Function to compute pressure
double computePressure()
{
	// Get pressure data from the SEN-16476 Sensor
	double currentPressure = getPressureReading();

	// Calculating the pressure difference
	pressureDifference = abs(currentPressure - previousPressureReading);
	previousPressureReading = currentPressure;

	// Checking if the pressure difference is within a set threshold
	if (pressureDifference < PRESSURE_THRESHOLD)
	{
		timeSinceLastBreath++;
	}
	else
	{
		timeSinceLastBreath = 0;
	}

	return currentPressure;
}

int main()
{

	// initialize LCD display on STM-32
	lcdDisplaySetup();

	// setup i2c communication with the sensor
	i2cSetup();

	// initialize variables to 0
	timeSinceLastBreath = 0;
	pressureDifference = 0;
	previousPressureReading = 0;

	while (true)
	{
		// set up the Sen-16476 sensor
		sen16476Setup();

		// small delay to obtain pressure readings
		wait(READ_WAIT_DURATION);

		// compute pressure values
		double pressure = computePressure();

		// display readings to STM32 LCD
		printToLCD(pressure);

		// delay between next iteration
		wait(LOOP_WAIT_DURATION);
	}

	return 0;
}
