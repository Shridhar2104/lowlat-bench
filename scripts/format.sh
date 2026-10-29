#!/usr/bin/env bash
# Format all C++ sources with the same clang-format version CI uses.
#   pipx install clang-format==23.1.3
set -euo pipefail
cd "$(dirname "$0")/.."
clang-format --version
clang-format -i bench/*.cpp
