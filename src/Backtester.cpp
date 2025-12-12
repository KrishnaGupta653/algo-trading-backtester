#include "../include/Backtester.hpp"
#include "../include/TechnicalIndicators.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <algorithm>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
using namespace std;
Backtester::Backtester(const vector<OHLCV>& d,int shortMA, 
                       int longMA,
                       double capital, 
                       bool rsi,
                       bool ema,
                       bool macd,
                       bool bollinger,
                       double stopLoss,
                       double takeProfit,
                       double commission,
                       bool kelly)
    : data(d), shortPeriod(shortMA), longPeriod(longMA),
      initialCapital(capital), useRSI(rsi), useEMA(ema), 
      useMACD(macd), useBollinger(bollinger),
      stopLossPercent(stopLoss), takeProfitPercent(takeProfit),
      commissionRate(commission),
      currentCash(capital), currentShares(0.0), inPosition(false),
      useKellyCriterion(kelly) {}

void Backtester::run() {
    if (data.size() < static_cast<size_t>(longPeriod + 1)) {
        cerr << "Insufficient data for backtesting\n";
        return;
    }
    
    // Extract close prices
    vector<double> closes;
    for (const auto& bar : data) {
        closes.push_back(bar.close);
    }
    
    // Compute indicators based on settings
    vector<double> shortMA, longMA;
    if (useEMA) {
        shortMA = TechnicalIndicators::EMA(closes, shortPeriod);
        longMA = TechnicalIndicators::EMA(closes, longPeriod);
    } else {
        shortMA = TechnicalIndicators::SMA(closes, shortPeriod);
        longMA = TechnicalIndicators::SMA(closes, longPeriod);
    }
    
    auto rsi = useRSI ? TechnicalIndicators::RSI(closes, 14) : vector<double>();
    
    MACDResult macdData;
    if (useMACD) {
        macdData = TechnicalIndicators::MACD(closes);
    }
    
    BollingerBands bb;
    if (useBollinger) {
        bb = TechnicalIndicators::BollingerBand(closes);
    }
    
    // Generate signals and execute trades
    for (size_t i = longPeriod; i < data.size(); i++) {
        // Check risk management if in position
        if (inPosition) {
            // Stop loss check
            if (checkStopLoss(i)) {
                exitPosition(i);
                continue;
            }
            
            // Take profit check
            if (checkTakeProfit(i)) {
                exitPosition(i);
                continue;
            }
        }
        
        // Generate entry/exit signals
        bool entrySignal = false;
        bool exitSignal = false;
        
        // Primary signal: MA crossover
        if (i > 0) {
            bool currentCross = shortMA[i] > longMA[i];
            bool previousCross = shortMA[i-1] > longMA[i-1];
            
            if (currentCross && !previousCross) {
                entrySignal = true;
            } else if (!currentCross && previousCross) {
                exitSignal = true;
            }
        }
        
        // RSI filter (optional)
        if (useRSI && entrySignal) {
            if (rsi[i] >= 70) entrySignal = false; // Overbought
        }
        
        // MACD confirmation (optional)
        if (useMACD && entrySignal) {
            if (macdData.histogram[i] <= 0) entrySignal = false;
        }
        
        // Bollinger Bands filter (optional)
        if (useBollinger && entrySignal) {
            if (closes[i] > bb.upper[i]) entrySignal = false; // Price too high
        }
        
        // Execute trades
        if (entrySignal && !inPosition) {
            enterPosition(i);
        } else if (exitSignal && inPosition) {
            exitPosition(i);
        }
    }
    
    // Close any open position at the end
    if (inPosition) {
        exitPosition(data.size() - 1);
    }
}

void Backtester::enterPosition(size_t idx) {
    double entryPrice = (idx + 1 < data.size() && data[idx + 1].open > 0)
                            ? data[idx + 1].open
                            : data[idx].close;
    
    // Apply commission
    double commission = currentCash * commissionRate;
    double availableCash = currentCash - commission;
    
    // Calculate position size
    double positionFraction = 1.0;
    if (useKellyCriterion && trades.size() >= 5) {
        positionFraction = calculateKellyFraction();
    }
    
    currentShares = (availableCash * positionFraction) / entryPrice;
    currentCash = 0.0;
    inPosition = true;
    
    Trade t;
    t.entryDate = data[idx].date;
    t.entryPrice = entryPrice;
    t.shares = currentShares;
    trades.push_back(t);
}

void Backtester::exitPosition(size_t idx) {
    double exitPrice = (idx + 1 < data.size() && data[idx + 1].open > 0)
                           ? data[idx + 1].open
                           : data[idx].close;
    
    double grossProceeds = currentShares * exitPrice;
    double commission = grossProceeds * commissionRate;
    currentCash = grossProceeds - commission;
    currentShares = 0.0;
    inPosition = false;
    
    Trade& t = trades.back();
    t.exitDate = data[idx].date;
    t.exitPrice = exitPrice;
    t.pnl = currentCash - (t.shares * t.entryPrice);
    t.returnPct = (t.pnl / (t.shares * t.entryPrice)) * 100.0;
}

bool Backtester::checkStopLoss(size_t idx) const {
    if (stopLossPercent <= 0 || trades.empty()) return false;
    
    double currentPrice = data[idx].close;
    double entryPrice = trades.back().entryPrice;
    double pnlPercent = (currentPrice - entryPrice) / entryPrice;
    
    return pnlPercent <= -stopLossPercent;
}

bool Backtester::checkTakeProfit(size_t idx) const {
    if (takeProfitPercent <= 0 || trades.empty()) return false;
    
    double currentPrice = data[idx].close;
    double entryPrice = trades.back().entryPrice;
    double pnlPercent = (currentPrice - entryPrice) / entryPrice;
    
    return pnlPercent >= takeProfitPercent;
}

double Backtester::calculateKellyFraction() const {
    if (trades.size() < 5) return 1.0;
    
    int wins = 0;
    double totalWinAmount = 0.0, totalLossAmount = 0.0;
    
    for (const auto& t : trades) {
        if (t.pnl > 0) {
            wins++;
            totalWinAmount += t.returnPct;
        } else {
            totalLossAmount += -t.returnPct;
        }
    }
    
    if (wins == 0 || wins == static_cast<int>(trades.size())) return 1.0;
    
    double winRate = wins / static_cast<double>(trades.size());
    double avgWin = totalWinAmount / wins;
    double avgLoss = totalLossAmount / (trades.size() - wins);
    
    if (avgLoss == 0.0) return 1.0;
    
    // Kelly = W - (1-W)/R where W=win rate, R=win/loss ratio
    double kelly = winRate - (1.0 - winRate) / (avgWin / avgLoss);
    
    // Use fractional Kelly (half Kelly for safety)
    return max(0.0, min(kelly * 0.5, 1.0));
}

PerformanceMetrics Backtester::calculateMetrics() const {
    PerformanceMetrics m;
    m.numTrades = trades.size();
    
    double finalValue = currentCash + (inPosition ? currentShares * data.back().close : 0.0);
    m.totalReturn = ((finalValue - initialCapital) / initialCapital) * 100.0;
    
    // CAGR calculation
    string firstDate = data.front().date;
    string lastDate = data.back().date;
    double years = calculateYears(firstDate, lastDate);
    m.cagr = (pow(finalValue / initialCapital, 1.0 / years) - 1.0) * 100.0;
    
    // Max Drawdown
    m.maxDrawdown = calculateMaxDrawdown();
    
    // Trade statistics
    m.winningTrades = 0;
    double totalWin = 0.0, totalLoss = 0.0;
    for (const auto& t : trades) {
        if (t.pnl > 0) {
            m.winningTrades++;
            totalWin += t.pnl;
        } else {
            totalLoss += -t.pnl;
        }
    }
    m.winRate = m.numTrades > 0 ? (m.winningTrades * 100.0 / m.numTrades) : 0.0;
    m.avgWin = m.winningTrades > 0 ? totalWin / m.winningTrades : 0.0;
    m.avgLoss = (m.numTrades - m.winningTrades) > 0 ? totalLoss / (m.numTrades - m.winningTrades) : 0.0;
    m.profitFactor = totalLoss > 0 ? totalWin / totalLoss : (totalWin > 0 ? 999.99 : 0.0);
    
    // Sharpe Ratio
    m.sharpeRatio = calculateSharpeRatio();
    
    return m;
}

double Backtester::calculateMaxDrawdown() const {
    if (data.empty()) return 0.0;
    
    double peak = initialCapital;
    double maxDD = 0.0;
    double equity = initialCapital;
    
    size_t tradeIdx = 0;
    bool holding = false;
    double entryPrice = 0.0;
    double shares = 0.0;
    
    for (size_t i = longPeriod; i < data.size(); i++) {
        if (tradeIdx < trades.size()) {
            if (!holding && data[i].date == trades[tradeIdx].entryDate) {
                holding = true;
                entryPrice = trades[tradeIdx].entryPrice;
                shares = trades[tradeIdx].shares;
                equity = shares * entryPrice;
            }
            
            if (holding) {
                equity = shares * data[i].close;
                
                if (data[i].date == trades[tradeIdx].exitDate) {
                    holding = false;
                    equity = shares * trades[tradeIdx].exitPrice;
                    tradeIdx++;
                }
            }
        }
        
        if (equity > peak) peak = equity;
        double dd = ((peak - equity) / peak) * 100.0;
        if (dd > maxDD) maxDD = dd;
    }
    
    return maxDD;
}

double Backtester::calculateSharpeRatio() const {
    if (trades.empty()) return 0.0;
    
    vector<double> returns;
    for (const auto& t : trades) {
        returns.push_back(t.returnPct / 100.0);
    }
    
    double mean = accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    double variance = 0.0;
    for (double r : returns) {
        variance += (r - mean) * (r - mean);
    }
    variance /= returns.size();
    double stdDev = sqrt(variance);
    
    if (stdDev == 0.0) return 0.0;
    
    // Annualized Sharpe
    double sharpe = (mean / stdDev) * sqrt(252.0 / (data.size() / static_cast<double>(trades.size())));
    return sharpe;
}

double Backtester::calculateYears(const string& start, const string& end) const {
    int startYear = stoi(start.substr(0, 4));
    int endYear = stoi(end.substr(0, 4));
    double years = endYear - startYear;
    return years > 0 ? years : 1.0;
}

void Backtester::exportResults(const string& filename) const {
      // Create results directory if it doesn't exist
    #ifdef _WIN32
        _mkdir("results");
    #else
        mkdir("results", 0777);
    #endif
    ofstream file(filename);
    
    file << "BACKTEST SUMMARY\n";
    file << "================\n\n";
    
    auto metrics = calculateMetrics();
    
    file << "Initial Capital,$" << fixed << setprecision(2) << initialCapital << "\n";
    double finalValue = currentCash + (inPosition ? currentShares * data.back().close : 0.0);
    file << "Final Value,$" << finalValue << "\n";
    file << "Total Return," << setprecision(2) << metrics.totalReturn << "%\n";
    file << "CAGR," << metrics.cagr << "%\n";
    file << "Max Drawdown," << metrics.maxDrawdown << "%\n";
    file << "Sharpe Ratio," << setprecision(3) << metrics.sharpeRatio << "\n";
    file << "Number of Trades," << metrics.numTrades << "\n";
    file << "Winning Trades," << metrics.winningTrades << "\n";
    file << "Win Rate," << setprecision(2) << metrics.winRate << "%\n";
    file << "Average Win,$" << metrics.avgWin << "\n";
    file << "Average Loss,$" << metrics.avgLoss << "\n";
    file << "Profit Factor," << metrics.profitFactor << "\n\n";
    
    file << "TRADE LOG\n";
    file << "=========\n";
    file << "Entry Date,Exit Date,Entry Price,Exit Price,Shares,P&L,Return %\n";
    
    for (const auto& t : trades) {
        file << t.entryDate << "," << t.exitDate << ","
             << fixed << setprecision(2)
             << t.entryPrice << "," << t.exitPrice << ","
             << setprecision(4) << t.shares << ","
             << setprecision(2) << t.pnl << ","
             << t.returnPct << "%\n";
    }
    
    file.close();
}

void Backtester::printSummary() const {
    auto metrics = calculateMetrics();
    
    cout << "\n=== BACKTEST RESULTS ===\n";
    cout << fixed << setprecision(2);
    cout << "Initial Capital: $" << initialCapital << "\n";
    double finalValue = currentCash + (inPosition ? currentShares * data.back().close : 0.0);
    cout << "Final Value: $" << finalValue << "\n";
    cout << "Total Return: " << metrics.totalReturn << "%\n";
    cout << "CAGR: " << metrics.cagr << "%\n";
    cout << "Max Drawdown: " << metrics.maxDrawdown << "%\n";
    cout << "Sharpe Ratio: " << setprecision(3) << metrics.sharpeRatio << "\n";
    cout << "Trades: " << metrics.numTrades << " (" << metrics.winningTrades
              << " wins, " << setprecision(1) << metrics.winRate << "% win rate)\n";
    cout << "Profit Factor: " << setprecision(2) << metrics.profitFactor << "\n";
}