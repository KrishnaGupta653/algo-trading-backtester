
"""
Stock Data Downloader - Download historical stock data from Yahoo Finance
Works with Indian (NSE/BSE) and US stocks
No API key required - uses yfinance library

Usage:
    python download_stock_data.py RELIANCE.NS
    python download_stock_data.py AAPL --years 5
    python download_stock_data.py TCS.NS INFY.NS HDFCBANK.NS --output data/
"""

import yfinance as yf
import pandas as pd
import argparse
from datetime import datetime, timedelta
import os
import sys

# Popular Indian stocks (NSE)
INDIAN_STOCKS = {
    'reliance': 'RELIANCE.NS',
    'tcs': 'TCS.NS',
    'infy': 'INFY.NS',
    'hdfcbank': 'HDFCBANK.NS',
    'icicibank': 'ICICIBANK.NS',
    'bhartiartl': 'BHARTIARTL.NS',
    'itc': 'ITC.NS',
    'sbin': 'SBIN.NS',
    'hindunilvr': 'HINDUNILVR.NS',
    'bajfinance': 'BAJFINANCE.NS',
    'wipro': 'WIPRO.NS',
    'maruti': 'MARUTI.NS',
    'tatamotors': 'TATAMOTORS.NS',
    'adaniports': 'ADANIPORTS.NS',
    'ongc': 'ONGC.NS'
}

# Popular US stocks
US_STOCKS = {
    'apple': 'AAPL',
    'microsoft': 'MSFT',
    'google': 'GOOGL',
    'amazon': 'AMZN',
    'tesla': 'TSLA',
    'meta': 'META',
    'nvidia': 'NVDA',
    'jpmorgan': 'JPM',
    'visa': 'V',
    'walmart': 'WMT'
}


def download_stock(symbol, years=10, output_dir='.'):
    """
    Download historical stock data from Yahoo Finance
    
    Args:
        symbol: Stock symbol (e.g., 'AAPL', 'RELIANCE.NS')
        years: Number of years of historical data
        output_dir: Directory to save CSV file
    
    Returns:
        Path to saved CSV file or None if failed
    """
    try:
        # Calculate date range
        end_date = datetime.now()
        start_date = end_date - timedelta(days=years*365)
        
        print(f"\n📊 Downloading {symbol}...")
        print(f"   Period: {start_date.date()} to {end_date.date()}")
        
        # Download data
        ticker = yf.Ticker(symbol)
        df = ticker.history(start=start_date, end=end_date)
        
        if df.empty:
            print(f"   ❌ No data found for {symbol}")
            return None
        
        # Reset index to make Date a column
        df.reset_index(inplace=True)
        
        # Rename columns to match expected format
        df.rename(columns={'Adj Close': 'Adj Close'}, inplace=True)
        
        # Select required columns in correct order
        columns = ['Date', 'Open', 'High', 'Low', 'Close', 'Volume']
        
        # Add Adj Close if available
        if 'Adj Close' in df.columns:
            columns.insert(5, 'Adj Close')
        else:
            df['Adj Close'] = df['Close']
            columns.insert(5, 'Adj Close')
        
        df = df[columns]
        
        # Format Date column
        df['Date'] = pd.to_datetime(df['Date']).dt.strftime('%Y-%m-%d')
        
        # Create output directory if needed
        os.makedirs(output_dir, exist_ok=True)
        
        # Save to CSV
        filename = os.path.join(output_dir, f"{symbol.replace('.', '_')}.csv")
        df.to_csv(filename, index=False)
        
        print(f"   ✅ Saved {len(df)} rows to {filename}")
        print(f"   📅 Date range: {df['Date'].iloc[0]} to {df['Date'].iloc[-1]}")
        print(f"   💰 Price range: ${df['Close'].min():.2f} - ${df['Close'].max():.2f}")
        
        return filename
        
    except Exception as e:
        print(f"   ❌ Error downloading {symbol}: {str(e)}")
        return None


def download_multiple(symbols, years=10, output_dir='.'):
    """Download multiple stocks"""
    results = []
    
    print(f"\n🚀 Downloading {len(symbols)} stocks...")
    print(f"📁 Output directory: {output_dir}")
    print("=" * 60)
    
    for symbol in symbols:
        # Handle common names
        symbol_lookup = symbol.lower()
        if symbol_lookup in INDIAN_STOCKS:
            symbol = INDIAN_STOCKS[symbol_lookup]
            print(f"💡 '{symbol_lookup}' → {symbol}")
        elif symbol_lookup in US_STOCKS:
            symbol = US_STOCKS[symbol_lookup]
            print(f"💡 '{symbol_lookup}' → {symbol}")
        
        result = download_stock(symbol, years, output_dir)
        if result:
            results.append(result)
    
    print("\n" + "=" * 60)
    print(f"✅ Successfully downloaded {len(results)}/{len(symbols)} stocks")
    
    return results


def download_preset(preset, years=10, output_dir='.'):
    """Download preset stock lists"""
    presets = {
        'indian_top10': [
            'RELIANCE.NS', 'TCS.NS', 'HDFCBANK.NS', 'INFY.NS', 'ICICIBANK.NS',
            'BHARTIARTL.NS', 'ITC.NS', 'SBIN.NS', 'HINDUNILVR.NS', 'BAJFINANCE.NS'
        ],
        'us_top10': [
            'AAPL', 'MSFT', 'GOOGL', 'AMZN', 'TSLA',
            'META', 'NVDA', 'JPM', 'V', 'WMT'
        ],
        'indian_it': ['TCS.NS', 'INFY.NS', 'WIPRO.NS', 'HCLTECH.NS', 'TECHM.NS'],
        'indian_banks': ['HDFCBANK.NS', 'ICICIBANK.NS', 'SBIN.NS', 'KOTAKBANK.NS', 'AXISBANK.NS'],
        'us_tech': ['AAPL', 'MSFT', 'GOOGL', 'META', 'NVDA', 'TSLA', 'AMD', 'INTC']
    }
    
    if preset not in presets:
        print(f"❌ Unknown preset: {preset}")
        print(f"Available presets: {', '.join(presets.keys())}")
        return []
    
    print(f"📦 Downloading preset: {preset}")
    return download_multiple(presets[preset], years, output_dir)


def list_available():
    """List available stock shortcuts"""
    print("\n🇮🇳 Indian Stocks (NSE):")
    print("-" * 40)
    for name, symbol in sorted(INDIAN_STOCKS.items()):
        print(f"  {name:15} → {symbol}")
    
    print("\n🇺🇸 US Stocks:")
    print("-" * 40)
    for name, symbol in sorted(US_STOCKS.items()):
        print(f"  {name:15} → {symbol}")
    
    print("\n📦 Presets:")
    print("-" * 40)
    print("  indian_top10   → Top 10 NSE stocks")
    print("  us_top10       → Top 10 US stocks")
    print("  indian_it      → Indian IT sector")
    print("  indian_banks   → Indian banking sector")
    print("  us_tech        → US tech giants")
    
    print("\n💡 Usage examples:")
    print("  python download_stock_data.py reliance")
    print("  python download_stock_data.py AAPL TSLA MSFT")
    print("  python download_stock_data.py --preset indian_top10")
    print("  python download_stock_data.py TCS.NS --years 5 --output data/")


def main():
    parser = argparse.ArgumentParser(
        description='Download historical stock data from Yahoo Finance',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Download single stock (10 years default)
  python download_stock_data.py RELIANCE.NS
  
  # Download multiple stocks
  python download_stock_data.py AAPL TSLA MSFT
  
  # Use shortcuts for Indian/US stocks
  python download_stock_data.py reliance tcs infy
  python download_stock_data.py apple tesla google
  
  # Custom time period
  python download_stock_data.py HDFCBANK.NS --years 5
  
  # Save to specific directory
  python download_stock_data.py TCS.NS --output data/
  
  # Download preset lists
  python download_stock_data.py --preset indian_top10
  python download_stock_data.py --preset us_tech --years 3
  
  # List available shortcuts
  python download_stock_data.py --list
        """
    )
    
    parser.add_argument('symbols', nargs='*', help='Stock symbols to download')
    parser.add_argument('--years', type=int, default=10, help='Years of historical data (default: 10)')
    parser.add_argument('--output', '-o', default='.', help='Output directory (default: current)')
    parser.add_argument('--preset', choices=['indian_top10', 'us_top10', 'indian_it', 'indian_banks', 'us_tech'],
                       help='Download preset stock list')
    parser.add_argument('--list', '-l', action='store_true', help='List available stock shortcuts')
    
    args = parser.parse_args()
    
    # List available stocks
    if args.list:
        list_available()
        return
    
    # Download preset
    if args.preset:
        download_preset(args.preset, args.years, args.output)
        return
    
    # Need at least one symbol
    if not args.symbols:
        parser.print_help()
        print("\n💡 Tip: Use --list to see available shortcuts")
        return
    
    # Download specified stocks
    download_multiple(args.symbols, args.years, args.output)


if __name__ == '__main__':
    # Check if yfinance is installed
    try:
        import yfinance
    except ImportError:
        print("❌ yfinance not installed!")
        print("📦 Install it with: pip install yfinance")
        print("   Or: pip install yfinance pandas")
        sys.exit(1)
    
    main()