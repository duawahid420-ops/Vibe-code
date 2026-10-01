// Artisan Craft Authenticity System  (portable: C++98 / C++11 / C++17, GCC / MinGW / MSVC / Clang)

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

using namespace std;

struct CraftProduct {
    int productId;
    string artisanName;
    string artisanLocation;
    string craftType;
    string creationStory;
    double price;
    string verificationHash;
    bool isClaimed;
};

vector<CraftProduct> registry;
int nextId = 101;

string intToString(int v) {
    ostringstream ss;
    ss << v;
    return ss.str();
}

unsigned long hashString(const string& s) {
    unsigned long h = 2166136261UL;
    for (size_t i = 0; i < s.size(); i++) {
        h ^= (unsigned char)s[i];
        h = (h * 16777619UL) & 0xFFFFFFFFUL;
    }
    return h;
}

string craftPrefix(const string& craft) {
    string p;
    for (size_t i = 0; i < craft.size() && p.size() < 3; i++) {
        unsigned char c = (unsigned char)craft[i];
        if (isalpha(c)) p += (char)toupper(c);
    }
    while (p.size() < 3) p += 'X';
    return p;
}

string generateVerificationTag(int id, const string& artisan, const string& craft) {
    string seed = "ARTISAN-SECRET-SALT" + intToString(id) + artisan + craft;
    unsigned long h = hashString(seed);
    ostringstream ss;
    ss << "AUTH-" << craftPrefix(craft) << "-" << id << "-"
       << uppercase << hex << setw(8) << setfill('0') << h;
    return ss.str();
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toUpperStr(string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = (char)toupper((unsigned char)s[i]);
    return s;
}

string readLine(const string& prompt) {
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) {
            cout << "\nInput closed. Exiting." << endl;
            exit(0);
        }
        line = trim(line);
        if (!line.empty()) return line;
        cout << "  Input cannot be empty. Try again." << endl;
    }
}

double readPrice(const string& prompt) {
    while (true) {
        string line = readLine(prompt);
        istringstream in(line);
        double v;
        char extra;
        if ((in >> v) && !(in >> extra) && v > 0) return v;
        cout << "  Please enter a valid positive number." << endl;
    }
}

int readMenuChoice(const string& prompt) {
    while (true) {
        string line = readLine(prompt);
        istringstream in(line);
        int v;
        char extra;
        if ((in >> v) && !(in >> extra)) return v;
        cout << "  Please enter a number." << endl;
    }
}

void banner(const string& title) {
    cout << "\n========================================" << endl;
    cout << "  " << title << endl;
    cout << "========================================" << endl;
}

void registerCraft() {
    banner("ARTISAN REGISTRATION PORTAL");

    CraftProduct item;
    item.artisanName     = readLine("Enter Artisan Name: ");
    item.artisanLocation = readLine("Enter Artisan Village/City: ");
    item.craftType       = readLine("Enter Craft Type (e.g., Sindhi Embroidery, Ralli): ");
    item.creationStory   = readLine("Enter Creation Story / Background: ");
    item.price           = readPrice("Enter Fair Price (PKR): ");

    item.productId = nextId++;
    item.isClaimed = false;
    item.verificationHash = generateVerificationTag(item.productId, item.artisanName, item.craftType);

    registry.push_back(item);

    cout << "\n[SUCCESS] Craft registered successfully!" << endl;
    cout << "Product ID      : " << item.productId << endl;
    cout << "Unique Auth Tag : [" << item.verificationHash << "]" << endl;
    cout << "Attach this tag to the physical craft." << endl;
}

void verifyCraft() {
    banner("BUYER VERIFICATION PORTAL");

    string inputTag = toUpperStr(readLine("Enter the Product Verification Tag from the item: "));

    for (size_t i = 0; i < registry.size(); i++) {
        CraftProduct& item = registry[i];
        if (item.verificationHash == inputTag) {
            cout << "\n----------------------------------------" << endl;
            cout << "STATUS: [REGISTERED HANDMADE CRAFT]" << endl;
            cout << "----------------------------------------" << endl;
            cout << "Craft Name    : " << item.craftType << endl;
            cout << "Created By    : " << item.artisanName << endl;
            cout << "Origin        : " << item.artisanLocation << endl;
            cout << "Fair Price    : PKR " << fixed << setprecision(2) << item.price << endl;
            cout << "Artisan Story : \"" << item.creationStory << "\"" << endl;

            if (!item.isClaimed) {
                cout << "\nVerification Status: FIRST-TIME VERIFIED (Genuine Product)" << endl;
                item.isClaimed = true;
            } else {
                cout << "\n[WARNING] This tag has already been scanned/claimed before!" << endl;
                cout << "Possible copy or duplicate product detected." << endl;
            }
            return;
        }
    }

    cout << "\n[ALERT] Invalid Tag! No record found in the authenticity registry." << endl;
    cout << "This may be a counterfeit or mass-printed fake product." << endl;
}

void listAllCrafts() {
    banner("ALL REGISTERED AUTHENTIC CRAFTS");

    if (registry.empty()) {
        cout << "No crafts registered yet." << endl;
        return;
    }

    for (size_t i = 0; i < registry.size(); i++) {
        const CraftProduct& item = registry[i];
        cout << "ID: " << item.productId
             << " | Type: " << item.craftType
             << " | Artisan: " << item.artisanName
             << " | Origin: " << item.artisanLocation
             << " | PKR " << fixed << setprecision(2) << item.price
             << " | Tag: " << item.verificationHash
             << " | " << (item.isClaimed ? "CLAIMED" : "UNCLAIMED")
             << endl;
    }
    cout << "\nTotal registered: " << registry.size() << endl;
}

int main() {
    while (true) {
        banner("ARTISAN CRAFT AUTHENTICITY SYSTEM");
        cout << "  1. Register a Craft (Artisan)" << endl;
        cout << "  2. Verify a Craft (Buyer)" << endl;
        cout << "  3. List All Registered Crafts" << endl;
        cout << "  4. Exit" << endl;

        int choice = readMenuChoice("Select an option (1-4): ");
        switch (choice) {
            case 1: registerCraft(); break;
            case 2: verifyCraft();   break;
            case 3: listAllCrafts(); break;
            case 4:
                cout << "\nThank you for supporting authentic artisans. Goodbye!" << endl;
                cout << "Press Enter to close...";
                { string dummy; getline(cin, dummy); }
                return 0;
            default:
                cout << "Invalid option. Please choose 1-4." << endl;
        }
    }
}
