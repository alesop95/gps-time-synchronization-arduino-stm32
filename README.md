# Root README.md

## Project Title

GPS-based Time Synchronization on Arduino and STM32 Nucleo

## Context

This repository contains the complete experimental work developed for a Bachelor's thesis in Electronic / Computer Engineering. The objective of the work is the experimental evaluation and comparison of the clock drift of two different embedded platforms using GPS time as absolute reference.

The analyzed platforms are:

* Arduino board using the internal timing mechanism based on `millis()`
* STM32 Nucleo board using the hardware Real-Time Clock (RTC) with external LSE oscillator (32.768 kHz)

The GPS module is used as an external, absolute time reference.

## Objective

The goal of the project is to:

* Acquire GPS time using NMEA sentences
* Extract and process time information
* Compare GPS time with local clocks
* Quantify clock drift over long acquisition intervals (hours)

## Hardware Used

* Arduino board (ATmega328-based)
* STM32 Nucleo board (STM32L1xx family)
* GPS module (NMEA output, 9600 baud)
* USB serial connection to PC

## Methodology

1. GPS NMEA strings are received via UART.
2. The `$GPGGA` sentence is parsed to extract time information.
3. Arduino prints GPS time and `millis()` value over serial.
4. A Python script acquires serial data and stores it into CSV files.
5. STM32 Nucleo uses the RTC peripheral driven by the LSE oscillator.
6. GPS time and RTC time are periodically compared.
7. Clock drift is computed and plotted.

## Repository Structure

* `arduino/` Arduino source code
* `python/` Python acquisition script
* `stm32_nucleo/` STM32 + Contiki OS code
* `data/` Raw CSV data
* `plots/` Generated plots
* `thesis/` Thesis document

## Results Summary

Over an acquisition interval of approximately 4 hours:

* Arduino clock drift: ~5.0 seconds
* STM32 RTC drift: ~1.6 seconds

The STM32 RTC shows approximately 3x better stability compared to Arduino internal timing.

## License

MIT License

---

# arduino/README.md

## Description

This folder contains the Arduino firmware used to interface with the GPS module and extract the GPS time information.

## Board

* Arduino Uno (ATmega328P)

## GPS Interface

* Communication via SoftwareSerial
* Baud rate: 9600 bps

### Pin Mapping

* GPS TX → Arduino pin 9
* GPS RX → Arduino pin 8

## Output Format

The firmware outputs serial lines formatted as:

```
<GPS_time>;<arduino_millis>
```

Example:

```
235317.000;3004
```

## Purpose

The output is consumed by a Python script for CSV logging and post-processing.

---

# python/README.md

## Description

This folder contains the Python script used to read serial data from the Arduino board and store it into a CSV file.

## Requirements

* Python 2.7
* pySerial library

## Serial Configuration

* Port: COM3 (modifiable in code)
* Baud rate: 9600 bps

## Output

* File name: `output.csv`
* Format: CSV (comma-separated)
* Two columns:

  * GPS time in milliseconds
  * Arduino time in milliseconds

The file is overwritten at each acquisition.

---

# stm32_nucleo/README.md

## Description

This directory contains the firmware developed for the STM32 Nucleo board using Contiki OS.

## Hardware

* STM32 Nucleo (STM32L1xx)
* External 32.768 kHz crystal (LSE)

## Clock Configuration

* RTC clock source: LSE
* Frequency: 32.768 kHz
* Asynchronous prescaler: 127
* Synchronous prescaler: 255

The LSE oscillator is selected due to its suitability for calendar and long-term timing applications.

## Operating System

* Contiki OS

## Functionality

* GPS NMEA reception via UART
* RTC initialization and synchronization
* Periodic comparison between GPS time and RTC time
* Output of time difference via serial

---

# stm32_nucleo/contiki/README.md

## Description

This folder contains the Contiki process and HAL-based code used to:

* Configure the RTC
* Receive GPS data
* Compute time differences

## Timing Strategy

* GPS data acquisition every 30 seconds
* RTC runs freely after initial synchronization
* Drift computed as difference between GPS and RTC time

## Output Format

Serial output consists of comma-separated values:

```
<RTC-GPS difference>,<GPS timestamp>
```

---

# data/README.md

## Description

This directory contains raw CSV data acquired during the experiments.

### Subfolders

* `arduino/` Data acquired from Arduino experiments
* `nucleo/` Data acquired from STM32 Nucleo experiments

The files are used for offline analysis and plotting.

---

# plots/README.md

## Description

This directory contains plots generated from the experimental data.

## Plots

* `arduino_drift.png`: Drift of Arduino internal clock vs GPS
* `nucleo_rtc_drift.png`: Drift of STM32 RTC vs GPS

All plots are generated from CSV data stored in the `data/` directory.

---

# thesis/README.md

## Description

This directory contains the Bachelor's thesis document associated with this project.

## Contents

* `thesis.pdf`: Final thesis document
* `thesis_sources.tex`: LaTeX source files (if available)

The thesis provides the complete theoretical background, experimental setup, and analysis of results.
