# 🏦 ATM Simulation in C++

A command-line ATM simulation system built in **C++**, allowing users to securely create accounts, log in, deposit/withdraw funds, and view transaction history. All data is stored persistently using text files.

---

## 🔧 Features

- 👤 Account creation with 4-digit PIN
- 🔐 Secure login system
- 💵 Deposit and withdraw functionality
- 📊 View current account balance
- 📜 Transaction history stored in text files
- 💾 File-based data persistence

---

## 📁 File Structure

├── project_01.cpp // Main source code
├── accounts.txt // Stores account numbers, PINs, and balances
├── account_XXXX.txt // Individual transaction history

---

## 🚀 How to Run

1. Compile the program:
   ```bash
   g++ -std=c++11 project_01.cpp -o atm

Run the executable:
  ./atm

**✨ Future Enhancements**
  PIN encryption
  
  GUI using Qt or SFML
  
  JSON-based storage
  
  Transaction time stamps

