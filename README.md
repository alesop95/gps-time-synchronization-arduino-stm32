GPS-based Time Synchronization on Arduino and STM32 Nucleo

This project contains the complete experimental work developed for a Bachelor’s thesis in Electronic / Computer Engineering. The purpose of the work is the experimental evaluation and comparison of clock drift on two embedded platforms, using GPS time as an absolute reference.

The analyzed platforms are an Arduino board, relying on its internal software-based timing mechanism (millis()), and an STM32 Nucleo board, using the hardware Real-Time Clock (RTC) driven by an external low-speed crystal oscillator (LSE at 32.768 kHz). A GPS module providing NMEA sentences is used as an external and absolute time reference for both systems.

The main objective of the project is to acquire GPS time, extract and process time information from NMEA sentences, compare GPS time with the local clocks of the two platforms, and quantify clock drift over long acquisition intervals on the order of several hours.

The hardware setup consists of an Arduino Uno board based on the ATmega328P microcontroller, an STM32 Nucleo board belonging to the STM32L1xx family, a GPS module with NMEA output operating at 9600 baud, and a USB serial connection to a PC for data acquisition and logging.

The experimental methodology is the following. GPS NMEA strings are received via UART. The $GPGGA sentence is parsed in order to extract the time information. On the Arduino platform, the firmware prints the GPS time together with the value returned by millis() over the serial interface. A Python script running on the PC acquires the serial data and stores it into CSV files for offline analysis. On the STM32 Nucleo platform, the RTC peripheral is initialized and driven by the external LSE oscillator. GPS time and RTC time are periodically compared, and the resulting time difference is transmitted over serial. Clock drift is then computed and plotted from the acquired data.

On the Arduino side, the firmware interfaces with the GPS module using a software-based serial interface. The GPS module communicates at 9600 bps. The GPS transmit pin is connected to Arduino pin 9, while the GPS receive pin is connected to Arduino pin 8. The serial output produced by the firmware is formatted as:

<GPS_time>;<arduino_millis>


An example of output is:

235317.000;3004


This output is consumed by a Python script running on the PC. The Python script is written for Python 2.7 and uses the pySerial library. It opens a serial port (default COM3, configurable in the code) at 9600 bps, continuously reads incoming data, and stores it into a CSV file named output.csv. The file contains two columns: GPS time expressed in milliseconds and Arduino time expressed in milliseconds. The output file is overwritten at each acquisition session.

On the STM32 Nucleo side, the firmware is developed using Contiki OS and the STM32 HAL. The RTC is configured to use the external LSE oscillator with a nominal frequency of 32.768 kHz. The asynchronous prescaler is set to 127 and the synchronous prescaler is set to 255, yielding a 1 Hz RTC time base. The LSE oscillator is selected because it is suitable for calendar and long-term timing applications. The firmware receives GPS NMEA data via UART, initializes and synchronizes the RTC, and periodically compares RTC time with GPS time. The system acquires GPS data every 30 seconds, while the RTC runs freely after the initial synchronization. The drift is computed as the difference between RTC time and GPS time. The serial output is formatted as comma-separated values:

<RTC-GPS difference>,<GPS timestamp>


All experimental data are stored as raw CSV files and used for offline analysis and plotting. The generated plots show the drift of the Arduino internal clock with respect to GPS time and the drift of the STM32 RTC with respect to GPS time.

Over an acquisition interval of approximately four hours, the Arduino internal clock exhibits a drift of about 5.0 seconds, while the STM32 RTC exhibits a drift of about 1.6 seconds. The STM32 RTC therefore shows approximately three times better stability compared to the Arduino internal timing mechanism.