#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

// OHLCV data structure for stock price data
struct OHLCV {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    double adjClose;
    long long volume;
};

// Trade structure to track individual trades
struct Trade {
    std::string entryDate;
    std::string exitDate;
    double entryPrice;
    double exitPrice;
    double shares;
    double pnl;
    double returnPct;
};

// Performance metrics for backtesting results
struct PerformanceMetrics {
    double totalReturn;
    double cagr;
    double maxDrawdown;
    double sharpeRatio;
    int numTrades;
    int winningTrades;
    double winRate;
    double avgWin;
    double avgLoss;
    double profitFactor;
};

// MACD indicator result structure
struct MACDResult {
    std::vector<double> macd;
    std::vector<double> signal;
    std::vector<double> histogram;
};

// Bollinger Bands result structure
struct BollingerBands {
    std::vector<double> upper;
    std::vector<double> middle;
    std::vector<double> lower;
};

// Strategy parameters for comparison
struct StrategyParams {
    int shortMA;
    int longMA;
    std::string name;
};

#endif // TYPES_HPP