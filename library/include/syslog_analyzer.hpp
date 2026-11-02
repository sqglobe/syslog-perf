#pragma once

#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace syslog {

// Severity 0..7 follows syslog PRI % 8; 8 means that PRI is absent.
inline constexpr int unknown_severity = 8;

struct ProcessStats {
    std::string process;
    int pid = -1; // No PID in the syslog line.
    int severity = unknown_severity;
    std::uint64_t count = 0;
};

struct SyslogStats {
    std::uint64_t messages = 0;
    std::uint64_t skipped = 0;
    std::array<std::uint64_t, 9> severity_counts{};
    std::vector<ProcessStats> processes;
};

// Educational performance exercise: intentionally inefficient implementation.
// Accepts traditional RFC 3164-style Linux syslog lines, with optional <PRI>.
SyslogStats analyze_syslog(std::istream& input);

// Output is separate so the benchmark measures only analyze_syslog().
std::ostream& operator<<(std::ostream& output, const SyslogStats& stats);

} // namespace syslog
