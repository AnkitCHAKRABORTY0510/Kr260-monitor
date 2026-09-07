#include "dpu.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

namespace fs = std::filesystem;

namespace kr260 {

static std::string lowercase(
    std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(
                std::tolower(c));
        });

    return value;
}

DPU::DPU()
{
    discover();
}

void DPU::discover()
{
    info_ = DPUInfo{};

    const std::vector<std::string> bases =
    {
        "/sys/class/accel",
        "/sys/class/misc",
        "/sys/bus/platform/devices",
        "/sys/bus/amba/devices"
    };

    for (const auto& base : bases)
    {
        std::error_code ec;

        if (!fs::exists(base, ec))
            continue;

        for (const auto& entry :
             fs::directory_iterator(base, ec))
        {
            if (ec)
                break;

            std::string name =
                entry.path().filename().string();

            std::string lower =
                lowercase(name);

            if (lower.find("dpu") ==
                std::string::npos)
            {
                continue;
            }

            info_.detected = true;
            info_.device = name;
            info_.path =
                entry.path().string();

            info_.status = "DETECTED";

            /*
             * Do not guess architecture,
             * frequency or utilization.
             *
             * These values depend on the actual
             * DPU overlay/design and XRT/Vitis-AI
             * stack installed on the KR260.
             */

            return;
        }
    }
}

void DPU::update()
{
    /*
     * Discovery is cheap, but we don't need to
     * rediscover the whole sysfs tree every refresh.
     *
     * For now the monitor keeps the discovered
     * information.
     *
     * Later this function can read actual DPU/XRT
     * performance counters.
     */
}

const DPUInfo& DPU::info() const
{
    return info_;
}

}
