#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

class Account
{
private :
    string Accountnumber ;
    string pin ;
    double balance ;
    vector<string> transaction ;
public :
    Account(string accNum = "", string p = "" , double bal = 0.0)
    {
        Accountnumber = accNum ;
        pin = p ;
        balance = bal ;
    }
    string getAccountNumber() const
    {
        return Accountnumber ;
    }
    string getPin() const
    {
        return pin ;
    }
    double getBalance() const
    {
        return balance ;
    }
    vector<string> getTransaction()
    {
        return transaction ;
    }
    void deposit(double amount)
    {
        balance += amount ;
        transaction.push_back("Deposit :" + to_string(amount)) ;
        transaction.push_back("Balance :" + to_string(balance)) ;
    }
    bool withdraw(double amount)
    {
        if(amount > balance)
        {
            return false ;
        }
        balance -= amount ;
        transaction.push_back("Withdrawal :" + to_string(amount)) ;
        transaction.push_back("Balance :" + to_string(balance)) ;
        return true ;
    }
    void loadTransaction(const string &filename)
    {
        ifstream file(filename) ;
        string line ;
        transaction.clear() ;
        while(getline(file,line))
        {
            transaction.push_back(line) ;
        }
        file.close() ;
    }
    void saveTransaction(const string &filename) const
    {
        ofstream file(filename,ios::app) ;
        for(const auto& acc : transaction)
        {
            file << acc << endl ;
        }
        file.close() ;
    }
private :
    string getCurrTime() const
    {
        time_t now = time(0) ;
        tm *ltm = localtime(&now) ;
        char buffer[20] ;
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm) ;
        return string(buffer) ;    
    }
} ;

class ATM
{
private :
    vector<Account> accounts ;
    Account* currentAccount ;
public :
    ATM()
    {
        currentAccount = nullptr ;
        loadAccounts() ;
    }
    void loadAccounts()
    {
        ifstream file("accounts.txt") ;
        string accNum, pin ;
        double bal ;
        while(file >> accNum >> pin >> bal)
        {
            Account acc(accNum,pin,bal) ;
            acc.loadTransaction("account_" + accNum + ".txt") ;
            accounts.push_back(acc) ;            
        }
        file.close() ;
    }
    void saveAccounts() const
    {
        ofstream file("accounts.txt") ;
         for (const auto& acc : accounts) 
        {
            file << acc.getAccountNumber() << " " << acc.getPin() << " " << acc.getBalance() << endl;
            acc.saveTransaction("account_" + acc.getAccountNumber() + ".txt");
        }
        file.close() ;
    }

    bool createAccount(const string & accNum, const string & pin, double initialBalance )
    {
        for(const auto& acc : accounts)
        {
            if(acc.getAccountNumber() == accNum)
            {
                return false ; // Account already exists
            }
            Account newAcc(accNum,pin,initialBalance) ;
            accounts.push_back(newAcc) ;
            saveAccounts() ;
            return true ;
        }
        return false ; // Account creation failed
    }

    bool login(const string & accNum, const string & pin)
    {
        for(auto& acc : accounts)
        {
            if(acc.getAccountNumber() == accNum && acc.getPin() == pin)
            {
                currentAccount = &acc ;
                return true ;
            }
        }
        return false ; // Login failed
    }

    void logout()
    {
        currentAccount = nullptr ;
        saveAccounts() ;
    }

    void checkBalance() const
    {
        if(currentAccount)
        {
            cout << "Current Balance: $" << fixed << setprecision(2) << currentAccount->getBalance() << endl ;
        }
        else
        {
            cout << "No account logged in." << endl ;
        }
    }

    void deposit(double amount)
    {
        if(currentAccount && amount > 0)
        {
            currentAccount->deposit(amount) ;
            cout << "Deposit successful. New balance: $" << currentAccount->getBalance() << endl ;
            saveAccounts() ;
        }
        else
        {
            cout << "Invalid deposit amount." << endl ;
        }
    }

    void withdraw(double amount)
    {
        if(currentAccount && amount > 0)
        {
            if(currentAccount->withdraw(amount))
            {
                cout << "Withdrawal successful. New balance: $" << currentAccount->getBalance() << endl ;
                saveAccounts() ;
            }
            else
            {
                cout << "Insufficient funds for withdrawal." << endl ;
            }
        }
        else
        {
            cout << "Invalid withdrawal amount." << endl ;
        }
    }

    void showTransactionHistory() const
    {
        if(currentAccount)
        {
            cout << "Transaction History for Account " << currentAccount->getAccountNumber() << ":\n";
            for( const auto& transaction : currentAccount->getTransaction()) 
            {
                cout << transaction << endl ;
            }
        }
    }
} ;

int main()
{
    ATM atm ;
    int choice ;
    string accNum, pin ;
    double amount ;

    while(true)
    {
        cout << "\n=== ATM Menu ===\n";
        cout << "1. Create Account\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if(choice == 3)
        {
            cout << "Thank you for using the ATM. Goodbye!\n";
            break ;
        }
        switch (choice)
        {
            case 1 : // Create Account
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
                }
                 else {
                    cout << "Invalid account number or PIN.\n";
                }
                break;

            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}