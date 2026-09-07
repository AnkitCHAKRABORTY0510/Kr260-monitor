#pragma once

#include <string>
#include <cstdint>

namespace kr260 {

struct DPUInfo {
    bool detected = false;

    std::string device;
    std::string path;

    std::string architecture;
    std::string status;

    double frequency_mhz = 0.0;

    // These remain invalid until a real DPU performance
    // counter is connected.
    double utilization_percent = -1.0;

    uint64_t jobs = 0;
    double latency_ms = 0.0;
};

class DPU {
public:
    DPU();

    void update();

    const DPUInfo& info() const;

private:
    DPUInfo info_;

    void discover();
};

}
 