#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

constexpr int SENSOR_COUNT = 21;
constexpr int SETTING_COUNT = 3;

struct TelemetryReading {
    int engineID = 0;
    int cycle = 0;
    std::array<double, SETTING_COUNT> operatingSettings{};
    std::array<double, SENSOR_COUNT> sensors{};
};

struct LoadResult {
    std::vector<TelemetryReading> readings;
    int invalidRecords = 0;
};

struct BaselineStats {
    std::array<double, SENSOR_COUNT> means{};
    std::array<double, SENSOR_COUNT> stdDevs{};
    int count = 0;
};

struct Anomaly {
    int engineID;
    int cycle;
    int sensorID;
    double value;
    double zScore;
};

struct EngineResult {
    int engineID;
    std::size_t recordCount;
    int baselineCount;
    std::vector<Anomaly> anomalies;
};

LoadResult loadTelemetry(const std::string& filepath);

std::unordered_map<int, std::vector<TelemetryReading>>
groupByEngine(const std::vector<TelemetryReading>& readings);

BaselineStats calculateBaseline(
    const std::vector<TelemetryReading>& readings,
    int baselineCycles = 30
);

std::vector<Anomaly> detectAnomalies(
    const std::vector<TelemetryReading>& readings,
    const BaselineStats& baseline,
    int baselineCycles = 30,
    double zThreshold = 3.0
);

EngineResult analyzeEngine(
    int engineID,
    const std::vector<TelemetryReading>& readings,
    int baselineCycles = 30,
    double zThreshold = 3.0
);
