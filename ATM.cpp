#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

// Class to represent a bank account
class Account {
private:
    string accountNumber;
    string pin;
    double balance;
    vector<string> transactions;

public:
    Account(string accNum = "", string p = "", double bal = 0.0) 
        : accountNumber(accNum), pin(p), balance(bal) {}

    string getAccountNumber() const { return accountNumber; }
    string getPin() const { return pin; }
    double getBalance() const { return balance; }
    vector<string> getTransactions() const { return transactions; }

    void deposit(double amount) {
        balance += amount;
        string transaction = "Deposit: +" + to_string(amount) + " | Balance: " + to_string(balance) + " | " + getCurrentTime();
        transactions.push_back(transaction);
    }

    bool withdraw(double amount) {
        if (amount > balance) {
            return false;
        }
        balance -= amount;
        string transaction = "Withdrawal: -" + to_string(amount) + " | Balance: " + to_string(balance) + " | " + getCurrentTime();
        transactions.push_back(transaction);
        return true;
    }

    void loadTransactions(const string& filename) {
        ifstream file(filename);
        string line;
        transactions.clear();
        while (getline(file, line)) {
            transactions.push_back(line);
        }
        file.close();
    }

    void saveTransactions(const string& filename) const {
        ofstream file(filename, ios::app);
        for (const auto& transaction : transactions) {
            file << transaction << endl;
        }
        file.close();
    }

private:
    string getCurrentTime() const {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
        return string(buffer);
    }
};

// Class to manage ATM operations
class ATM {
private:
    vector<Account> accounts;
    Account* currentAccount;

public:
    ATM() : currentAccount(nullptr) {
        loadAccounts();
    }

    void loadAccounts() {
        ifstream file("accounts.txt");
        string accNum, pin;
        double bal;
        while (file >> accNum >> pin >> bal) {
            Account acc(accNum, pin, bal);
            acc.loadTransactions("account_" + accNum + ".txt");
            accounts.push_back(acc);
        }
        file.close();
    }

    void saveAccounts() const {
        ofstream file("accounts.txt");
        for (const auto& acc : accounts) {
            file << acc.getAccountNumber() << " " << acc.getPin() << " " << acc.getBalance() << endl;
            acc.saveTransactions("account_" + acc.getAccountNumber() + ".txt");
        }
        file.close();
    }

    bool createAccount(const string& accNum, const string& pin, double initialBalance) {
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                return false; // Account number already exists
            }
        }
        Account newAcc(accNum, pin, initialBalance);
        accounts.push_back(newAcc);
        saveAccounts();
        return true;
    }

    bool login(const string& accNum, const string& pin) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum && acc.getPin() == pin) {
                currentAccount = &acc;
                return true;
            }
        }
        return false;
    }

    void logout() {
        currentAccount = nullptr;
        saveAccounts();
    }

    void checkBalance() const {
        if (currentAccount) {
            cout << "Current Balance: $" << fixed << setprecision(2) << currentAccount->getBalance() << endl;
        }
    }

    void deposit(double amount) {
        if (currentAccount && amount > 0) {
            currentAccount->deposit(amount);
            cout << "Deposited $" << amount << " successfully.\n";
            saveAccounts();
        } else {
            cout << "Invalid deposit amount.\n";
        }
    }

    void withdraw(double amount) {
        if (currentAccount && amount > 0) {
            if (currentAccount->withdraw(amount)) {
                cout << "Withdrawn $" << amount << " successfully.\n";
                saveAccounts();
            } else {
                cout << "Insufficient funds.\n";
            }
        } else {
            cout << "Invalid withdrawal amount.\n";
        }
    }

    void showTransactionHistory() const {
        if (currentAccount) {
            cout << "Transaction History for Account " << currentAccount->getAccountNumber() << ":\n";
            for (const auto& transaction : currentAccount->getTransactions()) {
                cout << transaction << endl;
            }
        }
    }
};

// Main function to run the ATM interface
int main() {
    ATM atm;
    int choice;
    string accNum, pin;
    double amount;

    while (true) {
        cout << "\n=== ATM Interface ===\n";
        cout << "1. Create Account\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 3) {
            cout << "Thank you for using the ATM. Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1: // Create Account
                cout << "Enter new account number (4 digits): ";
                cin >> accNum;
                if (accNum.length() != 4 || !all_of(accNum.begin(), accNum.end(), ::isdigit)) {
                    cout << "Invalid account number. Must be 4 digits.\n";
                    break;
                }
                cout << "Enter new 4-digit PIN: ";
                cin >> pin;
                if (pin.length() != 4 || !all_of(pin.begin(), pin.end(), ::isdigit)) {
                    cout << "Invalid PIN. Must be 4 digits.\n";
                    break;
                }
                cout << "Enter initial balance: $";
                cin >> amount;
                if (amount < 0) {
                    cout << "Invalid amount.\n";
                    break;
                }
                if (atm.createAccount(accNum, pin, amount)) {
                    cout << "Account created successfully!\n";
                } else {
                    cout << "Account number already exists.\n";
                }
                break;

            case 2: // Login
                cout << "Enter account number: ";
                cin >> accNum;
                cout << "Enter PIN: ";
                cin >> pin;
                if (atm.login(accNum, pin)) {
                    while (true) {
                        cout << "\n=== Account Menu ===\n";
                        cout << "1. Check Balance\n";
                        cout << "2. Deposit\n";
                        cout << "3. Withdraw\n";
                        cout << "4. Transaction History\n";
                        cout << "5. Logout\n";
                        cout << "Enter choice: ";
                        cin >> choice;

                        if (choice == 5) {
                            atm.logout();
                            break;
                        }

                        switch (choice) {
                            case 1:
                                atm.checkBalance();
                                break;
                            case 2:
                                cout << "Enter deposit amount: $";
                                cin >> amount;
                                atm.deposit(amount);
                                break;
                            case 3:
                                cout << "Enter withdrawal amount: $";
                                cin >> amount;
                                atm.withdraw(amount);
                                break;
                            case 4:
                                atm.showTransactionHistory();
                                break;
                            default:
                                cout << "Invalid choice.\n";
                        }
                    }
                } else {
                    cout << "Invalid account number or PIN.\n";
                }
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }

    return 0;
}