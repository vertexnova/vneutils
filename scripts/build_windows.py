#!/usr/bin/env python3
# ----------------------------------------------------------------------
# Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
# ----------------------------------------------------------------------

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


def get_project_dir() -> Path:
    return Path(__file__).parent.parent.resolve()


def run_command(cmd: list[str], cwd: Path | None = None) -> int:
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, cwd=cwd)
    return result.returncode


def main() -> int:
    parser = argparse.ArgumentParser(description="Build VneUtils on Windows")
    parser.add_argument("--release", action="store_true", help="Build in Release mode")
    parser.add_argument("--debug", action="store_true", help="Build in Debug mode (default)")
    parser.add_argument("--no-tests", action="store_true", help="Don't build tests")
    parser.add_argument("--clean", action="store_true", help="Clean build directory")

    args = parser.parse_args()

    build_type = "Release" if args.release else "Debug"
    build_tests = "OFF" if args.no_tests else "ON"

    project_dir = get_project_dir()
    build_dir = project_dir / "build" / build_type

    print("=== VneUtils Build Script (Windows) ===")
    print(f"Build Type: {build_type}")
    print(f"Build Tests: {build_tests}")

    if args.clean and build_dir.exists():
        shutil.rmtree(build_dir)

    build_dir.mkdir(parents=True, exist_ok=True)

    configure_cmd = [
        "cmake", "-B", str(build_dir), "-S", str(project_dir),
        f"-DCMAKE_BUILD_TYPE={build_type}",
        f"-DBUILD_TESTS={build_tests}",
    ]

    if run_command(configure_cmd) != 0:
        return 1

    build_cmd = ["cmake", "--build", str(build_dir), "--config", build_type, "--parallel"]

    if run_command(build_cmd) != 0:
        return 1

    print("=== Build Complete ===")
    return 0


if __name__ == "__main__":
    sys.exit(main())
