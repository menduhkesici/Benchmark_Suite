#!/usr/bin/env python3
import argparse
import subprocess
from pathlib import Path


script_dir = Path(__file__).resolve().parent
source_folder = script_dir
build_folder_root = script_dir / "build"
dependencies_src_folder = script_dir / "build_dependencies"
dependencies_install_folder = build_folder_root / "_Dependencies"
dependencies_build_folder = dependencies_install_folder / "build"


def run_command(command: list[str]):
    print(f"Running: {' '.join(map(str, command))}")
    subprocess.run(command, check=True)


def configure_dependencies():
    dependencies_build_folder.mkdir(parents=True, exist_ok=True)
    run_command(
        ["cmake", "-S", str(dependencies_src_folder), "-B", str(dependencies_build_folder), "-DCMAKE_BUILD_TYPE=Release"]
    )
    run_command(["cmake", "--build", str(dependencies_build_folder)])
    run_command(["cmake", "--install", str(dependencies_build_folder), "--prefix", str(dependencies_install_folder)])


def build_project(build_type: str):
    build_folder = build_folder_root / build_type
    build_folder.mkdir(parents=True, exist_ok=True)

    run_command(
        [
            "cmake",
            "-S",
            str(source_folder),
            "-B",
            str(build_folder),
            f"-DCMAKE_INSTALL_PREFIX={dependencies_install_folder}",
            f"-DCMAKE_BUILD_TYPE={build_type}",
        ]
    )
    run_command(["cmake", "--build", str(build_folder)])


def run_profile(exe_name: str):
    build_bin_folder = build_folder_root / "RelWithDebInfo" / "bin"
    callgrind_out = build_folder_root / f"callgrind.out.{exe_name}"
    exe_path = build_bin_folder / exe_name

    run_command(["valgrind", "--tool=callgrind", f"--callgrind-out-file={callgrind_out}", str(exe_path)])


def run_clang_tidy():
    build_folder = build_folder_root / "Debug"
    run_command(["run-clang-tidy-18", "-p", str(build_folder)])


def main():
    parser = argparse.ArgumentParser(description="Build C++ project")
    parser.add_argument(
        "--profile",
        help="Run profiling with valgrind for the given executable name",
        type=str,
    )
    parser.add_argument(
        "--tidy",
        help="Run static analysis with clang-tidy for the project",
        action="store_true",
    )
    # TODO: Add support for code coverage statistics
    args = parser.parse_args()

    if args.profile:
        run_profile(exe_name=args.profile)
    elif args.tidy:
        run_clang_tidy()
    else:
        configure_dependencies()
        for build_type in ["Debug", "RelWithDebInfo", "Release"]:
            build_project(build_type=build_type)


if __name__ == "__main__":
    main()
