#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <unordered_map>
#include <algorithm>

struct TelemetryReading {
    int engineID;
    int cycle;
    std::array<double, 3> operatingSettings;
    std::array<double, 21> sensors;
};

int main() {
    const std::string filepath = "data/cmapss/train_FD001.txt";
    const int baselineCycles = 30;
    const double zThreshold = 3.0;
    const double epsilon = 1e-10;

    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file "
                  << filepath << std::endl;
        return 1;
    }

    std::vector<TelemetryReading> dataset;
    int invalidRecords = 0;
    std::string line;

    // Load and validate telemetry data
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        TelemetryReading reading{};
        bool parseSuccess = true;

        if (!(ss >> reading.engineID >> reading.cycle)) {
            parseSuccess = false;
        }

        for (int i = 0; i < 3 && parseSuccess; ++i) {
            if (!(ss >> reading.operatingSettings[i])) {
                parseSuccess = false;
            }
        }

        for (int i = 0; i < 21 && parseSuccess; ++i) {
            if (!(ss >> reading.sensors[i])) {
                parseSuccess = false;
            }
        }

        std::string extra;
        if (parseSuccess && (ss >> extra)) {
            parseSuccess = false;
        }

        if (parseSuccess && reading.engineID > 0
                         && reading.cycle > 0) {
            dataset.push_back(reading);
        } else {
            ++invalidRecords;
        }
    }

    file.close();

    std::cout << "Successfully loaded "
              << dataset.size()
              << " telemetry records.\n";
    std::cout << "Invalid records: "
              << invalidRecords << "\n";

    if (dataset.empty()) {
        std::cerr << "Error: No valid telemetry data.\n";
        return 1;
    }

    // Group readings by engine ID
    std::unordered_map<int, std::vector<TelemetryReading>> engines;

    for (const auto& reading : dataset) {
        engines[reading.engineID].push_back(reading);
    }

    std::cout << "Total engines: "
              << engines.size() << "\n";

    // Sort engine IDs for predictable output
    std::vector<int> engineIDs;

    for (const auto& entry : engines) {
        engineIDs.push_back(entry.first);
    }

    std::sort(engineIDs.begin(), engineIDs.end());

    int totalAnomalies = 0;
    int processedEngines = 0;
    int skippedEngines = 0;

    // Analyze each engine independently
    for (int engineID : engineIDs) {
        const auto& engineReadings = engines.at(engineID);

        std::array<double, 21> sensorSums{};
        int baselineCount = 0;

        // Step 1: Calculate baseline sums
        for (const auto& reading : engineReadings) {
            if (reading.cycle <= baselineCycles) {
                for (int i = 0; i < 21; ++i) {
                    sensorSums[i] += reading.sensors[i];
                }
                ++baselineCount;
            }
        }

        if (baselineCount == 0) {
            std::cerr << "Warning: No baseline records for Engine "
                      << engineID << "\n";
            ++skippedEngines;
            continue;
        }

        // Step 2: Calculate baseline means
        std::array<double, 21> sensorMeans{};

        for (int i = 0; i < 21; ++i) {
            sensorMeans[i] = sensorSums[i] / baselineCount;
        }

        // Step 3: Calculate squared differences
        std::array<double, 21> squaredSums{};

        for (const auto& reading : engineReadings) {
            if (reading.cycle <= baselineCycles) {
                for (int i = 0; i < 21; ++i) {
                    double diff =
                        reading.sensors[i] - sensorMeans[i];

                    squaredSums[i] += diff * diff;
                }
            }
        }

        // Step 4: Calculate standard deviations
        std::array<double, 21> sensorStdDevs{};

        for (int i = 0; i < 21; ++i) {
            sensorStdDevs[i] =
                std::sqrt(squaredSums[i] / baselineCount);
        }

        // Step 5: Detect anomalies
        int engineAnomalies = 0;

        for (const auto& reading : engineReadings) {
            if (reading.cycle <= baselineCycles) {
                continue;
            }

            for (int i = 0; i < 21; ++i) {
                if (sensorStdDevs[i] < epsilon) {
                    continue;
                }

                double zScore =
                    (reading.sensors[i] - sensorMeans[i])
                    / sensorStdDevs[i];

                if (std::abs(zScore) > zThreshold) {
                    ++engineAnomalies;
                    ++totalAnomalies;

                    std::cout
                        << "Anomaly detected: Engine ID = "
                        << reading.engineID
                        << ", Cycle = " << reading.cycle
                        << ", Sensor ID = " << i + 1
                        << ", Value = " << reading.sensors[i]
                        << ", Z-score = " << zScore
                        << "\n";
                }
            }
        }

        ++processedEngines;

        std::cout << "ENGINE SUMMARY: Engine "
                  << engineID
                  << " | Records: " << engineReadings.size()
                  << " | Baseline: " << baselineCount
                  << " | Anomalies: " << engineAnomalies
                  << "\n";
    }

    // Overall summary
    std::cout << "\n========== FINAL SUMMARY ==========\n";
    std::cout << "Total telemetry records: "
              << dataset.size() << "\n";
    std::cout << "Total engines: "
              << engines.size() << "\n";
    std::cout << "Engines analyzed: "
              << processedEngines << "\n";
    std::cout << "Engines skipped: "
              << skippedEngines << "\n";
    std::cout << "Baseline cycles: "
              << baselineCycles << "\n";
    std::cout << "Z-score threshold: "
              << zThreshold << "\n";
    std::cout << "Total sensor-reading anomalies detected: "
              << totalAnomalies << "\n";
    std::cout << "===================================\n";

    return 0;
}
