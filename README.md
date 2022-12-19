# Respiratory-Monitoring-System

## Just Breathe - Embedded Challenge Fall 2022

Authors:
 - Suriya Prakash Jambunathan (sj3828)
 - Ashwath Shankarnarayan (as16494)
 - Prashanth Rebala (pr2359)
 - Akansha Dhote (avd8874)


This code is for an embedded system that monitors a person's breathing patterns using a pressure sensor and displays the results on an LCD display. The system is designed to detect if a person has not taken a breath in a certain amount of time and trigger an alert if necessary.<br>

#### Hardware and Software Requirements

 - STM32F429ZI microcontroller
 - SEN-16476 micro pressure sensor
 - LCD display
 - mbed library
 - Setup

Connect the SEN-16476 micro pressure sensor to the STM32F429ZI microcontroller using I2C communication. <br>
Connect the LCD display to the STM32F429ZI microcontroller. <br> 
Upload the code to the STM32F429ZI microcontroller using the mbed library. <br>

#### Usage

The system will automatically begin monitoring the person's breathing patterns and display the results on the LCD display.<br>
If the system detects that the person has not taken a breath in a certain amount of time, an alert will be triggered.<br>
The LCD display will show the current pressure reading in kilopascals and the time since the last breath was taken.<br>
The background color of the LCD display will change to reflect the person's breathing patterns, with a happy background indicating normal breathing and a sad background indicating a lack of breathing.<br>

#### Customization

The following constants can be adjusted to change the behavior of the system:

 - MAXIMUM_PSI: The maximum pressure reading in pounds per square inch.
 - MINIMUM_PSI: The minimum pressure reading in pounds per square inch.
 - OUTPUT_MAX: The maximum output from the pressure sensor.
 - OUTPUT_MIN: The minimum output from the pressure sensor.
 - CYCLE_FREQUENCY: The frequency of the I2C communication cycle.
 - PRESSURE_THRESHOLD: The minimum difference in pressure readings to trigger an alert.
 - COUNTER_THRESHOLD: The amount of time a person can go without taking a breath before an alert is triggered.
 - KPA_FACTOR: The conversion factor from pounds per square inch to kilopascals.
 - READ_WAIT_DURATION: The amount of time to wait before taking another pressure reading.
 - LOOP_WAIT_DURATION: The amount of time to wait before starting the next loop.
 - HAPPY_BACKGROUND: The background color of the LCD display when the person is breathing normally.
 - SAD_BACKGROUND: The background color of the LCD display when the person has not taken a breath in a certain amount of time.
 - DISPLAY_TRANSPARENCY: The transparency of the LCD display.

#### Functions


```lcdDisplaySetup()```<br>
This function sets up the LCD display by configuring the background and foreground layers and setting the transparency and text color.

```i2cSetup()```<br>
This function sets up the I2C communication channel by configuring the SDA and SCL pins and setting the communication frequency.

```sen16476Setup()```<br>
This function sets up the SEN-16476 micro pressure sensor by sending an I2C write command to the sensor.

```wait(int waitTime)```<br>
This function stalls the execution of the code for a specified amount of time.

```pressureTransferFunction(double reading)```<br>
This function converts a pressure reading from the sensor to kilopascals.

```readPressure()```<br>
This function reads the current pressure from the sensor and converts it to kilopascals using the pressureTransferFunction() function.

```updateDisplay()```<br>
This function updates the LCD display with the current pressure reading and time since the last breath. It also changes the background color of the display based on the person's breathing patterns.

```checkBreath()```<br>
This function checks if the person has taken a breath within a certain amount of time and triggers an alert if necessary.


#### Main Loop

The main loop of the code continuously reads the pressure from the sensor, updates the LCD display, and checks the person's breathing patterns using the readPressure(), updateDisplay(), and checkBreath() functions. It then waits for a specified amount of time before starting the next loop.
