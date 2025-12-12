#ifndef CSVPARSER_HPP
#define CSVPARSER_HPP

#include "types.hpp"
#include <string>
#include <vector>

class CSVParser {
public:
    // Parse CSV file and return vector of OHLCV data
    static std::vector<OHLCV> parse(const std::string& filename);

private:
    // Parse a single line from CSV
    static OHLCV parseLine(const std::string& line);
    
    // Helper functions
    static double parseDouble(const std::string& s);
    static long long parseLong(const std::string& s);
    static void trim(std::string& s);
};

#endif // CSVPARSER_HPP