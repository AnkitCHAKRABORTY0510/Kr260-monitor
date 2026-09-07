#include "thermal.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace kr260 {

Thermal::Thermal()
{
    update();
}

void Thermal::update()
{
    sensors_.clear();

    const fs::path base =
        "/sys/class/thermal";

    std::error_code ec;

    if (!fs::exists(base, ec))
        return;

    for (const auto& entry :
         fs::directory_iterator(base, ec))
    {
        if (ec)
            break;

        if (!entry.is_directory())
            continue;

        const fs::path directory =
            entry.path();

        std::ifstream type_file(
            directory / "type");

        std::string type;

        if (type_file)
            std::getline(type_file, type);

        std::ifstream temp_file(
            directory / "temp");

        long long temperature = 0;

        if (!temp_file ||
            !(temp_file >> temperature))
        {
            continue;
        }

        ThermalSensor sensor;

        sensor.name =
            type.empty()
            ? directory.filename().string()
            : type;

        // Linux thermal zone temperature is normally
        // reported in millidegrees Celsius.
        sensor.temperature_c =
            static_cast<double>(
                temperature) / 1000.0;

        sensors_.push_back(sensor);
    }

    std::sort(
        sensors_.begin(),
        sensors_.end(),
        [](const ThermalSensor& a,
           const ThermalSensor& b)
        {
            return a.name < b.name;
        });
}

const std::vector<ThermalSensor>&
Thermal::sensors() const
{
    return sensors_;
}

double Thermal::max_temperature() const
{
    if (sensors_.empty())
        return 0.0;

    double maximum = sensors_[0].temperature_c;

    for (const auto& sensor : sensors_)
    {
        maximum =
            std::max(
                maximum,
                sensor.temperature_c);
    }

    return maximum;
}

} 