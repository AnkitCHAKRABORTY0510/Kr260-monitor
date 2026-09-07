#include "cpu.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>

namespace kr260 {

CPU::CPU()
{
    previous_stats_ = read_stats();
}

uint64_t CPU::RawStats::total() const
{
    return user +
           nice +
           system +
           idle +
           iowait +
           irq +
           softirq +
           steal;
}

uint64_t CPU::RawStats::idle_total() const
{
    return idle + iowait;
}

std::vector<CPU::RawStats> CPU::read_stats()
{
    std::vector<RawStats> stats;

    std::ifstream file("/proc/stat");

    if (!file)
        return stats;

    std::string line;

    while (std::getline(file, line))
    {
        if (line.rfind("cpu", 0) != 0)
            break;

        // Ignore aggregate "cpu" line.
        if (line.size() <= 3 ||
            !std::isdigit(
                static_cast<unsigned char>(line[3])))
        {
            continue;
        }

        std::stringstream ss(line);

        std::string name;
        RawStats s;

        ss >> name
           >> s.user
           >> s.nice
           >> s.system
           >> s.idle
           >> s.iowait
           >> s.irq
           >> s.softirq
           >> s.steal;

        stats.push_back(s);
    }

    return stats;
}

double CPU::read_frequency(unsigned int cpu)
{
    std::ostringstream path;

    path << "/sys/devices/system/cpu/cpu"
         << cpu
         << "/cpufreq/scaling_cur_freq";

    std::ifstream file(path.str());

    if (!file)
        return 0.0;

    uint64_t khz = 0;

    file >> khz;

    return static_cast<double>(khz) / 1000.0;
}

void CPU::calculate_usage(
    const std::vector<RawStats>& current)
{
    info_.cores.resize(current.size());

    const size_t count =
        std::min(
            previous_stats_.size(),
            current.size());

    for (size_t i = 0; i < current.size(); ++i)
    {
        info_.cores[i].id =
            static_cast<unsigned int>(i);

        info_.cores[i].frequency_mhz =
            read_frequency(
                static_cast<unsigned int>(i));

        info_.cores[i].usage_percent = 0.0;

        if (i >= count)
            continue;

        uint64_t old_total =
            previous_stats_[i].total();

        uint64_t new_total =
            current[i].total();

        uint64_t old_idle =
            previous_stats_[i].idle_total();

        uint64_t new_idle =
            current[i].idle_total();

        if (new_total <= old_total)
            continue;

        uint64_t total_delta =
            new_total - old_total;

        uint64_t idle_delta = 0;

        if (new_idle >= old_idle)
            idle_delta =
                new_idle - old_idle;

        uint64_t busy_delta =
            total_delta > idle_delta
            ? total_delta - idle_delta
            : 0;

        info_.cores[i].usage_percent =
            100.0 *
            static_cast<double>(busy_delta) /
            static_cast<double>(total_delta);

        info_.cores[i].usage_percent =
            std::clamp(
                info_.cores[i].usage_percent,
                0.0,
                100.0);
    }
}

void CPU::update()
{
    auto current = read_stats();

    if (current.empty())
        return;

    calculate_usage(current);

    previous_stats_ = current;

    std::ifstream load_file("/proc/loadavg");

    if (load_file)
    {
        load_file >>
            info_.load_1 >>
            info_.load_5 >>
            info_.load_15;
    }
}

const CPUInfo& CPU::info() const
{
    return info_;
}

} // namespace kr260
