/**
 * @file main.cpp
 * @brief Complete C++ Banking System Implementation
 * 
 * Design of Customer, Account, and Transaction classes with 
 * deposit, withdrawal, fund transfer, transaction history,
 * and comprehensive account management.
 * 
 * Compile: g++ -std=c++17 -Wall -Wextra main.cpp -o banking_system
 * Run: ./banking_system
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// ==========================================
// 1. ENUMS & UTILITY HELPERS
// ==========================================

enum class TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER_IN,
    TRANSFER_OUT
};

enum class AccountType {
    SAVINGS,
    CHECKING,
    BUSINESS
};

std::string transactionTypeToString(TransactionType type) {
    switch (type) {
        case TransactionType::DEPOSIT:      return "DEPOSIT";
        case TransactionType::WITHDRAWAL:   return "WITHDRAWAL";
        case TransactionType::TRANSFER_IN:  return "TRANSFER IN";
        case TransactionType::TRANSFER_OUT: return "TRANSFER OUT";
        default: return "UNKNOWN";
    }
}

std::string accountTypeToString(AccountType type) {
    switch (type) {
        case AccountType::SAVINGS:  return "Savings";
        case AccountType::CHECKING: return "Checking";
        case AccountType::BUSINESS: return "Business";
        default: return "Unknown";
    }
}

std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&localTm, &now);
#else
    localtime_r(&now, &localTm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// ==========================================
// 2. TRANSACTION CLASS
// ==========================================
class Transaction {
private:
    std::string transactionId;
    std::string timestamp;
    TransactionType type;
    double amount;
    double balanceAfter;
    std::string description;
    std::string relatedAccount;

public:
    Transaction(std::string id, TransactionType t, double amt, double balAfter, 
                std::string desc = "", std::string relatedAcc = "")
        : transactionId(std::move(id)),
          timestamp(getCurrentTimestamp()),
          type(t),
          amount(amt),
          balanceAfter(balAfter),
          description(std::move(desc)),
          relatedAccount(std::move(relatedAcc)) {}

    // Getters
    [[nodiscard]] std::string getTransactionId() const { return transactionId; }
    [[nodiscard]] std::string getTimestamp() const { return timestamp; }
    [[nodiscard]] TransactionType getType() const { return type; }
    [[nodiscard]] double getAmount() const { return amount; }
    [[nodiscard]] double getBalanceAfter() const { return balanceAfter; }
    [[nodiscard]] std::string getDescription() const { return description; }
    [[nodiscard]] std::string getRelatedAccount() const { return relatedAccount; }

    // Formatted output
    void display() const {
        std::cout << "| " << std::left << std::setw(14) << transactionId
                  << "| " << std::setw(20) << timestamp
                  << "| " << std::setw(14) << transactionTypeToString(type)
                  << "| $" << std::right << std::setw(11) << std::fixed << std::setprecision(2) << amount
                  << " | $" << std::setw(11) << balanceAfter
                  << " | " << std::left << std::setw(25) << description
                  << " |
";
    }
};

// ==========================================
// 3. ACCOUNT CLASS
// ==========================================
class Account {
private:
    std::string accountNumber;
    std::string customerId;
    AccountType accountType;
    double balance;
    std::string openedAt;
    std::vector<Transaction> transactionHistory;
    int transactionCounter;

    std::string generateTransactionId() {
        return "TXN-" + accountNumber + "-" + std::to_string(++transactionCounter);
    }

public:
    Account(std::string accNum, std::string custId, AccountType type, double initialDeposit = 0.0)
        : accountNumber(std::move(accNum)),
          customerId(std::move(custId)),
          accountType(type),
          balance(0.0),
          openedAt(getCurrentTimestamp()),
          transactionCounter(0) {
        
        if (initialDeposit > 0.0) {
            deposit(initialDeposit, "Initial Account Opening Deposit");
        }
    }

    // Getters
    [[nodiscard]] std::string getAccountNumber() const { return accountNumber; }
    [[nodiscard]] std::string getCustomerId() const { return customerId; }
    [[nodiscard]] AccountType getAccountType() const { return accountType; }
    [[nodiscard]] double getBalance() const { return balance; }
    [[nodiscard]] std::string getOpenedAt() const { return openedAt; }
    [[nodiscard]] const std::vector<Transaction>& getTransactionHistory() const { return transactionHistory; }

    // Core Banking Operations
    bool deposit(double amount, const std::string& description = "Cash Deposit") {
        if (amount <= 0) {
            std::cout << "[Error] Deposit amount must be positive.
";
            return false;
        }

        balance += amount;
        std::string txnId = generateTransactionId();
        transactionHistory.emplace_back(txnId, TransactionType::DEPOSIT, amount, balance, description);
        return true;
    }

    bool withdraw(double amount, const std::string& description = "Cash Withdrawal") {
        if (amount <= 0) {
            std::cout << "[Error] Withdrawal amount must be positive.
";
            return false;
        }
        if (amount > balance) {
            std::cout << "[Error] Insufficient funds! Current balance: $" 
                      << std::fixed << std::setprecision(2) << balance 
                      << ", Requested: $" << amount << "
";
            return false;
        }

        balance -= amount;
        std::string txnId = generateTransactionId();
        transactionHistory.emplace_back(txnId, TransactionType::WITHDRAWAL, amount, balance, description);
        return true;
    }

    bool transferTo(Account& targetAccount, double amount, const std::string& note = "") {
        if (amount <= 0) {
            std::cout << "[Error] Transfer amount must be positive.
";
            return false;
        }
        if (this->accountNumber == targetAccount.getAccountNumber()) {
            std::cout << "[Error] Cannot transfer funds to the same account.
";
            return false;
        }
        if (amount > balance) {
            std::cout << "[Error] Insufficient funds for transfer. Available: $" 
                      << std::fixed << std::setprecision(2) << balance << "
";
            return false;
        }

        // Deduct from sender
        balance -= amount;
        std::string outTxnId = generateTransactionId();
        std::string outDesc = "Transfer to ACC#" + targetAccount.getAccountNumber() + (note.empty() ? "" : " (" + note + ")");
        transactionHistory.emplace_back(outTxnId, TransactionType::TRANSFER_OUT, amount, balance, outDesc, targetAccount.getAccountNumber());

        // Credit to recipient
        targetAccount.balance += amount;
        std::string inTxnId = targetAccount.generateTransactionId();
        std::string inDesc = "Transfer from ACC#" + this->accountNumber + (note.empty() ? "" : " (" + note + ")");
        targetAccount.transactionHistory.emplace_back(inTxnId, TransactionType::TRANSFER_IN, amount, targetAccount.balance, inDesc, this->accountNumber);

        return true;
    }

    // Display Account Overview
    void displayAccountInfo() const {
        std::cout << "
=======================================================
";
        std::cout << "               ACCOUNT INFORMATION                    
";
        std::cout << "=======================================================
";
        std::cout << " Account Number : " << accountNumber << "
";
        std::cout << " Customer ID    : " << customerId << "
";
        std::cout << " Account Type   : " << accountTypeToString(accountType) << "
";
        std::cout << " Current Balance: $" << std::fixed << std::setprecision(2) << balance << "
";
        std::cout << " Date Opened    : " << openedAt << "
";
        std::cout << " Total Txns     : " << transactionHistory.size() << "
";
        std::cout << "=======================================================
";
    }

    // Display Recent Transactions
    void displayRecentTransactions(size_t limit = 5) const {
        std::cout << "
--- Recent Transactions for Account [" << accountNumber << "] ---
";
        if (transactionHistory.empty()) {
            std::cout << "No transactions recorded yet.
";
            return;
        }

        std::cout << "+---------------+---------------------+---------------+---------------+---------------+---------------------------+
";
        std::cout << "| Txn ID        | Timestamp           | Type          | Amount        | Balance After | Description               |
";
        std::cout << "+---------------+---------------------+---------------+---------------+---------------+---------------------------+
";

        size_t start = (transactionHistory.size() > limit) ? (transactionHistory.size() - limit) : 0;
        for (size_t i = transactionHistory.size(); i > start; --i) {
            transactionHistory[i - 1].display();
        }
        std::cout << "+---------------+---------------------+---------------+---------------+---------------+---------------------------+
";
    }
};

// ==========================================
// 4. CUSTOMER CLASS
// ==========================================
class Customer {
private:
    std::string customerId;
    std::string name;
    std::string email;
    std::string phone;
    std::string address;
    std::string registeredAt;
    std::vector<std::string> accountNumbers;

public:
    Customer(std::string id, std::string n, std::string e, std::string p, std::string addr)
        : customerId(std::move(id)),
          name(std::move(n)),
          email(std::move(e)),
          phone(std::move(p)),
          address(std::move(addr)),
          registeredAt(getCurrentTimestamp()) {}

    // Getters
    [[nodiscard]] std::string getCustomerId() const { return customerId; }
    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] std::string getEmail() const { return email; }
    [[nodiscard]] std::string getPhone() const { return phone; }
    [[nodiscard]] std::string getAddress() const { return address; }
    [[nodiscard]] std::string getRegisteredAt() const { return registeredAt; }
    [[nodiscard]] const std::vector<std::string>& getAccountNumbers() const { return accountNumbers; }

    // Account association
    void linkAccount(const std::string& accNumber) {
        if (std::find(accountNumbers.begin(), accountNumbers.end(), accNumber) == accountNumbers.end()) {
            accountNumbers.push_back(accNumber);
        }
    }

    void displayCustomerInfo() const {
        std::cout << "
=======================================================
";
        std::cout << "               CUSTOMER PROFILE                       
";
        std::cout << "=======================================================
";
        std::cout << " Customer ID : " << customerId << "
";
        std::cout << " Full Name   : " << name << "
";
        std::cout << " Email       : " << email << "
";
        std::cout << " Phone       : " << phone << "
";
        std::cout << " Address     : " << address << "
";
        std::cout << " Joined Date : " << registeredAt << "
";
        std::cout << " Linked Accts: " << accountNumbers.size() << " accounts (";
        for (size_t i = 0; i < accountNumbers.size(); ++i) {
            std::cout << accountNumbers[i] << (i + 1 < accountNumbers.size() ? ", " : "");
        }
        std::cout << ")
";
        std::cout << "=======================================================
";
    }
};

// ==========================================
// 5. BANKING SYSTEM MANAGER CLASS
// ==========================================
class BankingSystem {
private:
    std::unordered_map<std::string, Customer> customers;
    std::unordered_map<std::string, Account> accounts;
    int customerCounter;
    int accountCounter;

    std::string generateCustomerId() {
        std::ostringstream oss;
        oss << "CUST-" << std::setw(4) << std::setfill('0') << ++customerCounter;
        return oss.str();
    }

    std::string generateAccountNumber() {
        std::ostringstream oss;
        oss << "ACC-" << std::setw(6) << std::setfill('0') << ++accountCounter;
        return oss.str();
    }

public:
    BankingSystem() : customerCounter(100), accountCounter(1000) {}

    // Customer Management
    std::string createCustomer(const std::string& name, const std::string& email, 
                               const std::string& phone, const std::string& address) {
        std::string custId = generateCustomerId();
        Customer newCust(custId, name, email, phone, address);
        customers.insert({custId, newCust});
        std::cout << "[Success] Created Customer ID: " << custId << " for " << name << "
";
        return custId;
    }

    Customer* getCustomer(const std::string& custId) {
        auto it = customers.find(custId);
        if (it != customers.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // Account Management
    std::string openAccount(const std::string& custId, AccountType type, double initialDeposit = 0.0) {
        Customer* cust = getCustomer(custId);
        if (!cust) {
            std::cout << "[Error] Customer ID not found: " << custId << "
";
            return "";
        }

        std::string accNum = generateAccountNumber();
        Account newAcc(accNum, custId, type, initialDeposit);
        accounts.insert({accNum, newAcc});
        cust->linkAccount(accNum);

        std::cout << "[Success] Opened " << accountTypeToString(type) 
                  << " Account #" << accNum << " for " << cust->getName() 
                  << " with initial balance $" << std::fixed << std::setprecision(2) << initialDeposit << "
";
        return accNum;
    }

    Account* getAccount(const std::string& accNum) {
        auto it = accounts.find(accNum);
        if (it != accounts.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // Transactions execution
    bool processDeposit(const std::string& accNum, double amount, const std::string& note = "Deposit") {
        Account* acc = getAccount(accNum);
        if (!acc) {
            std::cout << "[Error] Account not found: " << accNum << "
";
            return false;
        }
        return acc->deposit(amount, note);
    }

    bool processWithdrawal(const std::string& accNum, double amount, const std::string& note = "Withdrawal") {
        Account* acc = getAccount(accNum);
        if (!acc) {
            std::cout << "[Error] Account not found: " << accNum << "
";
            return false;
        }
        return acc->withdraw(amount, note);
    }

    bool processTransfer(const std::string& fromAccNum, const std::string& toAccNum, double amount, const std::string& note = "Transfer") {
        Account* sender = getAccount(fromAccNum);
        Account* receiver = getAccount(toAccNum);

        if (!sender) {
            std::cout << "[Error] Sender account not found: " << fromAccNum << "
";
            return false;
        }
        if (!receiver) {
            std::cout << "[Error] Recipient account not found: " << toAccNum << "
";
            return false;
        }

        return sender->transferTo(*receiver, amount, note);
    }

    void displayCustomerAccounts(const std::string& custId) {
        Customer* cust = getCustomer(custId);
        if (!cust) {
            std::cout << "[Error] Customer not found: " << custId << "
";
            return;
        }

        cust->displayCustomerInfo();
        std::cout << "
Accounts owned by " << cust->getName() << ":
";
        const auto& accList = cust->getAccountNumbers();
        if (accList.empty()) {
            std::cout << "No accounts registered yet.
";
            return;
        }

        for (const auto& accNum : accList) {
            Account* acc = getAccount(accNum);
            if (acc) {
                std::cout << " -> " << acc->getAccountNumber() 
                          << " | " << std::left << std::setw(10) << accountTypeToString(acc->getAccountType())
                          << " | Balance: $" << std::fixed << std::setprecision(2) << acc->getBalance() << "
";
            }
        }
    }
};

// ==========================================
// 6. MAIN & DEMO DRIVER
// ==========================================
int main() {
    BankingSystem bank;

    std::cout << "=======================================================
";
    std::cout << "       WELCOME TO MODERN C++ BANKING SYSTEM            
";
    std::cout << "=======================================================

";

    // 1. Create Customers
    std::cout << "[1] Creating Customers...
";
    std::string aliceId = bank.createCustomer("Alice Smith", "alice@example.com", "+1-555-0199", "123 Maple St, NY");
    std::string bobId   = bank.createCustomer("Bob Johnson", "bob@example.com", "+1-555-0244", "456 Oak Ave, CA");

    // 2. Open Accounts
    std::cout << "
[2] Opening Accounts...
";
    std::string aliceChecking = bank.openAccount(aliceId, AccountType::CHECKING, 1500.00);
    std::string aliceSavings  = bank.openAccount(aliceId, AccountType::SAVINGS, 5000.00);
    std::string bobChecking    = bank.openAccount(bobId, AccountType::CHECKING, 800.00);

    // 3. Perform Deposits & Withdrawals
    std::cout << "
[3] Performing Deposits & Withdrawals...
";
    bank.processDeposit(aliceChecking, 450.00, "Salary Bonus Deposit");
    bank.processWithdrawal(aliceChecking, 200.00, "ATM Cash Withdrawal");

    // 4. Fund Transfers
    std::cout << "
[4] Transferring Funds...
";
    std::cout << "Transferring $350.00 from Alice's Checking to Bob's Checking...
";
    bank.processTransfer(aliceChecking, bobChecking, 350.00, "Dinner split reimbursement");

    std::cout << "
Transferring $1000.00 from Alice's Savings to Alice's Checking...
";
    bank.processTransfer(aliceSavings, aliceChecking, 1000.00, "Monthly savings allocation");

    // 5. Display Account Information & Statements
    std::cout << "
[5] Displaying Account Details & Transaction Histories...
";
    Account* aliceAcc = bank.getAccount(aliceChecking);
    if (aliceAcc) {
        aliceAcc->displayAccountInfo();
        aliceAcc->displayRecentTransactions(10);
    }

    Account* bobAcc = bank.getAccount(bobChecking);
    if (bobAcc) {
        bobAcc->displayAccountInfo();
        bobAcc->displayRecentTransactions(10);
    }

    // 6. Display Customer Portfolios
    std::cout << "
[6] Displaying Full Customer Portfolio for Alice...
";
    bank.displayCustomerAccounts(aliceId);

    std::cout << "
Banking system execution completed successfully.
";
    return 0;
}
