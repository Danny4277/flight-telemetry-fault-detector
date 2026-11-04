# C++ Flight Telemetry Fault Detection System

A C++ application for processing simulated aircraft telemetry
and identifying abnormal sensor readings.

## Project Goals

- Process simulated aircraft sensor measurements
- Detect abnormal telemetry conditions
- Practice C++ systems programming
- Implement input validation and automated testing

## Technologies

- C++17
- Google Colab
- Git and GitHub

## Current Progress

- [x] Milestone 1: Define and display telemetry readings
- [ ] Milestone 2: Read telemetry data from CSV
- [ ] Milestone 3: Implement fault detection
- [ ] Milestone 4: Add automated testing
- [ ] Milestone 5: Final documentation

## Build and Run

```bash
g++ -std=c++17 -Wall -Wextra src/main.cpp -o telemetry
./telemetry


## Dataset

This project uses the NASA C-MAPSS FD001 turbofan engine
degradation simulation dataset.

The training dataset contains:
- 100 simulated engines
- 20,631 telemetry records
- 3 operating settings
- 21 sensor measurements per record

Source:
https://www.nasa.gov/intelligent-systems-division/discovery-and-systems-health/pcoe/pcoe-data-set-repository/

Download and extract the dataset into:

data/cmapss/train_FD001.txt

The raw dataset is excluded from Git tracking.

## Current Implementation

- Reads whitespace-separated NASA telemetry records
- Parses engine IDs, operating cycles, and sensor measurements
- Stores records in C++ vectors
- Counts malformed records
