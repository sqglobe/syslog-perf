#include "syslog_analyzer.hpp"

#include <iostream>
#include <sstream>

int main()
{
    std::istringstream input(
        "<34>Oct 27 09:00:01 web01 sshd[2178]: Accepted publickey\n"
        "<34>Oct 27 09:00:02 web01 sshd[2178]: Accepted publickey\n"
        "<30>Oct 27 09:00:03 web01 systemd[1]: Started\n"
        "Oct 27 09:00:04 web01 kernel: unknown priority\n"
        "<200>Oct 27 09:00:05 web01 sshd[2178]: invalid PRI\n"
        "<34>Oct 27 09:00:06 web01 sshd[9999999999999]: invalid PID\n"
        "malformed\n");

    const auto stats = syslog::analyze_syslog(input);
    const bool ok =
        stats.messages == 4 && stats.skipped == 3 &&
        stats.severity_counts[2] == 2 &&
        stats.severity_counts[6] == 1 &&
        stats.severity_counts[8] == 1 &&
        stats.processes.size() == 3 &&
        stats.processes[0].process == "sshd" &&
        stats.processes[0].pid == 2178 &&
        stats.processes[0].count == 2 &&
        stats.processes[2].pid == -1;

    if (!ok) {
        std::cerr << "Unexpected syslog statistics\n";
        return 1;
    }
}
