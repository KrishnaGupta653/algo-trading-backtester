#ifndef BACKTESTER_HPP
#define BACKTESTER_HPP

#include "types.hpp"
#include <vector>
#include <string>

class Backtester {
private:
    std::vector<OHLCV> data;
    std::vector<Trade> trades;
    
    // Strategy parameters
    int shortPeriod;
    int longPeriod;
    double initialCapital;
    bool useRSI;
    bool useEMA;
    bool useMACD;
    bool useBollinger;
    
    // Risk management parameters
    double stopLossPercent;
    double takeProfitPercent;
    double commissionRate;
    
    // Position tracking
    double currentCash;
    double currentShares;
    bool inPosition;
    
    // Kelly Criterion
    bool useKellyCriterion;

public:
    Backtester(const std::vector<OHLCV>& d, 
               int shortMA, 
               int longMA,
               double capital, 
               bool rsi = false,
               bool ema = false,
               bool macd = false,
               bool bollinger = false,
               double stopLoss = 0.0,
               double takeProfit = 0.0,
               double commission = 0.001,
               bool kelly = false);
    
    // Run the backtest
    void run();
    
    // Calculate performance metrics
    PerformanceMetrics calculateMetrics() const;
    
    // Export results to file
    void exportResults(const std::string& filename) const;
    
    // Print summary to console
    void printSummary() const;
    
    // Get trades for analysis
    const std::vector<Trade>& getTrades() const { return trades; }

private:
    // Position management
    void enterPosition(size_t idx);
    void exitPosition(size_t idx);
    
    // Performance calculations
    double calculateMaxDrawdown() const;
    double calculateSharpeRatio() const;
    double calculateYears(const std::string& start, const std::string& end) const;
    
    // Kelly Criterion for position sizing
    double calculateKellyFraction() const;
    
    // Risk management checks
    bool checkStopLoss(size_t idx) const;
    bool checkTakeProfit(size_t idx) const;
};

#endif // BACKTESTER_HPP