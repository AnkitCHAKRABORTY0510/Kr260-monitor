#pragma once

#include <cstdint>
#include <string>

namespace kr260 {

enum class DPUState {
    Unavailable,
    InstalledNotLoaded,
    Loaded
};

struct DPUInfo {
    bool detected = false;

    DPUState state = DPUState::Unavailable;

    std::string device;
    std::string path;

    std::string architecture;
    std::string status;

    double frequency_mhz = 0.0;

    // -1.0 means unavailable.
    double utilization_percent = -1.0;

    // Valid only when connected to a real DPU/XRT counter.
    uint64_t jobs = 0;

    // Valid only when connected to a real DPU performance counter.
    double latency_ms = 0.0;
};

class DPU {
public:
    DPU();

    // Refresh DPU/XRT state.
    void update();

    // Return current DPU information.
    const DPUInfo& info() const;

private:
    DPUInfo info_;

    void discover();
};

} 

