cmake_minimum_required(VERSION 3.10)

project(kr260-monitor
    VERSION 0.1.0
    LANGUAGES CXX
)

# ============================================================
# C++ configuration
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# Compiler warnings
# ============================================================

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

# ============================================================
# ncurses
# ============================================================

find_package(Curses REQUIRED)

# ============================================================
# Include directories
# ============================================================

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${CURSES_INCLUDE_DIR}
)

# ============================================================
# Source files
# ============================================================

set(SOURCES

    src/main.cpp

    src/cpu.cpp
    src/memory.cpp
    src/thermal.cpp

    src/dpu.cpp
    src/xrt.cpp

    src/ui.cpp
)

# ============================================================
# Executable
# ============================================================

add_executable(
    kr260-monitor
    ${SOURCES}
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(
    kr260-monitor
    PRIVATE
    ${CURSES_LIBRARIES}
)

# ============================================================
# Release optimization
# ============================================================

target_compile_options(
    kr260-monitor
    PRIVATE

    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-DNDEBUG>
)

# ============================================================
# Optional stronger optimization for ARM64
#
# Uncomment only if you know your KR260 compiler targets
# the correct CPU architecture.
#
# target_compile_options(
#     kr260-monitor
#     PRIVATE
#     -mcpu=cortex-a72
# )
# ============================================================

# ============================================================
# Install
# ============================================================

include(GNUInstallDirs)

install(
    TARGETS kr260-monitor
    RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
