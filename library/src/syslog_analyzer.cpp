#include "syslog_analyzer.hpp"

#include <algorithm>
#include <iomanip>
#include <istream>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>

namespace syslog {

SyslogStats analyze_syslog(std::istream& input)
{
    SyslogStats stats;
    std::string line;

    const std::regex pattern(
            R"(^(?:<([0-9]{1,3})>)?[A-Z][a-z]{2} +[0-9]{1,2} +[0-9]{2}:[0-9]{2}:[0-9]{2} +[^ ]+ +([^ :\[]+)(?:\[([0-9]+)\])?:)"
        );

    while (std::getline(input, line)) {
        

        std::smatch match;
        if (!std::regex_search(line, match, pattern)) {
            ++stats.skipped;
            continue;
        }

        const std::string process = match[2].str();
        int pid = -1;
        int priority = -1;
        try {
            if (match[3].matched) {
                pid = std::stoi(match[3].str());
            }
            if (match[1].matched) {
                priority = std::stoi(match[1].str());
            }
        } catch (const std::exception&) {
            ++stats.skipped;
            continue;
        }

        if (priority > 191) { // Valid syslog PRI range: 0..191.
            ++stats.skipped;
            continue;
        }

        const int severity = priority == -1 ? unknown_severity : priority % 8;
        ++stats.messages;
        ++stats.severity_counts[static_cast<std::size_t>(severity)];

        auto found = std::find_if(
            stats.processes.begin(), stats.processes.end(),
            [&process, pid, severity](const auto& entry) {
            return entry.pid == pid &&
                   entry.severity == severity && 
                   entry.process == process;
            });

        if (found != stats.processes.end()) {
            ++found->count;
        } else {
            stats.processes.push_back({process, pid, severity, 1});
        }
    }
    return stats;
}

std::ostream& operator<<(std::ostream& output, const SyslogStats& stats)
{
    static constexpr const char* names[] = {
        "emerg", "alert", "crit", "err", "warning", "notice",
        "info", "debug", "unknown"
    };

    output << "Messages: " << stats.messages << '\n'
           << "Skipped lines: " << stats.skipped << "\n\n"
           << "Messages by severity:\n";
    for (int level = 0; level <= unknown_severity; ++level) {
        output << "  " << std::left << std::setw(8) << names[level]
               << stats.severity_counts[static_cast<std::size_t>(level)] << '\n';
    }

    output << "\n" << std::left
           << std::setw(20) << "PROCESS" << std::setw(12) << "PID"
           << std::setw(12) << "SEVERITY" << "COUNT\n";
    for (const auto& entry : stats.processes) {
        output << std::left << std::setw(20) << entry.process;
        if (entry.pid == -1) {
            output << std::setw(12) << "-";
        } else {
            output << std::setw(12) << entry.pid;
        }
        output << std::setw(12) << names[entry.severity]
               << entry.count << '\n';
    }
    return output;
}

} // namespace syslog
