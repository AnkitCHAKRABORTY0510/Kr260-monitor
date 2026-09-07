#include "ui.hpp"

#include <ncurses.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace kr260 {

static std::string format_bytes(
    uint64_t bytes)
{
    const char* units[] =
    {
        "B",
        "KB",
        "MB",
        "GB",
        "TB"
    };

    double value =
        static_cast<double>(bytes);

    int unit = 0;

    while (value >= 1024.0 &&
           unit < 4)
    {
        value /= 1024.0;
        ++unit;
    }

    std::ostringstream out;

    out << std::fixed
        << std::setprecision(
            unit == 0 ? 0 : 2)
        << value
        << " "
        << units[unit];

    return out.str();
}

UI::UI()
    : initialized_(false)
{
}

UI::~UI()
{
    if (initialized_)
        endwin();
}

void UI::initialize()
{
    if (initialized_)
        return;

    initscr();

    cbreak();
    noecho();

    keypad(stdscr, TRUE);

    // Non-blocking input.
    nodelay(stdscr, TRUE);

    curs_set(0);

    // Allow terminal resizing.
    clear();

    initialized_ = true;
}

void UI::draw_bar(
    int y,
    int x,
    int width,
    double percent)
{
    if (width <= 0)
        return;

    percent =
        std::clamp(
            percent,
            0.0,
            100.0);

    int filled =
        static_cast<int>(
            (percent / 100.0) *
            width);

    mvaddch(y, x, '[');

    for (int i = 0;
         i < width;
         ++i)
    {
        if (i < filled)
            addch('#');
        else
            addch(' ');
    }

    addch(']');

    printw(
        " %5.1f%%",
        percent);
}

void UI::draw_header(
    int width,
    int refresh_ms)
{
    attron(A_BOLD);

    mvprintw(
        0,
        0,
        " KR260 MONITOR");

    attroff(A_BOLD);

    std::ostringstream text;

    text << "Refresh "
         << refresh_ms
         << " ms | q Quit | r Refresh | +/- Speed";

    std::string value =
        text.str();

    int x =
        std::max(
            0,
            width -
            static_cast<int>(
                value.size()) -
            1);

    mvprintw(
        0,
        x,
        "%s",
        value.c_str());

    mvhline(
        1,
        0,
        ACS_HLINE,
        width);
}

int UI::draw_system(
    int y,
    int width,
    const CPUInfo& cpu,
    const MemoryInfo& memory,
    const Thermal& thermal)
{
    attron(A_BOLD);

    mvprintw(
        y++,
        0,
        " SYSTEM");

    attroff(A_BOLD);

    const int bar_width =
        std::min(
            28,
            std::max(
                10,
                width - 50));

    for (const auto& core :
         cpu.cores)
    {
        if (y >= LINES - 10)
            break;

        mvprintw(
            y,
            1,
            "CPU%-2u ",
            core.id);

        draw_bar(
            y,
            8,
            bar_width,
            core.usage_percent);

        if (core.frequency_mhz > 0.0)
        {
            mvprintw(
                y,
                8 + bar_width + 10,
                "%7.0f MHz",
                core.frequency_mhz);
        }

        ++y;
    }

    if (y < LINES - 1)
    {
        mvprintw(
            y,
            1,
            "RAM    ");

        draw_bar(
            y,
            8,
            bar_width,
            memory.used_percent);

        mvprintw(
            y,
            8 + bar_width + 10,
            "%s / %s",
            format_bytes(
                memory.used_bytes).c_str(),
            format_bytes(
                memory.total_bytes).c_str());

        ++y;
    }

    if (y < LINES - 1)
    {
        mvprintw(
            y,
            1,
            "SWAP   ");

        draw_bar(
            y,
            8,
            bar_width,
            memory.swap_used_percent);

        mvprintw(
            y,
            8 + bar_width + 10,
            "%s / %s",
            format_bytes(
                memory.swap_used_bytes).c_str(),
            format_bytes(
                memory.swap_total_bytes).c_str());

        ++y;
    }

    if (y < LINES - 1)
    {
        mvprintw(
            y++,
            1,
            "Load   %.2f  %.2f  %.2f",
            cpu.load_1,
            cpu.load_5,
            cpu.load_15);
    }

    if (y < LINES - 1)
    {
        double max_temp =
            thermal.max_temperature();

        if (max_temp > 0.0)
        {
            mvprintw(
                y++,
                1,
                "Temp   %.1f C",
                max_temp);
        }
        else
        {
            mvprintw(
                y++,
                1,
                "Temp   unavailable");
        }
    }

    return y;
}

int UI::draw_accelerator(
    int y,
    int width,
    const DPUInfo& dpu,
    const XRTInfo& xrt)
{
    if (y >= LINES)
        return y;

    mvhline(
        y++,
        0,
        ACS_HLINE,
        width);

    attron(A_BOLD);

    mvprintw(
        y++,
        0,
        " ACCELERATOR / DPU");

    attroff(A_BOLD);

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "XRT      : %s",
            xrt.detected
                ? "DETECTED"
                : "NOT DETECTED");
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "XRT dev  : %s",
            xrt.device.empty()
                ? "-"
                : xrt.device.c_str());
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "XRT ver  : %s",
            xrt.version.empty()
                ? "unknown"
                : xrt.version.c_str());
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "DPU      : %s",
            dpu.detected
                ? "DETECTED"
                : "NOT DETECTED");
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "DPU dev  : %s",
            dpu.device.empty()
                ? "-"
                : dpu.device.c_str());
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "DPU path : %s",
            dpu.path.empty()
                ? "-"
                : dpu.path.c_str());
    }

    if (y < LINES)
    {
        mvprintw(
            y++,
            1,
            "DPU util : ");

        if (dpu.utilization_percent >= 0.0)
        {
            printw(
                "%.1f%%",
                dpu.utilization_percent);
        }
        else
        {
            printw(
                "N/A (real counter not connected)");
        }
    }

    return y;
}

void UI::draw_footer(
    int height)
{
    if (height < 2)
        return;

    attron(A_DIM);

    mvprintw(
        height - 1,
        0,
        "Lightweight KR260 monitor | "
        "Direct /proc + /sys | "
        "No external commands");

    attroff(A_DIM);
}

bool UI::draw(
    const CPUInfo& cpu,
    const MemoryInfo& memory,
    const Thermal& thermal,
    const DPUInfo& dpu,
    const XRTInfo& xrt,
    int refresh_ms)
{
    if (!initialized_)
        initialize();

    int height;
    int width;

    getmaxyx(
        stdscr,
        height,
        width);

    erase();

    draw_header(
        width,
        refresh_ms);

    int y =
        draw_system(
            3,
            width,
            cpu,
            memory,
            thermal);

    draw_accelerator(
        y + 1,
        width,
        dpu,
        xrt);

    draw_footer(height);

    refresh();

    return true;
}

int UI::read_key()
{
    return getch();
}

}