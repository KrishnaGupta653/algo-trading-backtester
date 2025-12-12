#include "../include/CSVParser.hpp"
#include "../include/TechnicalIndicators.hpp"
#include "../include/Backtester.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;
void printUsage(const char* programName) {
    cout << "Usage: " << programName << " <csv_file> [options]\n\n";
    cout << "Options:\n";
    cout << "  --short <n>        Short MA period (default: 50)\n";
    cout << "  --long <n>         Long MA period (default: 200)\n";
    cout << "  --capital <n>      Initial capital (default: 100000)\n";
    cout << "  --rsi              Enable RSI filter\n";
    cout << "  --ema              Use EMA instead of SMA\n";
    cout << "  --macd             Enable MACD confirmation\n";
    cout << "  --bollinger        Enable Bollinger Bands filter\n";
    cout << "  --stoploss <n>     Stop loss percentage (e.g., 0.05 for 5%)\n";
    cout << "  --takeprofit <n>   Take profit percentage (e.g., 0.15 for 15%)\n";
    cout << "  --commission <n>   Commission rate (default: 0.001 for 0.1%)\n";
    cout << "  --kelly            Use Kelly Criterion for position sizing\n";
    cout << "  --compare          Run strategy comparison across multiple MA periods\n";
    cout << "  --output <file>    Output results file (default: results.csv)\n";
    cout << "\nExamples:\n";
    cout << "  " << programName << " data/AAPL.csv\n";
    cout << "  " << programName << " data/AAPL.csv --short 20 --long 50 --ema\n";
    cout << "  " << programName << " data/AAPL.csv --stoploss 0.05 --takeprofit 0.15 --kelly\n";
    cout << "  " << programName << " data/AAPL.csv --compare\n";
}

void runStrategyComparison(const vector<OHLCV>& data, double capital) {
    cout << "\n=== STRATEGY COMPARISON ===\n";
    cout << "Testing multiple parameter combinations...\n\n";
    
    vector<StrategyParams> strategies = {
        {10, 30, "Aggressive"},
        {20, 50, "Medium-Fast"},
        {50, 200, "Golden Cross"},
        {100, 300, "Conservative"}
    };
    
    cout << left << setw(20) << "Strategy" 
              << right << setw(12) << "Return %" 
              << setw(10) << "Trades" 
              << setw(10) << "Sharpe"
              << setw(12) << "Max DD %\n";
    cout << string(64, '-') << "\n";
    
    for (const auto& strategy : strategies) {
        Backtester bt(data, strategy.shortMA, strategy.longMA, capital, false);
        bt.run();
        auto metrics = bt.calculateMetrics();
        
        cout << left << setw(20) << strategy.name 
                  << right << fixed << setprecision(1)
                  << setw(12) << metrics.totalReturn
                  << setw(10) << metrics.numTrades
                  << setw(10) << setprecision(2) << metrics.sharpeRatio
                  << setw(12) << setprecision(1) << metrics.maxDrawdown << "\n";
    }
    cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    string filename = argv[1];
    int shortMA = 50;
    int longMA = 200;
    double capital = 100000.0;
    bool useRSI = false;
    bool useEMA = false;
    bool useMACD = false;
    bool useBollinger = false;
    double stopLoss = 0.0;
    double takeProfit = 0.0;
    double commission = 0.001;
    bool useKelly = false;
    bool runComparison = false;
    string outputFile = "results.csv";
    
    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--short" && i + 1 < argc) {
            shortMA = stoi(argv[++i]);
        } else if (arg == "--long" && i + 1 < argc) {
            longMA = stoi(argv[++i]);
        } else if (arg == "--capital" && i + 1 < argc) {
            capital = stod(argv[++i]);
        } else if (arg == "--rsi") {
            useRSI = true;
        } else if (arg == "--ema") {
            useEMA = true;
        } else if (arg == "--macd") {
            useMACD = true;
        } else if (arg == "--bollinger") {
            useBollinger = true;
        } else if (arg == "--stoploss" && i + 1 < argc) {
            stopLoss = stod(argv[++i]);
        } else if (arg == "--takeprofit" && i + 1 < argc) {
            takeProfit = stod(argv[++i]);
        } else if (arg == "--commission" && i + 1 < argc) {
            commission = stod(argv[++i]);
        } else if (arg == "--kelly") {
            useKelly = true;
        } else if (arg == "--compare") {
            runComparison = true;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }
    
    // Print configuration
    cout << "=== Stock Backtesting System ===\n";
    cout << "Loading data from: " << filename << "\n";
    cout << "Strategy: " << (useEMA ? "EMA" : "SMA") << " Crossover (" 
              << shortMA << "/" << longMA << ")\n";
    cout << "Initial Capital: $" << fixed << setprecision(2) << capital << "\n";
    
    // Print enabled features
    cout << "\nEnabled Features:\n";
    if (useRSI) cout << "  ✓ RSI Filter\n";
    if (useMACD) cout << "  ✓ MACD Confirmation\n";
    if (useBollinger) cout << "  ✓ Bollinger Bands\n";
    if (stopLoss > 0) cout << "  ✓ Stop Loss: " << (stopLoss * 100) << "%\n";
    if (takeProfit > 0) cout << "  ✓ Take Profit: " << (takeProfit * 100) << "%\n";
    if (commission > 0) cout << "  ✓ Commission: " << (commission * 100) << "%\n";
    if (useKelly) cout << "  ✓ Kelly Criterion Position Sizing\n";
    
    try {
        // Load data
        auto data = CSVParser::parse(filename);
        cout << "\nLoaded " << data.size() << " trading days\n";
        cout << "Period: " << data.front().date << " to " << data.back().date << "\n";
        
        // Run comparison if requested
        if (runComparison) {
            runStrategyComparison(data, capital);
        }
        
        // Run main backtest
        Backtester bt(data, shortMA, longMA, capital, useRSI, useEMA, useMACD, 
                     useBollinger, stopLoss, takeProfit, commission, useKelly);
        bt.run();
        bt.printSummary();
        bt.exportResults(outputFile);
        
        cout << "\nResults exported to " << outputFile << "\n";
        
        // Print resume bullets
        cout << "\n=== RESUME BULLETS ===\n";
        cout << "• Engineered high-performance C++ backtesting engine processing 10+ years of historical stock data\n";
        if (useEMA) {
            cout << "• Optimized signal generation using EMA for reduced lag vs traditional SMA\n";
        }
        if (useMACD) {
            cout << "• Integrated MACD momentum indicator for multi-factor signal confirmation\n";
        }
        if (useBollinger) {
            cout << "• Applied Bollinger Bands for volatility-based entry/exit optimization\n";
        }
        if (stopLoss > 0 || takeProfit > 0) {
            cout << "• Implemented risk management with stop-loss and take-profit mechanisms\n";
        }
        if (commission > 0) {
            cout << "• Simulated realistic trading costs with commission-adjusted P&L calculation\n";
        }
        if (useKelly) {
            cout << "• Implemented Kelly Criterion for optimal position sizing based on win rate and risk\n";
        }
        if (runComparison) {
            cout << "• Conducted parameter optimization across multiple MA periods for strategy tuning\n";
        }
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}