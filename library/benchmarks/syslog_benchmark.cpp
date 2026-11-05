#include "syslog_analyzer.hpp"

#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>

namespace {

struct BenchmarkInput {
    std::string contents;
    std::int64_t rows;
};

BenchmarkInput load_benchmark_input()
{
    std::ifstream file(SYSLOG_BENCHMARK_INPUT, std::ios::binary);
    if (!file) {
        throw std::runtime_error(
            "Cannot open benchmark input: " SYSLOG_BENCHMARK_INPUT);
    }

    std::string contents{
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    const auto rows = static_cast<std::int64_t>(
        std::count(contents.begin(), contents.end(), '\n'));
    return {std::move(contents), rows};
}

void BM_analyze_syslog(benchmark::State& state)
{
    static const BenchmarkInput fixture = load_benchmark_input();
    std::istringstream input(fixture.contents);

    for (auto _ : state) {
        input.clear();
        input.seekg(0);
        auto stats = syslog::analyze_syslog(input);
        benchmark::DoNotOptimize(stats);
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) *
                            fixture.rows);
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) *
                            static_cast<std::int64_t>(fixture.contents.size()));
}

} // namespace

BENCHMARK(BM_analyze_syslog);
BENCHMARK_MAIN();
