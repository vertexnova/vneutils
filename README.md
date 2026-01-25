# VneUtils

<p align="center">
  <a href="https://github.com/vertexnova/vneutils/actions/workflows/ci.yml">
    <img src="https://github.com/vertexnova/vneutils/actions/workflows/ci.yml/badge.svg?branch=main" alt="CI"/>
  </a>
  <a href="https://codecov.io/gh/vertexnova/vneutils">
    <img src="https://codecov.io/gh/vertexnova/vneutils/branch/main/graph/badge.svg" alt="Coverage"/>
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="C++ Standard"/>
  <img src="https://img.shields.io/badge/license-Apache%202.0-green.svg" alt="License"/>
</p>

Utility library for VertexNova projects providing common helpers for command-line parsing, file I/O, threading, and more.

## Features

- **Command Line Parser** - Flexible argument parsing with support for options, flags, and positional arguments

## Requirements

- C++20 or later
- CMake 3.16 or later

## Building

### macOS / Linux

```bash
git clone --recursive https://github.com/vertexnova/vneutils.git
cd vneutils
./scripts/build_macos.sh
```

### Windows

```powershell
git clone --recursive https://github.com/vertexnova/vneutils.git
cd vneutils
python scripts/build_windows.py
```

### CMake Direct

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build
```

## Usage

### Command Line Parser

```cpp
#include "vertexnova/utils/command_line_parser.h"

int main(int argc, char* argv[]) {
    vne::utils::CommandLineParser parser("MyApp", "1.0.0");
    
    parser.addOption({"--width", "-w"}, "Window width", "pixels", "1920");
    parser.addOption({"--height", "-h"}, "Window height", "pixels", "1080");
    parser.addOption({"--fullscreen", "-f"}, "Run in fullscreen mode");
    
    if (!parser.parse(argc, argv)) {
        std::cerr << parser.getErrorMessage() << std::endl;
        return 1;
    }
    
    if (parser.isHelpRequested()) {
        parser.showHelp();
        return 0;
    }
    
    int width = std::stoi(parser.value("--width"));
    int height = std::stoi(parser.value("--height"));
    bool fullscreen = parser.isSet("--fullscreen");
    
    // Use the values...
    return 0;
}
```

## Dependencies

### CMake Modules (`cmake/vnecmake/`)

| Module | Description |
|--------|-------------|
| VneCMake | Shared CMake modules for VertexNova projects |

## License

Apache License 2.0 - See [LICENSE](LICENSE) for details.
