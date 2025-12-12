#include "../include/CSVParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
using namespace std;
vector<OHLCV> CSVParser::parse(const string& filename) {
    vector<OHLCV> data;
    ifstream file(filename);
    
    if (!file.is_open()) {
        throw runtime_error("Cannot open file: " + filename);
    }
    
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        OHLCV row = parseLine(line);
        data.push_back(row);
    }
    
    return data;
}

OHLCV CSVParser::parseLine(const string& line) {
    OHLCV row;
    stringstream ss(line);
    string token;
    int col = 0;
    
    while (getline(ss, token, ',')) {
        trim(token);
        switch (col) {
            case 0: row.date = token; break;
            case 1: row.open = parseDouble(token); break;
            case 2: row.high = parseDouble(token); break;
            case 3: row.low = parseDouble(token); break;
            case 4: row.close = parseDouble(token); break;
            case 5: row.adjClose = parseDouble(token); break;
            case 6: row.volume = parseLong(token); break;
        }
        col++;
    }
    
    return row;
}

double CSVParser::parseDouble(const string& s) {
    try {
        return s.empty() ? 0.0 : stod(s);
    } catch (...) {
        return 0.0;
    }
}

long long CSVParser::parseLong(const string& s) {
    try {
        return s.empty() ? 0LL : stoll(s);
    } catch (...) {
        return 0LL;
    }
}

void CSVParser::trim(string& s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}