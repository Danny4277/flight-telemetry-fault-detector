#include "../src/telemetry.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

TelemetryReading makeReading(
    int engineID, int cycle, double sensorValue
) {
    TelemetryReading reading{};
    reading.engineID = engineID;
    reading.cycle = cycle;
    reading.sensors.fill(100.0);
    reading.sensors[0] = sensorValue;
    return reading;
}

void testGrouping() {
    std::vector<TelemetryReading> readings = {
        makeReading(1, 1, 10),
        makeReading(2, 1, 20),
        makeReading(1, 2, 30)
    };

    auto engines = groupByEngine(readings);

    assert(engines.size() == 2);
    assert(engines.at(1).size() == 2);
    assert(engines.at(2).size() == 1);
}

void testBaseline() {
    std::vector<TelemetryReading> readings = {
        makeReading(1, 1, 10),
        makeReading(1, 2, 20),
        makeReading(1, 3, 30),
        makeReading(1, 4, 100)
    };

    BaselineStats stats = calculateBaseline(readings, 3);

    assert(stats.count == 3);
    assert(std::abs(stats.means[0] - 20.0) < 1e-9);
    assert(std::abs(
        stats.stdDevs[0] - std::sqrt(200.0 / 3.0)
    ) < 1e-9);
}

void testAnomalyDetection() {
    std::vector<TelemetryReading> readings = {
        makeReading(1, 1, 9),
        makeReading(1, 2, 10),
        makeReading(1, 3, 11),
        makeReading(1, 4, 10),
        makeReading(1, 5, 20)
    };

    BaselineStats stats = calculateBaseline(readings, 3);
    auto anomalies = detectAnomalies(readings, stats, 3, 3.0);

    assert(anomalies.size() == 1);
    assert(anomalies[0].engineID == 1);
    assert(anomalies[0].cycle == 5);
    assert(anomalies[0].sensorID == 1);
    assert(anomalies[0].zScore > 3.0);
}

void testConstantSensor() {
    std::vector<TelemetryReading> readings = {
        makeReading(1, 1, 10),
        makeReading(1, 2, 10),
        makeReading(1, 3, 10),
        makeReading(1, 4, 50)
    };

    BaselineStats stats = calculateBaseline(readings, 3);
    auto anomalies = detectAnomalies(readings, stats, 3, 3.0);

    assert(stats.stdDevs[0] == 0.0);
    assert(anomalies.empty());
}

void testMissingBaseline() {
    std::vector<TelemetryReading> readings = {
        makeReading(1, 40, 10)
    };

    EngineResult result = analyzeEngine(1, readings);

    assert(result.baselineCount == 0);
    assert(result.anomalies.empty());
}

void testInvalidBaselineCycles() {
    bool caught = false;

    try {
        calculateBaseline({}, 0);
    } catch (const std::invalid_argument&) {
        caught = true;
    }

    assert(caught);
}

void testInvalidThreshold() {
    bool caught = false;

    try {
        detectAnomalies({}, BaselineStats{}, 30, 0.0);
    } catch (const std::invalid_argument&) {
        caught = true;
    }

    assert(caught);
}

int main() {
    testGrouping();
    testBaseline();
    testAnomalyDetection();
    testConstantSensor();
    testMissingBaseline();
    testInvalidBaselineCycles();
    testInvalidThreshold();

    std::cout << "All 7 unit tests passed!\n";
    return 0;
}
