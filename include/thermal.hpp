#pragma once

#include <string>
#include <vector>

namespace kr260 {

struct ThermalSensor {
    std::string name;
    double temperature_c = 0.0;
};

class Thermal {
public:
    Thermal();

    void update();

    const std::vector<ThermalSensor>& sensors() const;

    // Highest detected temperature.
    double max_temperature() const;

private:
    std::vector<ThermalSensor> sensors_;
};

}
