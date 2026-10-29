#!/usr/bin/env bash
# Run every benchmark in build/bench and save JSON results for plotting.
#   scripts/run_all.sh                 # all benchmarks
#   scripts/run_all.sh --benchmark_repetitions=5   # extra args go to every binary
set -euo pipefail
cd "$(dirname "$0")/.."
[[ -d build/bench ]] || { echo "Build first: cmake -S . -B build -G Ninja && cmake --build build"; exit 1; }
mkdir -p results
for b in build/bench/*; do
  [[ -x "$b" ]] || continue
  name="$(basename "$b")"
  echo "=== $name"
  "$b" --benchmark_out="results/$name.json" --benchmark_out_format=json "$@"
done
echo "JSON written to results/"
