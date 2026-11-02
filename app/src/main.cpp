#include "syslog_analyzer.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <syslog-file>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input) {
        std::cerr << "Cannot open: " << argv[1] << '\n';
        return 1;
    }

    std::cout << syslog::analyze_syslog(input);
}
