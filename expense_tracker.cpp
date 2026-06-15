#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <map>

using namespace std;

struct Expense {
    string date;
    string category;
    string description;
    double amount;
};

const vector<string> CATEGORIES = {"Food", "Transport", "Entertainment", "Shopping", "Bills"};

string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}

void saveToJSON(const vector<Expense>& expenses) {
    ofstream file("expenses.json");
    file << "[\n";
    for (size_t i = 0; i < expenses.size(); i++) {
        file << "  {\n";
        file << "    \"date\": \"" << expenses[i].date << "\",\n";
        file << "    \"category\": \"" << expenses[i].category << "\",\n";
        file << "    \"description\": \"" << expenses[i].description << "\",\n";
        file << "    \"amount\": " << fixed << setprecision(2) << expenses[i].amount << "\n";
        file << "  }";
        if (i < expenses.size() - 1) file << ",";
        file << "\n";
    }
    file << "]\n";
    file.close();
    cout << "Saved to expenses.json\n";
}

vector<Expense> loadFromJSON() {
    vector<Expense> expenses;
    ifstream file("expenses.json");
    if (!file.is_open()) return expenses;

    string line, date, category, description;
    double amount = 0;
    Expense current;

    while (getline(file, line)) {
        auto extract = [&](const string& key) -> string {
            size_t pos = line.find("\"" + key + "\": \"");
            if (pos != string::npos) {
                size_t start = pos + key.size() + 4;
                size_t end = line.find("\"", start);
                return line.substr(start, end - start);
            }
            return "";
        };

        if (line.find("\"date\"") != string::npos) current.date = extract("date");
        else if (line.find("\"category\"") != string::npos) current.category = extract("category");
        else if (line.find("\"description\"") != string::npos) current.description = extract("description");
        else if (line.find("\"amount\"") != string::npos) {
            size_t pos = line.find(": ");
            if (pos != string::npos) current.amount = stod(line.substr(pos + 2));
        }
        else if (line.find("}") != string::npos && !current.date.empty()) {
            expenses.push_back(current);
            current = Expense();
        }
    }
    file.close();
    return expenses;
}

void addExpense(vector<Expense>& expenses) {
    Expense e;
    e.date = getCurrentDate();

    cout << "\nCategories:\n";
    for (size_t i = 0; i < CATEGORIES.size(); i++) {
        cout << i + 1 << ". " << CATEGORIES[i] << "\n";
    }
    cout << "Pick category (1-5): ";
    int choice;
    cin >> choice;
    if (choice < 1 || choice > 5) { cout << "Invalid choice.\n"; return; }
    e.category = CATEGORIES[choice - 1];

    cin.ignore();
    cout << "Description: ";
    getline(cin, e.description);

    cout << "Amount (Rs): ";
    cin >> e.amount;
    if (e.amount <= 0) { cout << "Invalid amount.\n"; return; }

    expenses.push_back(e);
    saveToJSON(expenses);
    cout << "Expense added: Rs " << fixed << setprecision(2) << e.amount << " for " << e.category << "\n";
}

void viewSummary(const vector<Expense>& expenses) {
    if (expenses.empty()) { cout << "\nNo expenses yet.\n"; return; }

    double total = 0;
    map<string, double> byCategory;

    for (const auto& e : expenses) {
        total += e.amount;
        byCategory[e.category] += e.amount;
    }

    cout << "\n===== EXPENSE SUMMARY =====\n";
    cout << "Total Spent: Rs " << fixed << setprecision(2) << total << "\n\n";
    cout << "By Category:\n";
    for (const auto& cat : CATEGORIES) {
        if (byCategory.count(cat)) {
            cout << "  " << cat << ": Rs " << fixed << setprecision(2) << byCategory[cat] << "\n";
        }
    }

    cout << "\nRecent Expenses:\n";
    int show = min((int)expenses.size(), 5);
    for (int i = expenses.size() - 1; i >= (int)expenses.size() - show; i--) {
        cout << "  [" << expenses[i].date << "] " << expenses[i].category
             << " - " << expenses[i].description
             << " - Rs " << fixed << setprecision(2) << expenses[i].amount << "\n";
    }
}

void deleteExpense(vector<Expense>& expenses) {
    if (expenses.empty()) { cout << "\nNo expenses to delete.\n"; return; }

    cout << "\nAll Expenses:\n";
    for (size_t i = 0; i < expenses.size(); i++) {
        cout << i + 1 << ". [" << expenses[i].date << "] "
             << expenses[i].category << " - " << expenses[i].description
             << " - Rs " << expenses[i].amount << "\n";
    }
    cout << "Enter number to delete (0 to cancel): ";
    int choice;
    cin >> choice;
    if (choice < 1 || choice > (int)expenses.size()) { cout << "Cancelled.\n"; return; }
    expenses.erase(expenses.begin() + choice - 1);
    saveToJSON(expenses);
    cout << "Deleted.\n";
}

int main() {
    cout << "============================\n";
    cout << "  Rupee Expense Tracker\n";
    cout << "============================\n";

    vector<Expense> expenses = loadFromJSON();
    cout << "Loaded " << expenses.size() << " expenses.\n";

    while (true) {
        cout << "\n1. Add Expense\n2. View Summary\n3. Delete Expense\n4. Exit\n";
        cout << "Choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: addExpense(expenses); break;
            case 2: viewSummary(expenses); break;
            case 3: deleteExpense(expenses); break;
            case 4: cout << "Bye!\n"; return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}
