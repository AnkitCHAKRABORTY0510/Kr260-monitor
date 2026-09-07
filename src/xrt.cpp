#include "xrt.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace kr260 {

XRT::XRT()
{
    discover();
    detect_version();
}

void XRT::discover()
{
    info_ = XRTInfo{};

    /*
     * XRT-backed accelerator devices commonly appear
     * through DRM render nodes.
     *
     * We deliberately don't execute xrt-smi here.
     */

    const std::string dri_path =
        "/dev/dri";

    std::error_code ec;

    if (!fs::exists(dri_path, ec))
        return;

    for (const auto& entry :
         fs::directory_iterator(dri_path, ec))
    {
        if (ec)
            break;

        std::string name =
            entry.path().filename().string();

        if (name.rfind("renderD", 0) == 0)
        {
            info_.detected = true;
            info_.device = name;
            info_.device_path =
                entry.path().string();
            info_.status = "DETECTED";

            return;
        }
    }
}

void XRT::detect_version()
{
    /*
     * Try common XRT version files without invoking
     * an external process.
     *
     * Different KR260/PetaLinux images expose this
     * information differently.
     */

    const std::vector<std::string> paths =
    {
        "/sys/module/zocl/version",
        "/sys/module/xrt/version"
    };

    for (const auto& path : paths)
    {
        std::ifstream file(path);

        if (!file)
            continue;

        std::getline(file, info_.version);

        if (!info_.version.empty())
            return;
    }

    info_.version = "unknown";
}

void XRT::update()
{
    /*
     * No expensive probing here.
     *
     * Actual XRT device statistics should eventually
     * be obtained through the native XRT API.
     */
}

const XRTInfo& XRT::info() const
{
    return info_;
}

}
