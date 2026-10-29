// Row-major vs column-major traversal of the same N x N matrix.
// Same arithmetic, very different speed. Why? (cache lines + prefetcher)
// Ported from HFT-Prep/02-cpp/benchmarks/cache_traversal.cpp.
//
// Predict BEFORE running, then explain in README.md:
//   1. At which N does column-major start losing badly, and why that N?
//      (Compare N*N*4 bytes with your L1/L2/L3 sizes, printed in the header.)
//   2. Row-major bytes_per_second barely changes with N. Why?
//   3. Try N = 8192 locally: --benchmark_filter=/8192 after adding the Arg.
#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

std::vector<int> make_matrix(std::size_t n) { return std::vector<int>(n * n, 1); }

void BM_RowMajor(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto m = make_matrix(n);
    for (auto _ : state) {
        std::int64_t sum = 0;
        for (std::size_t r = 0; r < n; ++r)
            for (std::size_t c = 0; c < n; ++c)
                sum += m[r * n + c];
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n * n * sizeof(int)));
}

void BM_ColumnMajor(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto m = make_matrix(n);
    for (auto _ : state) {
        std::int64_t sum = 0;
        for (std::size_t c = 0; c < n; ++c)
            for (std::size_t r = 0; r < n; ++r)
                sum += m[r * n + c];
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(n * n * sizeof(int)));
}

BENCHMARK(BM_RowMajor)->RangeMultiplier(2)->Range(256, 4096)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_ColumnMajor)->RangeMultiplier(2)->Range(256, 4096)->Unit(benchmark::kMicrosecond);

}  // namespace

BENCHMARK_MAIN();
