// Layout: how field order and alignment change a struct's size, and what that
// costs when you stream through an array of them.
// Ported from HFT-Prep/02-cpp/benchmarks/layout.cpp.
//
// Predict each number BEFORE running, then explain the results in README.md:
//   1. sizeof/alignof of Bad, Good, PerCore, Order (printed in the header).
//   2. Why is Order::price an integer, not a double?
//   3. Why would PerCore be used in an array indexed by thread id?
//      (Look up false sharing.)
//   4. BM_SumField: Bad vs Good at 4K elements (fits in L1) vs 4M elements
//      (main memory). Where does the gap appear, and why there?
#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

struct Bad {  // field order wastes space
    char a;
    double b;
    char c;
    int d;
};

struct Good {  // same fields, reordered
    double b;
    int d;
    char a;
    char c;
};

struct alignas(64) PerCore {  // one cache line each
    std::uint64_t counter;
};

struct Order {  // a realistic hot-path struct
    std::uint64_t id;
    std::int64_t price;  // fixed-point ticks, not double
    std::uint32_t qty;
    char side;  // 'B' / 'S'
};

template <typename T>
void add_layout_context(const char* name) {
    benchmark::AddCustomContext(std::string("sizeof/alignof ") + name,
                                std::to_string(sizeof(T)) + " / " + std::to_string(alignof(T)));
}

// Stream through n structs and sum one int field. The work per element is the
// same for Bad and Good; only the number of bytes the CPU must pull in differs.
template <typename T>
void BM_SumField(benchmark::State& state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    std::vector<T> v(n);
    for (std::size_t i = 0; i < n; ++i)
        v[i].d = static_cast<int>(i & 7);

    for (auto _ : state) {
        std::int64_t sum = 0;
        for (const T& x : v)
            sum += x.d;
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0) *
                            static_cast<std::int64_t>(sizeof(T)));
    state.counters["sizeof"] = static_cast<double>(sizeof(T));
}

BENCHMARK_TEMPLATE(BM_SumField, Bad)->Arg(1 << 12)->Arg(1 << 22);
BENCHMARK_TEMPLATE(BM_SumField, Good)->Arg(1 << 12)->Arg(1 << 22);

}  // namespace

int main(int argc, char** argv) {
    add_layout_context<Bad>("Bad");
    add_layout_context<Good>("Good");
    add_layout_context<PerCore>("PerCore");
    add_layout_context<Order>("Order");

    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv))
        return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    return 0;
}
