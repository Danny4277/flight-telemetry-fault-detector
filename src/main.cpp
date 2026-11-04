#include "telemetry.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>

int main() {
    const std::string filepath = "data/cmapss/train_FD001.txt";
    constexpr int baselineCycles = 30;
    constexpr double zThreshold = 3.0;

    try {
        LoadResult data = loadTelemetry(filepath);

        if (data.readings.empty()) {
            std::cerr << "Error: No valid telemetry records.\n";
            return 1;
        }

        auto engines = groupByEngine(data.readings);

        std::vector<int> engineIDs;
        for (const auto& entry : engines) {
            engineIDs.push_back(entry.first);
        }
        std::sort(engineIDs.begin(), engineIDs.end());

        std::size_t totalAnomalies = 0;
        int processedEngines = 0;
        int skippedEngines = 0;

        std::cout << "Successfully loaded "
                  << data.readings.size()
                  << " telemetry records.\n";
        std::cout << "Invalid records: "
                  << data.invalidRecords << "\n";
        std::cout << "Total engines: "
                  << engines.size() << "\n";

        for (int engineID : engineIDs) {
            EngineResult result = analyzeEngine(
                engineID,
                engines.at(engineID),
                baselineCycles,
                zThreshold
            );

            if (result.baselineCount == 0) {
                ++skippedEngines;
                std::cerr << "Warning: No baseline for Engine "
                          << engineID << "\n";
                continue;
            }

            ++processedEngines;
            totalAnomalies += result.anomalies.size();

            for (const auto& anomaly : result.anomalies) {
                std::cout
                    << "Anomaly detected: Engine ID = "
                    << anomaly.engineID
                    << ", Cycle = " << anomaly.cycle
                    << ", Sensor ID = " << anomaly.sensorID
                    << ", Value = " << anomaly.value
                    << ", Z-score = " << anomaly.zScore
                    << "\n";
            }

            std::cout << "ENGINE SUMMARY: Engine "
                      << result.engineID
                      << " | Records: " << result.recordCount
                      << " | Baseline: " << result.baselineCount
                      << " | Anomalies: " << result.anomalies.size()
                      << "\n";
        }

        std::cout << "\n========== FINAL SUMMARY ==========\n";
        std::cout << "Total telemetry records: "
                  << data.readings.size() << "\n";
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

    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}
