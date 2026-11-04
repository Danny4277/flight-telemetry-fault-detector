#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

struct TelemetryReading {
    int engineID;
    int cycle;
    std::array<double, 3> operatingSettings;
    std::array<double, 21> sensors;
};

int main() {
    std::string filepath = "data/cmapss/train_FD001.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return 1;
    }

    std::vector<TelemetryReading> dataset;
    int invalidRecords = 0;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        TelemetryReading reading;
        bool parse_success = false;
        
        if (ss >> reading.engineID >> reading.cycle) {
            parse_success = true;
            for (int i = 0; i < 3; ++i) {
                if (!(ss >> reading.operatingSettings[i])) {
                    parse_success = false;
                    break;
                }
            }
            for (int i = 0; i < 21 && parse_success; ++i) {
                if (!(ss >> reading.sensors[i])) {
                    parse_success = false;
                    break;
                }
            }
        }
        
        if (parse_success) {
            dataset.push_back(reading);
        } else {
            ++invalidRecords;
        }
    }

    file.close();

    std::cout << "Successfully loaded " << dataset.size() << " telemetry records." << std::endl;
    std::cout << "Invalid records: " << invalidRecords << std::endl;
    if (!dataset.empty()) {
        std::cout << "First record: Engine ID = " << dataset[0].engineID 
                  << ", Cycle = " << dataset[0].cycle 
                  << ", First Sensor = " << dataset[0].sensors[0] << std::endl;
    }

    return 0;
}
