# Syslog + Linux perf + Google Benchmark

One deliberately slow  function, `syslog::analyze_syslog(std::istream&)`,
implemented in a **static library** `syslog_analyzer`. The function reads
traditional syslog records with optional `<PRI>`, counts messages and groups
by **process name + PID + severity**. The main program only opens the file,
calls the function and prints the result.

## Layout

```
library/include/syslog_analyzer.hpp       Public API and result types
library/src/syslog_analyzer.cpp           Slow function and output formatting
library/tests/syslog_tests.cpp            Fast correctness smoke test
library/benchmarks/syslog_benchmark.cpp   Google Benchmark
library/benchmarks/large.log              Fixed benchmark input
app/src/main.cpp                          Minimal CLI
logs/sample.log                           Traditional syslog sample
cmake/benchmark.cmake                     Google Benchmark dependency setup
CMakeLists.txt                            Top-level build orchestration
library/CMakeLists.txt                    Library, test, and benchmark targets
app/CMakeLists.txt                        CLI target
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
```

This enables Google Benchmark by default. CMake always fetches the pinned
Google Benchmark v1.9.5 release for a fresh build tree instead of using a
system-installed package; configuration therefore requires network access.
To build the app and tests **offline** without the benchmark dependency:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DSYSLOG_BUILD_BENCHMARKS=OFF
cmake --build build -j
```

## Run

```bash
./build/syslog_perf logs/sample.log
ctest --test-dir build --output-on-failure
```

If a traditional syslog message doesn't include `<PRI>`, the severity is
`unknown`; `PRI % 8` only makes sense when `PRI` is present. If `[PID]` is
missing, PID is printed as `-`. Invalid or unsupported lines are counted as
skipped. The parser intentionally accepts only the subset of RFC 3164 shown
in `logs/sample.log` and the generator, not the full syslog ecosystem (RFC
5424, year/timestamp variants, journald JSON, etc.).

## Google Benchmark

```bash
./build/syslog_bench --benchmark_repetitions=3 --benchmark_min_time=0.2s
./build/syslog_bench --benchmark_out=results.json --benchmark_out_format=json
```

The benchmark reads `library/benchmarks/large.log` once, outside the measured
loop, and reuses a memory-backed stream. Each iteration calls the same
`syslog::analyze_syslog()` function. The timed region includes parsing,
aggregation and in-memory stream rewind, but **not** fixture file I/O or console
printing. `SetItemsProcessed` reports processed messages.

## perf

```bash
python3 scripts/generate_load.py --lines 25000 --output build/large.log
./build/syslog_perf build/large.log > /dev/null   # warm page cache
perf stat -d -- ./build/syslog_perf build/large.log > /dev/null
perf record --call-graph fp -F 199 -- ./build/syslog_perf build/large.log > /dev/null
perf report
perf annotate

# You can also profile the Google Benchmark target itself:
perf record --call-graph fp -F 199 -- ./build/syslog_bench \
    --benchmark_filter='BM_analyze_syslog' --benchmark_min_time=2s
```

The `-fno-omit-frame-pointer` option is set on both the **library** and
our executables to improve stack unwinding. `RelWithDebInfo` preserves debug symbols while
optimizing code. If perf reports permission errors, check Linux's
`kernel.perf_event_paranoid` setting and your environment's privileges.

## Deliberate inefficiencies (exercise for readers)

* `std::regex` construction **on every line**.
* regex matching and temporary strings for fields.
* numeric parsing via `std::stoi` and string temporaries.
* linear search in `std::vector<ProcessStats>` for every message.
* potential vector reallocations and string copies.

Do not optimize benchmark code instead of the library: edit
`library/src/syslog_analyzer.cpp` and rerun benchmarks / perf / tests. Validate
correctness after each optimization.
