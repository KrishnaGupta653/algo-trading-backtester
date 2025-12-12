#ifndef TECHNICALINDICATORS_HPP
#define TECHNICALINDICATORS_HPP

#include "types.hpp"
#include <vector>

class TechnicalIndicators {
public:
    // Simple Moving Average - O(n) with sliding window
    static std::vector<double> SMA(const std::vector<double>& prices, int period);
    
    // Exponential Moving Average - More responsive than SMA
    static std::vector<double> EMA(const std::vector<double>& prices, int period);
    
    // Relative Strength Index - Momentum indicator
    static std::vector<double> RSI(const std::vector<double>& prices, int period = 14);
    
    // MACD - Moving Average Convergence Divergence
    static MACDResult MACD(const std::vector<double>& prices, 
                           int fastPeriod = 12, 
                           int slowPeriod = 26, 
                           int signalPeriod = 9);
    
    // Standard Deviation - For volatility analysis
    static std::vector<double> StdDev(const std::vector<double>& prices, int period);
    
    // Bollinger Bands - Volatility bands
    static BollingerBands BollingerBand(const std::vector<double>& prices, 
                                        int period = 20, 
                                        double numStdDev = 2.0);
};

#endif // TECHNICALINDICATORS_HPP