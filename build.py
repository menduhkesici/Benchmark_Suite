#!/usr/bin/env python3
import subprocess
from pathlib import Path


def run_command(command):
    print(f"Running: {' '.join(command)}")
    subprocess.run(command, check=True)


def main():
    script_dir = Path(__file__).resolve().parent
    source_folder = script_dir / "src"
    build_folder_root = script_dir / "build"
    dependencies_src_folder = script_dir / "build_dependencies"
    dependencies_install_folder = build_folder_root / "_Dependencies"
    dependencies_build_folder = dependencies_install_folder / "build"

    # Configure, build, and install dependencies
    dependencies_build_folder.mkdir(parents=True, exist_ok=True)
    run_command(
        ["cmake", "-S", str(dependencies_src_folder), "-B", str(dependencies_build_folder), "-DCMAKE_BUILD_TYPE=Release"]
    )
    run_command(["cmake", "--build", str(dependencies_build_folder)])
    run_command(["cmake", "--install", str(dependencies_build_folder), "--prefix", str(dependencies_install_folder)])

    # Build the project in different build modes
    for build_type in ["Debug", "RelWithDebInfo", "Release"]:
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

        # TODO: Add support for static code analysis

        # TODO: Add support for code coverage statistics


if __name__ == "__main__":
    main()
