# lowlat-bench

[![CI](https://github.com/Shridhar2104/lowlat-bench/actions/workflows/ci.yml/badge.svg)](https://github.com/Shridhar2104/lowlat-bench/actions/workflows/ci.yml)

**What does low-latency C++ really cost?** A lab of small, focused microbenchmarks
(Google Benchmark, C++26) that measure the things trading-system code cares about:
memory layout, caches, allocation, dispatch, branches, and OS jitter.
Every number comes with an explanation of *why*.

## Build and run

Needs CMake 3.25+, Ninja and a C++26 compiler (GCC 14+ or Clang 18+).
Google Benchmark is fetched and built automatically.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bench/layout
./build/bench/cache_traversal
scripts/run_all.sh          # run everything, JSON to results/
```

Options: `-DLLB_NATIVE=ON` (tune for this CPU), `-DLLB_WERROR=ON` (warnings are errors, used in CI).

CI builds with GCC 16, GCC 14 and Clang 18, smoke-runs every benchmark and checks formatting
(`clang-format` 23.1.3, see `scripts/format.sh`). CI numbers are not results: shared runners are noisy.

## Benchmarks

| Binary | Question |
|---|---|
| `layout` | How do field order and `alignas` change `sizeof`, and what does a bigger struct cost when streaming an array? |
| `cache_traversal` | Same matrix, same sum: why is column-major traversal so much slower than row-major? |

## Results

### Machine

<!-- TODO: CPU model, cores, L1/L2/L3 sizes, OS + kernel, compiler + flags, VM or bare metal. -->

### layout

<!-- TODO (Day 4): paste the table, then explain every number:
     sizeof/alignof of each struct, why price is an integer, why PerCore is 64 bytes,
     and where Bad vs Good diverges in BM_SumField (4K vs 4M elements) and why. -->

### cache_traversal

<!-- TODO (Day 4): paste the table, then explain: the N where column-major falls off a cliff,
     how that maps to your cache sizes, and why row-major throughput stays flat. -->

## Method

<!-- TODO (Week 1): harness rules: DoNotOptimize / ClobberMemory, warm-up, repetitions,
     -O2 vs -O3, pinning to a core, CPU frequency scaling. -->

## Roadmap

**C++ costs**
- [ ] copy vs move (string, vector)
- [ ] dispatch: virtual vs CRTP vs `std::variant` + visit vs `std::function`
- [ ] containers: vector vs list vs deque vs map
- [ ] branch prediction: sorted vs unsorted, branchless, `[[likely]]`
- [ ] exceptions vs happy path; heap allocation in a hot loop

**Hardware**
- [x] struct layout and padding
- [x] row-major vs column-major traversal
- [ ] pointer-chasing latency vs working-set size (L1/L2/L3 cliffs)
- [ ] false sharing
- [ ] AoS vs SoA
- [ ] hardware counters (`perf stat`): cache misses, branch misses, IPC
- [ ] prefetching; bandwidth vs latency

## License

MIT
