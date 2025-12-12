# Trading Backtester - High-Performance C++ Stock Analysis System

A professional-grade backtesting engine for algorithmic trading strategies with advanced technical indicators, risk management, and performance analytics.

## 🚀 Features

### Core Functionality

- **Fast CSV Parsing**: Efficient O(n) parsing of historical stock data
- **Multiple Technical Indicators**:
  - Simple Moving Average (SMA)
  - Exponential Moving Average (EMA)
  - Relative Strength Index (RSI)
  - MACD (Moving Average Convergence Divergence)
  - Bollinger Bands
- **Strategy Execution**: MA crossover with multi-indicator confirmation
- **Risk Management**: Stop-loss and take-profit mechanisms
- **Position Sizing**: Kelly Criterion for optimal capital allocation
- **Transaction Costs**: Realistic commission modeling
- **Performance Metrics**: Total return, CAGR, max drawdown, Sharpe ratio

### Advanced Features

- EMA-based strategies for reduced lag
- MACD confirmation for entry signals
- Bollinger Bands volatility filtering
- RSI overbought/oversold filtering
- Automated stop-loss and take-profit
- Kelly Criterion position sizing
- Strategy comparison across multiple parameters
- Detailed trade logging and analysis

## 📁 Project Structure

```
trading_backtester/
│
├── src/
│   ├── main.cpp                    # Main entry point with CLI
│   ├── CSVParser.cpp               # CSV data parsing
│   ├── TechnicalIndicators.cpp     # All technical indicators
│   └── Backtester.cpp              # Core backtesting engine
│
├── include/
│   ├── types.hpp                   # Data structures
│   ├── CSVParser.hpp               # CSV parser header
│   ├── TechnicalIndicators.hpp     # Indicators header
│   └── Backtester.hpp              # Backtester header
│
├── data/
│   └── (CSV files go here)
│
├── scripts/
│   └── download_stock_data.py      # Stock data downloader
│
├── build/
│   └── (compiled binaries)
│
├── results/
│   └── (backtest results)
│
├── Makefile                        # Build configuration
├── CMakeLists.txt                  # CMake configuration
└── README.md                       # This file
```

## 🛠️ Installation & Setup

### Prerequisites

- C++17 compatible compiler (g++ 7+, clang++ 5+)
- Make or CMake
- Python 3.6+ (for data download)
- yfinance library: `pip install yfinance pandas`

### Build Instructions

#### Option 1: Using Make

```bash
# Build the project
make

# Build and run with default settings
make run

# Build and run with advanced features
make run-advanced

# Run strategy comparison
make compare

# Clean build artifacts
make clean
```

#### Option 2: Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run
./backtester ../data/AAPL.csv
```

#### Option 3: Direct Compilation

```bash
g++ -std=c++17 -O3 -Wall -Wextra \
    src/main.cpp \
    src/CSVParser.cpp \
    src/TechnicalIndicators.cpp \
    src/Backtester.cpp \
    -Iinclude -o build/backtester
```

## 📊 Downloading Stock Data

The project includes a Python script to download historical stock data from Yahoo Finance.

### Quick Start

```bash
# Download 10 years of Apple stock data
python3 scripts/download_stock_data.py AAPL --output data/

# Download Indian stocks (NSE)
python3 scripts/download_stock_data.py RELIANCE.NS TCS.NS INFY.NS --output data/

# Download preset lists
python3 scripts/download_stock_data.py --preset indian_top10 --output data/
python3 scripts/download_stock_data.py --preset us_tech --output data/

# Custom time period
python3 scripts/download_stock_data.py TSLA --years 5 --output data/
```

### Available Shortcuts

- **Indian Stocks**: `reliance`, `tcs`, `infy`, `hdfcbank`, `icicibank`, etc.
- **US Stocks**: `apple`, `microsoft`, `google`, `amazon`, `tesla`, etc.
- **Presets**: `indian_top10`, `us_top10`, `indian_it`, `indian_banks`, `us_tech`

## 🎯 Usage Examples

### Basic Usage

```bash
# Run with default settings (50/200 SMA, $100k capital)
./build/backtester data/AAPL.csv

# Custom MA periods
./build/backtester data/AAPL.csv --short 20 --long 50

# Different initial capital
./build/backtester data/AAPL.csv --capital 50000
```

### Advanced Features

#### Use EMA Instead of SMA

```bash
./build/backtester data/AAPL.csv --ema
```

#### Enable Risk Management

```bash
# 5% stop-loss, 15% take-profit
./build/backtester data/AAPL.csv --stoploss 0.05 --takeprofit 0.15
```

#### Add Technical Indicators

```bash
# RSI filter (avoid overbought entries)
./build/backtester data/AAPL.csv --rsi

# MACD confirmation
./build/backtester data/AAPL.csv --macd

# Bollinger Bands filter
./build/backtester data/AAPL.csv --bollinger

# Combine multiple indicators
./build/backtester data/AAPL.csv --ema --rsi --macd --bollinger
```

#### Kelly Criterion Position Sizing

```bash
./build/backtester data/AAPL.csv --kelly
```

#### Include Transaction Costs

```bash
# 0.1% commission per trade
./build/backtester data/AAPL.csv --commission 0.001
```

#### Full-Featured Strategy

```bash
./build/backtester data/AAPL.csv \
    --short 20 --long 50 \
    --ema --macd \
    --stoploss 0.05 --takeprofit 0.15 \
    --commission 0.001 \
    --kelly \
    --output results/my_strategy.csv
```

#### Strategy Comparison

```bash
# Compare multiple MA combinations
./build/backtester data/AAPL.csv --compare
```

## 📈 Output

### Console Output

```
=== Stock Backtesting System ===
Loading data from: data/AAPL.csv
Strategy: EMA Crossover (20/50)
Initial Capital: $100000.00

Enabled Features:
  ✓ MACD Confirmation
  ✓ Stop Loss: 5%
  ✓ Take Profit: 15%
  ✓ Commission: 0.1%
  ✓ Kelly Criterion Position Sizing

Loaded 2517 trading days
Period: 2014-12-15 to 2024-12-13

=== BACKTEST RESULTS ===
Initial Capital: $100000.00
Final Value: $485623.45
Total Return: 385.62%
CAGR: 16.32%
Max Drawdown: 28.45%
Sharpe Ratio: 1.234
Trades: 47 (32 wins, 68.1% win rate)
Profit Factor: 2.56

Results exported to results.csv
```

### CSV Results File

The results file contains:

1. **Summary section**: All performance metrics
2. **Trade log**: Detailed entry/exit data for each trade

## 🎓 Resume Bullets

After running this project, you can add these accomplishments to your resume:

### Technical Implementation

- Engineered high-performance C++ backtesting engine processing 10+ years of historical stock data with O(n) time complexity
- Implemented modular architecture with clean separation of concerns across CSV parsing, technical analysis, and execution layers

### Algorithmic Trading

- Optimized signal generation using EMA for reduced lag vs traditional SMA
- Integrated MACD momentum indicator for multi-factor signal confirmation
- Applied Bollinger Bands for volatility-based entry/exit optimization

### Risk Management

- Implemented risk management with stop-loss and take-profit mechanisms
- Simulated realistic trading costs with commission-adjusted P&L calculation
- Implemented Kelly Criterion for optimal position sizing based on win rate and risk/reward ratios

### Performance Analysis

- Conducted parameter optimization across multiple MA periods for strategy tuning
- Calculated comprehensive performance metrics including CAGR, Sharpe ratio, and maximum drawdown
- Achieved X% CAGR with Y Sharpe ratio across Z trades [fill in your actual results]

### Data Structures & Algorithms

- Utilized STL containers (vectors, deques) for efficient time-series data handling
- Implemented sliding window algorithms for moving average calculations
- Designed custom data structures for OHLCV bars, trades, and performance metrics

## 🔧 Command Line Options

| Option             | Description                | Default     |
| ------------------ | -------------------------- | ----------- |
| `--short <n>`      | Short MA period            | 50          |
| `--long <n>`       | Long MA period             | 200         |
| `--capital <n>`    | Initial capital            | 100000      |
| `--rsi`            | Enable RSI filter          | Off         |
| `--ema`            | Use EMA instead of SMA     | Off         |
| `--macd`           | Enable MACD confirmation   | Off         |
| `--bollinger`      | Enable Bollinger Bands     | Off         |
| `--stoploss <n>`   | Stop loss % (e.g., 0.05)   | 0           |
| `--takeprofit <n>` | Take profit % (e.g., 0.15) | 0           |
| `--commission <n>` | Commission rate            | 0.001       |
| `--kelly`          | Use Kelly Criterion        | Off         |
| `--compare`        | Run strategy comparison    | Off         |
| `--output <file>`  | Results filename           | results.csv |

## 📊 Performance Metrics Explained

- **Total Return**: Overall percentage gain/loss
- **CAGR**: Compound Annual Growth Rate - annualized return
- **Max Drawdown**: Largest peak-to-trough decline
- **Sharpe Ratio**: Risk-adjusted return (higher is better)
- **Win Rate**: Percentage of profitable trades
- **Profit Factor**: Gross profit / Gross loss (>1 is profitable)

## 🧪 Testing Different Stocks

### US Stocks

```bash
./build/backtester data/AAPL.csv   # Apple
./build/backtester data/TSLA.csv   # Tesla
./build/backtester data/MSFT.csv   # Microsoft
./build/backtester data/GOOGL.csv  # Google
```

### Indian Stocks (NSE)

```bash
./build/backtester data/RELIANCE_NS.csv   # Reliance Industries
./build/backtester data/TCS_NS.csv        # TCS
./build/backtester data/INFY_NS.csv       # Infosys
./build/backtester data/HDFCBANK_NS.csv   # HDFC Bank
```

## 🚀 Next Steps & Extensions

### Potential Enhancements

1. **Additional Indicators**: ADX, Stochastic, ATR
2. **Multi-Asset Support**: Portfolio backtesting
3. **Machine Learning**: Integrate ML-based signals
4. **Real-time Trading**: Connect to broker APIs
5. **Web Dashboard**: Visualize results with charts
6. **Parameter Optimization**: Grid search for best parameters
7. **Walk-Forward Analysis**: Out-of-sample testing

## 📝 Notes

- CSV files must have headers: `Date,Open,High,Low,Close,Adj Close,Volume`
- Dates should be in `YYYY-MM-DD` format
- The system uses next-day open prices for execution when available
- Results are approximate and for educational purposes only
- Past performance does not guarantee future results

## 🤝 Contributing

This is a personal project for learning and portfolio purposes. Feel free to fork and extend!

## 📄 License

MIT License - Free to use for educational and personal projects.

## ⚠️ Disclaimer

This software is for educational purposes only. Do not use for actual trading without proper testing and understanding. Trading involves substantial risk of loss.

---

**Built with**: C++17, STL, Make/CMake
**Data Source**: Yahoo Finance (via yfinance)
**Time to Complete**: ~12 hours for core + 6 hours for advanced features
