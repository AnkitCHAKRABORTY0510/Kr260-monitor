#pragma once

#include <cstdint>

namespace kr260 {

struct MemoryInfo {
    uint64_t total_bytes = 0;
    uint64_t available_bytes = 0;

    uint64_t used_bytes = 0;

    uint64_t swap_total_bytes = 0;
    uint64_t swap_free_bytes = 0;
    uint64_t swap_used_bytes = 0;

    double used_percent = 0.0;
    double swap_used_percent = 0.0;
};

class Memory {
public:
    Memory();

    void update();

    const MemoryInfo& info() const;

private:
    MemoryInfo info_;

    void read_meminfo();
};

} 

