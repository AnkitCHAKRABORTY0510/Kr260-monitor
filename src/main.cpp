#include "cpu.hpp"
#include "dpu.hpp"
#include "memory.hpp"
#include "thermal.hpp"
#include "ui.hpp"
#include "xrt.hpp"

#include <chrono>
#include <thread>
#include <ncurses.h>

using namespace kr260;

int main()
{
    CPU cpu;
    Memory memory;
    Thermal thermal;
    DPU dpu;
    XRT xrt;
    UI ui;

    ui.initialize();

    int refresh_ms = 1000;

    bool running = true;

    // Initial CPU baseline.
    cpu.update();

    while (running)
    {
        // ----------------------------------------------------
        // Update data
        // ----------------------------------------------------

        cpu.update();
        memory.update();
        thermal.update();
        dpu.update();
        xrt.update();

        // ----------------------------------------------------
        // Draw UI
        // ----------------------------------------------------

        ui.draw(
            cpu.info(),
            memory.info(),
            thermal,
            dpu.info(),
            xrt.info(),
            refresh_ms);

        // ----------------------------------------------------
        // Wait while keeping keyboard responsive.
        // ----------------------------------------------------

        int remaining =
            refresh_ms;

        while (remaining > 0)
        {
            constexpr int STEP_MS = 50;

            int sleep_ms =
                remaining > STEP_MS
                ? STEP_MS
                : remaining;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    sleep_ms));

            remaining -= sleep_ms;

            int key =
                ui.read_key();

            if (key == ERR)
                continue;

            switch (key)
            {
                case 'q':
                case 'Q':
                    running = false;
                    break;

                case 'r':
                case 'R':
                    remaining = 0;
                    break;

                case '+':
                case '=':
                    refresh_ms /= 2;

                    if (refresh_ms < 100)
                        refresh_ms = 100;

                    remaining = 0;
                    break;

                case '-':
                case '_':
                    refresh_ms *= 2;

                    if (refresh_ms > 5000)
                        refresh_ms = 5000;

                    remaining = 0;
                    break;

                default:
                    break;
            }

            if (!running)
                break;
        }
    }

    return 0;
}
