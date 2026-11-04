# Flight Telemetry Fault Detector

A C++17 statistical anomaly detection system for simulated aircraft turbofan engine telemetry, using NASA's C-MAPSS FD001 dataset.

## Overview

This project analyzes multivariate sensor data to identify readings that deviate significantly from an engine's initial operating baseline.

The system processes telemetry from 100 simulated turbofan engines, computes per-engine sensor statistics, and flags unusual readings using Z-scores.

**Technologies:** C++17, STL, file I/O, statistical analysis, automated testing, Git, GitHub, Google Colab.

## Dataset

NASA C-MAPSS (Commercial Modular Aero-Propulsion System Simulation), FD001 training dataset.

- 100 simulated engines
- 20,631 telemetry records
- 21 sensors per record
- 3 operating settings
- Engine identifiers and operating cycle numbers
- Simulated degradation trajectories

Dataset reference:
https://data.nasa.gov/dataset/cmapss-jet-engine-simulated-data

## How It Works

1. **Parse telemetry:** Read whitespace-separated engine records and validate their structure.
2. **Group by engine:** Organize telemetry records using an unordered map keyed by engine ID.
3. **Calculate baseline:** Use cycles 1-30 to calculate each sensor's mean and population standard deviation independently for each engine.
4. **Detect anomalies:** Evaluate later sensor readings using their baseline Z-scores.
5. **Generate results:** Report individual anomalies, per-engine counts, and overall statistics.

The anomaly detection formula is:

    z = (sensor_value - baseline_mean) / baseline_stddev

A sensor reading is flagged when:

    abs(z) > 3.0

Sensors with near-zero baseline standard deviation are skipped to avoid unstable division.

## Project Structure

```text
.
├── src/
│   ├── main.cpp
│   ├── telemetry.h
│   └── telemetry.cpp
├── tests/
│   └── test_telemetry.cpp
├── data/
│   └── cmapss/
│       └── train_FD001.txt
├── .gitignore
└── README.md
```

## Build and Run

From the repository root:

```bash
g++ -std=c++17 -O2 -Wall -Wextra \
    src/main.cpp src/telemetry.cpp -o telemetry

./telemetry > detection_results.txt
```

The dataset must be available at:

    data/cmapss/train_FD001.txt

## Run Unit Tests

```bash
g++ -std=c++17 -O2 -Wall -Wextra \
    tests/test_telemetry.cpp src/telemetry.cpp -o test_telemetry

./test_telemetry
```

The seven tests cover:

- Engine grouping
- Baseline mean and standard deviation
- Anomaly detection
- Constant-sensor handling
- Missing baseline records
- Invalid baseline cycle count
- Invalid anomaly threshold

## Results

Results from the pre-refactoring implementation:

| Metric | Value |
|---|---:|
| Telemetry records | 20,631 |
| Engines analyzed | 100 |
| Engines skipped | 0 |
| Sensors per engine | 21 |
| Baseline cycles | 30 |
| Z-score threshold | 3.0 |
| Engine 1 anomalies | 502 |
| Total sensor-reading anomalies | 54,481 |

These values serve as regression targets for the modularized implementation and should be confirmed by rerunning the program.

## Engineering Features

- Modular C++17 design
- Reusable data parsing and statistical analysis functions
- STL vectors, arrays, and unordered maps
- Input validation and exception handling
- Automated unit tests using assertions
- Deterministic engine reporting order
- Per-engine and overall anomaly summaries

## Limitations

- A statistical anomaly does not necessarily represent a confirmed mechanical fault.
- The detector uses a fixed initial baseline and does not adapt to changing operating conditions.
- Constant or near-constant baseline sensors are skipped.
- The current method does not use labeled failure events to measure precision or recall.
- Anomalies are counted per sensor reading, not per unique cycle or engine.
- The fixed threshold and baseline length have not been optimized.

## Future Improvements

- Operating-condition-aware anomaly detection
- Configurable thresholds and baseline windows
- CSV and JSON output
- Automated CI testing with GitHub Actions
- Comparison against predictive maintenance models

## Project Purpose

Built to practice C++ software engineering, statistical anomaly detection, telemetry processing, and testing using a simulated aerospace dataset.
