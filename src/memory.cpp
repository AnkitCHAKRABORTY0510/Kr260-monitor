#include "memory.hpp"

#include <fstream>
#include <string>

namespace kr260 {

Memory::Memory()
{
    update();
}

void Memory::read_meminfo()
{
    std::ifstream file("/proc/meminfo");

    if (!file)
        return;

    std::string key;
    uint64_t value;
    std::string unit;

    while (file >> key >> value >> unit)
    {
        // /proc/meminfo reports kB.
        uint64_t bytes =
            value * 1024ULL;

        if (key == "MemTotal:")
        {
            info_.total_bytes = bytes;
        }
        else if (key == "MemAvailable:")
        {
            info_.available_bytes = bytes;
        }
        else if (key == "SwapTotal:")
        {
            info_.swap_total_bytes = bytes;
        }
        else if (key == "SwapFree:")
        {
            info_.swap_free_bytes = bytes;
        }
    }

    if (info_.total_bytes >=
        info_.available_bytes)
    {
        info_.used_bytes =
            info_.total_bytes -
            info_.available_bytes;
    }
    else
    {
        info_.used_bytes = 0;
    }

    if (info_.total_bytes > 0)
    {
        info_.used_percent =
            100.0 *
            static_cast<double>(
                info_.used_bytes) /
            static_cast<double>(
                info_.total_bytes);
    }

    if (info_.swap_total_bytes >=
        info_.swap_free_bytes)
    {
        info_.swap_used_bytes =
            info_.swap_total_bytes -
            info_.swap_free_bytes;
    }
    else
    {
        info_.swap_used_bytes = 0;
    }

    if (info_.swap_total_bytes > 0)
    {
        info_.swap_used_percent =
            100.0 *
            static_cast<double>(
                info_.swap_used_bytes) /
            static_cast<double>(
                info_.swap_total_bytes);
    }
    else
    {
        info_.swap_used_percent = 0.0;
    }
}

void Memory::update()
{
    read_meminfo();
}

const MemoryInfo& Memory::info() const
{
    return info_;
}

} 
