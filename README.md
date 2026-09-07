# KR260 Monitor

A lightweight terminal-based system monitor written in **C++17** for the **AMD/Xilinx Kria KR260** platform.

The goal of this project is to provide a low-overhead, `htop`-style monitoring interface for KR260 Linux systems, with a focus on CPU, memory, thermal information, XRT/device detection, and DPU-related information.

## Features

* CPU utilization monitoring
* CPU load average
* Current CPU frequency
* Memory usage monitoring
* Thermal zone monitoring
* DPU device discovery
* XRT/device detection
* Terminal-based interactive UI using `ncurses`
* Configurable refresh interval
* Lightweight implementation using Linux `/proc` and `/sys` interfaces
* C++17 implementation
* Modular source structure

> **Note:** DPU utilization is currently reported as `N/A` because a reliable hardware utilization counter has not yet been connected. The project intentionally avoids reporting fabricated utilization values.

## Project Structure

```text
kr260-monitor/
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── cpu.hpp
│   ├── memory.hpp
│   ├── thermal.hpp
│   ├── dpu.hpp
│   ├── xrt.hpp
│   └── ui.hpp
│
└── src/
    ├── main.cpp
    ├── cpu.cpp
    ├── memory.cpp
    ├── thermal.cpp
    ├── dpu.cpp
    ├── xrt.cpp
    └── ui.cpp
```

## Requirements

### Software

* Linux
* C++17 compatible compiler
* GCC/G++
* CMake
* ncurses development library

Install the required packages on Debian/Ubuntu-based systems:

```bash
sudo apt update
sudo apt install build-essential cmake libncurses-dev
```

### Hardware

The primary target platform is:

**AMD/Xilinx Kria KR260 Robotics Starter Kit**

The monitor is designed for Linux running on the KR260 and uses standard Linux interfaces such as:

```text
/proc
/sys
/dev
```

## Build

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/kr260-monitor.git
cd kr260-monitor
```

Create a build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Build:

```bash
cmake --build . -j$(nproc)
```

The executable will be created as:

```text
build/kr260-monitor
```

## Run

From the build directory:

```bash
./kr260-monitor
```

If the monitor requires access to specific hardware or system interfaces on your KR260, run it with the appropriate permissions.

## Keyboard Controls

| Key       | Action                    |
| --------- | ------------------------- |
| `q` / `Q` | Quit                      |
| `r` / `R` | Refresh immediately       |
| `+`       | Increase refresh interval |
| `-`       | Decrease refresh interval |

## Data Sources

The monitor intentionally avoids repeatedly launching external monitoring commands where possible.

### CPU

CPU information is obtained from Linux interfaces such as:

```text
/proc/stat
/proc/loadavg
/sys/devices/system/cpu/
```

CPU utilization is calculated from `/proc/stat`.

### Memory

Memory information is obtained from:

```text
/proc/meminfo
```

The monitor uses this information to calculate memory usage.

### Thermal

Thermal information is discovered through:

```text
/sys/class/thermal/
```

Available thermal zones are inspected dynamically.

### DPU

The DPU module currently performs conservative device discovery through Linux system interfaces.

Actual DPU utilization requires a reliable runtime or hardware counter and is therefore not estimated or fabricated.

### XRT

The XRT module performs basic device/runtime detection using Linux device and system interfaces.

Future versions will integrate the native XRT C++ API for more detailed accelerator information.

## Design Goals

The project is designed around several principles:

### Low Overhead

The monitor should consume as few system resources as reasonably possible.

Instead of repeatedly executing commands such as:

```bash
xrt-smi
xdputil
```

the monitor reads system interfaces directly whenever possible.

This avoids repeatedly creating external processes during every refresh cycle.

### Modular Architecture

Each monitoring subsystem is implemented independently:

```text
CPU
 │
 ├── cpu.cpp
 │
Memory
 │
 ├── memory.cpp
 │
Thermal
 │
 ├── thermal.cpp
 │
DPU
 │
 ├── dpu.cpp
 │
XRT
 │
 ├── xrt.cpp
 │
UI
 │
 └── ui.cpp
```

This makes it easier to add additional KR260-specific monitoring functionality later.

## Planned Features

The project is still under development.

Planned improvements include:

* [ ] Native XRT C++ API integration
* [ ] Real DPU utilization monitoring
* [ ] DPU job/counter monitoring
* [ ] DPU frequency monitoring
* [ ] FPGA/accelerator status
* [ ] XRT memory usage
* [ ] Accelerator temperature monitoring
* [ ] Power monitoring
* [ ] More detailed CPU frequency information
* [ ] CPU core-by-core utilization
* [ ] Network statistics
* [ ] Storage statistics
* [ ] Process monitoring
* [ ] Configurable refresh rate
* [ ] KR260-specific accelerator telemetry
* [ ] SED-VP custom hardware counters

## Future SED-VP Integration

One of the long-term goals is to extend the monitor with telemetry from the **SED-VP custom accelerator**.

The planned architecture is:

```text
                    KR260 Monitor
                         │
        ┌────────────────┼────────────────┐
        │                │                │
       CPU             Memory           Thermal
        │                │                │
        └────────────────┼────────────────┘
                         │
                    Accelerator
                         │
              ┌──────────┴──────────┐
              │                     │
             XRT                   DPU
              │                     │
              └──────────┬──────────┘
                         │
                    SED-VP HW
                         │
              Custom Performance
                    Counters
```

This would allow the monitor to display both general KR260 system information and SED-VP accelerator-specific performance metrics in one terminal interface.

## Development

Build in Release mode:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j$(nproc)
```

For development/debugging:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j$(nproc)
```

## Contributing

Contributions, improvements, bug fixes, and KR260-specific monitoring ideas are welcome.

Before submitting changes:

1. Build the project.
2. Verify the monitor starts correctly.
3. Check that the monitor does not crash if a `/proc` or `/sys` entry is unavailable.
4. Avoid adding unnecessary external process execution to the refresh loop.
5. Keep platform-specific functionality isolated in its corresponding module.

## License

This project is currently provided for research and development purposes.

A formal open-source license will be added in a future release.

## Status

**Development / Experimental**

The basic monitoring framework is functional. XRT and DPU monitoring are currently being expanded toward hardware-backed telemetry.

---

**KR260 Monitor — Lightweight C++ System & Accelerator Monitor**
