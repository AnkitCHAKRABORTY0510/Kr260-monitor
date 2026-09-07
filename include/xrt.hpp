#pragma once

#include <string>

namespace kr260 {

struct XRTInfo {
    bool detected = false;

    std::string device;
    std::string device_path;

    std::string status;
    std::string version;
};

class XRT {
public:
    XRT();

    void update();

    const XRTInfo& info() const;

private:
    XRTInfo info_;

    void discover();
    void detect_version();
};

} 
