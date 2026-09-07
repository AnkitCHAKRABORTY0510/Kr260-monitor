#pragma once

#include "cpu.hpp"
#include "dpu.hpp"
#include "memory.hpp"
#include "thermal.hpp"
#include "xrt.hpp"

namespace kr260 {

class UI {
public:
    UI();
    ~UI();

    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;

    void initialize();

    // Returns:
    //   true  -> continue
    //   false -> quit
    bool draw(
        const CPUInfo& cpu,
        const MemoryInfo& memory,
        const Thermal& thermal,
        const DPUInfo& dpu,
        const XRTInfo& xrt,
        int refresh_ms);

    int read_key();

private:
    bool initialized_;

    void draw_header(
        int width,
        int refresh_ms);

    int draw_system(
        int y,
        int width,
        const CPUInfo& cpu,
        const MemoryInfo& memory,
        const Thermal& thermal);

    int draw_accelerator(
        int y,
        int width,
        const DPUInfo& dpu,
        const XRTInfo& xrt);

    void draw_bar(
        int y,
        int x,
        int width,
        double percent);

    void draw_footer(
        int height);
};

}
