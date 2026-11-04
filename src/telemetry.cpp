#include "telemetry.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

LoadResult loadTelemetry(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    LoadResult result;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string firstToken;

        if (!(ss >> firstToken)) {
            continue;
        }

        TelemetryReading reading{};
        std::stringstream record(line);
        bool valid = true;

        if (!(record >> reading.engineID >> reading.cycle)) {
            valid = false;
        }

        for (int i = 0; i < SETTING_COUNT && valid; ++i) {
            if (!(record >> reading.operatingSettings[i])) {
                valid = false;
            }
        }

        for (int i = 0; i < SENSOR_COUNT && valid; ++i) {
            if (!(record >> reading.sensors[i])) {
                valid = false;
            }
        }

        std::string extra;
        if (valid && (record >> extra)) {
            valid = false;
        }

        if (valid && (reading.engineID <= 0 || reading.cycle <= 0)) {
            valid = false;
        }

        if (valid) {
            result.readings.push_back(reading);
        } else {
            ++result.invalidRecords;
        }
    }

    return result;
}

std::unordered_map<int, std::vector<TelemetryReading>>
groupByEngine(const std::vector<TelemetryReading>& readings) {
    std::unordered_map<int, std::vector<TelemetryReading>> engines;

    for (const auto& reading : readings) {
        engines[reading.engineID].push_back(reading);
    }

    return engines;
}

BaselineStats calculateBaseline(
    const std::vector<TelemetryReading>& readings,
    int baselineCycles
) {
    if (baselineCycles <= 0) {
        throw std::invalid_argument("baselineCycles must be positive");
    }

    BaselineStats stats;
    std::array<double, SENSOR_COUNT> sums{};

    for (const auto& reading : readings) {
        if (reading.cycle <= baselineCycles) {
            ++stats.count;

            for (int i = 0; i < SENSOR_COUNT; ++i) {
                sums[i] += reading.sensors[i];
            }
        }
    }

    if (stats.count == 0) {
        return stats;
    }

    for (int i = 0; i < SENSOR_COUNT; ++i) {
        stats.means[i] = sums[i] / stats.count;
    }

    std::array<double, SENSOR_COUNT> squaredSums{};

    for (const auto& reading : readings) {
        if (reading.cycle <= baselineCycles) {
            for (int i = 0; i < SENSOR_COUNT; ++i) {
                double diff = reading.sensors[i] - stats.means[i];
                squaredSums[i] += diff * diff;
            }
        }
    }

    for (int i = 0; i < SENSOR_COUNT; ++i) {
        stats.stdDevs[i] =
            std::sqrt(squaredSums[i] / stats.count);
    }

    return stats;
}

std::vector<Anomaly> detectAnomalies(
    const std::vector<TelemetryReading>& readings,
    const BaselineStats& baseline,
    int baselineCycles,
    double zThreshold
) {
    if (baselineCycles <= 0 || zThreshold <= 0.0) {
        throw std::invalid_argument("Invalid detection parameters");
    }

    std::vector<Anomaly> anomalies;
    constexpr double epsilon = 1e-10;

    if (baseline.count == 0) {
        return anomalies;
    }

    for (const auto& reading : readings) {
        if (reading.cycle <= baselineCycles) {
            continue;
        }

        for (int i = 0; i < SENSOR_COUNT; ++i) {
            if (baseline.stdDevs[i] < epsilon) {
                continue;
            }

            double zScore =
                (reading.sensors[i] - baseline.means[i])
                / baseline.stdDevs[i];

            if (std::abs(zScore) > zThreshold) {
                anomalies.push_back({
                    reading.engineID,
                    reading.cycle,
                    i + 1,
                    reading.sensors[i],
                    zScore
                });
            }
        }
    }

    return anomalies;
}

EngineResult analyzeEngine(
    int engineID,
    const std::vector<TelemetryReading>& readings,
    int baselineCycles,
    double zThreshold
) {
    BaselineStats baseline =
        calculateBaseline(readings, baselineCycles);

    EngineResult result{
        engineID,
        readings.size(),
        baseline.count,
        {}
    };

    if (baseline.count > 0) {
        result.anomalies = detectAnomalies(
            readings, baseline, baselineCycles, zThreshold
        );
    }

    return result;
}
