#include "dpu.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace kr260 {

namespace {

/*
 * Convert a string to lowercase.
 */
std::string lowercase(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });

    return value;
}


/*
 * Check whether a file or device exists.
 */
bool file_exists(const std::string& path)
{
    std::error_code ec;

    return fs::exists(path, ec);
}


/*
 * Read the first line of a text file.
 */
std::string read_file(const std::string& path)
{
    std::ifstream file(path);

    if (!file)
        return {};

    std::string value;

    std::getline(file, value);

    return value;
}


/*
 * Check whether the KR260 XRT/ZOCL render device exists.
 *
 * On the KR260 target board this is normally:
 *
 *     /dev/dri/renderD128
 */
bool has_xrt_device()
{
    return file_exists("/dev/dri/renderD128");
}


/*
 * Read the number of compute units reported by ZOCL.
 *
 * ZOCL exposes this through:
 *
 *     /sys/class/drm/renderD128/device/kds_numcus
 *
 * When no accelerator XCLBIN is loaded, the KR260
 * currently reports 0.
 */
int read_number_of_cus()
{
    const std::string path =
        "/sys/class/drm/renderD128/device/kds_numcus";

    const std::string value = read_file(path);

    if (value.empty())
        return -1;

    try
    {
        return std::stoi(value);
    }
    catch (...)
    {
        return -1;
    }
}


/*
 * Determine whether an accelerator XCLBIN is loaded.
 *
 * ZOCL exposes the currently loaded XCLBIN UUID through:
 *
 *     /sys/class/drm/renderD128/device/xclbinid
 *
 * An all-zero UUID means that no XCLBIN is currently loaded.
 */
bool xclbin_loaded()
{
    const std::string path =
        "/sys/class/drm/renderD128/device/xclbinid";

    const std::string uuid = read_file(path);

    if (uuid.empty())
        return false;

    const std::string lower = lowercase(uuid);

    /*
     * Standard UUID representation.
     */
    if (lower ==
        "00000000-0000-0000-0000-000000000000")
    {
        return false;
    }

    /*
     * Some systems may expose the UUID without
     * hyphens.
     */
    if (lower ==
        "00000000000000000000000000000000")
    {
        return false;
    }

    return true;
}

} // namespace


/*
 * Constructor.
 */
DPU::DPU()
{
    discover();
}


/*
 * Discover the current DPU/XRT state.
 */
void DPU::discover()
{
    /*
     * Reset the information before every discovery.
     */
    info_ = DPUInfo{};


    /*
     * ------------------------------------------------------------
     * 1. Check XRT / ZOCL
     * ------------------------------------------------------------
     */

    if (!has_xrt_device())
    {
        info_.detected = false;
        info_.state = DPUState::Unavailable;

        info_.device.clear();
        info_.path.clear();

        info_.status = "XRT NOT AVAILABLE";

        return;
    }


    /*
     * XRT device exists.
     */
    info_.device = "/dev/dri/renderD128";


    /*
     * ------------------------------------------------------------
     * 2. Check whether the DPU XCLBIN is installed
     * ------------------------------------------------------------
     *
     * On the current KR260 software installation we expect:
     *
     *     /usr/lib/dpu.xclbin
     */
    const std::string dpu_xclbin =
        "/usr/lib/dpu.xclbin";

    info_.path = dpu_xclbin;


    if (!file_exists(dpu_xclbin))
    {
        info_.detected = false;
        info_.state = DPUState::Unavailable;

        info_.status = "DPU IMAGE NOT FOUND";

        return;
    }


    /*
     * At this point:
     *
     *     XRT device      -> available
     *     DPU XCLBIN file -> installed
     *
     * This does NOT necessarily mean that the DPU
     * is currently loaded into the FPGA/XRT.
     */


    /*
     * ------------------------------------------------------------
     * 3. Check whether an XCLBIN is currently loaded
     * ------------------------------------------------------------
     */

    const bool loaded = xclbin_loaded();


    /*
     * ------------------------------------------------------------
     * 4. Check number of compute units
     * ------------------------------------------------------------
     */

    const int num_cus = read_number_of_cus();


    /*
     * If there is no loaded XCLBIN or there are no compute units,
     * report:
     *
     *     INSTALLED / NOT LOADED
     *
     * This is the expected state on the current KR260 board
     * before the DPU accelerator overlay/XCLBIN is loaded.
     */
    if (!loaded || num_cus <= 0)
    {
        info_.detected = false;
        info_.state = DPUState::InstalledNotLoaded;

        info_.status = "INSTALLED / NOT LOADED";

        info_.architecture.clear();

        info_.frequency_mhz = 0.0;

        /*
         * -1 means the value is unavailable.
         */
        info_.utilization_percent = -1.0;

        info_.jobs = 0;

        info_.latency_ms = 0.0;

        return;
    }


    /*
     * ------------------------------------------------------------
     * 5. XCLBIN is loaded and at least one CU exists
     * ------------------------------------------------------------
     *
     * We can safely report that an accelerator image is loaded.
     *
     * We deliberately do NOT claim that every CU is a DPU.
     * Proper DPU identification requires inspecting the actual
     * XCLBIN metadata.
     */
    info_.detected = true;

    info_.state = DPUState::Loaded;

    info_.status = "LOADED";


    /*
     * ------------------------------------------------------------
     * 6. DPU performance information
     * ------------------------------------------------------------
     *
     * These values remain unavailable until the monitor is
     * connected to real DPU/VART/XRT performance counters.
     */

    info_.architecture.clear();

    info_.frequency_mhz = 0.0;

    /*
     * -1.0 means unavailable.
     */
    info_.utilization_percent = -1.0;

    info_.jobs = 0;

    info_.latency_ms = 0.0;
}


/*
 * Refresh DPU information.
 *
 * This is called by the main monitoring loop.
 */
void DPU::update()
{
    /*
     * Re-run discovery because the accelerator can be
     * loaded or unloaded while the monitor is running.
     */
    discover();
}


/*
 * Return the current DPU information.
 */
const DPUInfo& DPU::info() const
{
    return info_;
}

}
