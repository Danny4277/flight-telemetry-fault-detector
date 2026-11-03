#include <iostream>

struct TelemetryReading {
    double altitude;
    double airspeed;
    double engineTemp;
    double vibration;
};

int main() {

    // TODO: Create a TelemetryReading object
    TelemetryReading reading = {12500.0, 245.0, 965.0, 8.2};

    // TODO: Print the four sensor measurements
    std::cout << "Altitude: " << reading.altitude << std::endl;
    std::cout << "Airspeed: " << reading.airspeed << std::endl;
    std::cout << "Engine Temp: " << reading.engineTemp << std::endl;
    std::cout << "Vibration: " << reading.vibration << std::endl;

    return 0;
}
