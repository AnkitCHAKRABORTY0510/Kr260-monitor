#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace kr260 {

struct CPUCore {
    unsigned int id = 0;
    double usage_percent = 0.0;
    double frequency_mhz = 0.0;
};

struct CPUInfo {
    std::vector<CPUCore> cores;

    double load_1 = 0.0;
    double load_5 = 0.0;
    double load_15 = 0.0;
};

class CPU {
public:
    CPU();

    // Read current CPU state and calculate utilization
    void update();

    const CPUInfo& info() const;

private:
    struct RawStats {
        uint64_t user = 0;
        uint64_t nice = 0;
        uint64_t system = 0;
        uint64_t idle = 0;
        uint64_t iowait = 0;
        uint64_t irq = 0;
        uint64_t softirq = 0;
        uint64_t steal = 0;

        uint64_t total() const;
        uint64_t idle_total() const;
    };

    std::vector<RawStats> previous_stats_;
    CPUInfo info_;

    std::vector<RawStats> read_stats();
    double read_frequency(unsigned int cpu);

    void calculate_usage(
        const std::vector<RawStats>& current);
};

}
