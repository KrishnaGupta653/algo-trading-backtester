#include "../include/TechnicalIndicators.hpp"
#include <cmath>
#include <algorithm>

// Simple Moving Average - O(n) with sliding window
std::vector<double> TechnicalIndicators::SMA(const std::vector<double>& prices, int period) {
    std::vector<double> sma(prices.size(), 0.0);
    if (prices.size() < static_cast<size_t>(period)) return sma;
    
    double sum = 0.0;
    for (int i = 0; i < period; i++) {
        sum += prices[i];
    }
    sma[period - 1] = sum / period;
    
    for (size_t i = period; i < prices.size(); i++) {
        sum = sum - prices[i - period] + prices[i];
        sma[i] = sum / period;
    }
    
    return sma;
}

// Exponential Moving Average - More responsive than SMA
std::vector<double> TechnicalIndicators::EMA(const std::vector<double>& prices, int period) {
    std::vector<double> ema(prices.size(), 0.0);
    if (prices.size() < static_cast<size_t>(period)) return ema;
    
    // Initial SMA as first EMA value
    double sum = 0.0;
    for (int i = 0; i < period; i++) {
        sum += prices[i];
    }
    ema[period - 1] = sum / period;
    
    // Calculate multiplier
    double multiplier = 2.0 / (period + 1.0);
    
    // Calculate EMA
    for (size_t i = period; i < prices.size(); i++) {
        ema[i] = (prices[i] - ema[i - 1]) * multiplier + ema[i - 1];
    }
    
    return ema;
}

// Relative Strength Index - O(n)
std::vector<double> TechnicalIndicators::RSI(const std::vector<double>& prices, int period) {
    std::vector<double> rsi(prices.size(), 50.0);
    if (prices.size() < static_cast<size_t>(period + 1)) return rsi;
    
    double avgGain = 0.0, avgLoss = 0.0;
    
    // Initial calculation
    for (int i = 1; i <= period; i++) {
        double change = prices[i] - prices[i - 1];
        if (change > 0) avgGain += change;
        else avgLoss += -change;
    }
    avgGain /= period;
    avgLoss /= period;
    
    double rs = (avgLoss == 0.0) ? 100.0 : avgGain / avgLoss;
    rsi[period] = 100.0 - (100.0 / (1.0 + rs));
    
    // Smoothed calculation
    for (size_t i = period + 1; i < prices.size(); i++) {
        double change = prices[i] - prices[i - 1];
        double gain = (change > 0) ? change : 0.0;
        double loss = (change < 0) ? -change : 0.0;
        
        avgGain = (avgGain * (period - 1) + gain) / period;
        avgLoss = (avgLoss * (period - 1) + loss) / period;
        
        rs = (avgLoss == 0.0) ? 100.0 : avgGain / avgLoss;
        rsi[i] = 100.0 - (100.0 / (1.0 + rs));
    }
    
    return rsi;
}

// MACD - Moving Average Convergence Divergence
MACDResult TechnicalIndicators::MACD(const std::vector<double>& prices, 
                                     int fastPeriod, 
                                     int slowPeriod, 
                                     int signalPeriod) {
    MACDResult result;
    
    auto fastEMA = EMA(prices, fastPeriod);
    auto slowEMA = EMA(prices, slowPeriod);
    
    // MACD line = fast EMA - slow EMA
    result.macd.resize(prices.size(), 0.0);
    for (size_t i = 0; i < prices.size(); i++) {
        result.macd[i] = fastEMA[i] - slowEMA[i];
    }
    
    // Signal line = EMA of MACD
    result.signal = EMA(result.macd, signalPeriod);
    
    // Histogram = MACD - Signal
    result.histogram.resize(prices.size(), 0.0);
    for (size_t i = 0; i < prices.size(); i++) {
        result.histogram[i] = result.macd[i] - result.signal[i];
    }
    
    return result;
}

// Standard Deviation for volatility analysis
std::vector<double> TechnicalIndicators::StdDev(const std::vector<double>& prices, int period) {
    std::vector<double> stddev(prices.size(), 0.0);
    auto sma = SMA(prices, period);
    
    for (size_t i = period - 1; i < prices.size(); i++) {
        double sum = 0.0;
        for (int j = 0; j < period; j++) {
            double diff = prices[i - j] - sma[i];
            sum += diff * diff;
        }
        stddev[i] = std::sqrt(sum / period);
    }
    
    return stddev;
}

// Bollinger Bands - Volatility bands
BollingerBands TechnicalIndicators::BollingerBand(const std::vector<double>& prices, 
                                                   int period, 
                                                   double numStdDev) {
    BollingerBands bb;
    bb.middle = SMA(prices, period);
    auto stddev = StdDev(prices, period);
    
    bb.upper.resize(prices.size());
    bb.lower.resize(prices.size());
    
    for (size_t i = 0; i < prices.size(); i++) {
        bb.upper[i] = bb.middle[i] + numStdDev * stddev[i];
        bb.lower[i] = bb.middle[i] - numStdDev * stddev[i];
    }
    
    return bb;
}