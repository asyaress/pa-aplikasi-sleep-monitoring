#include "data_store.h"

#include <fstream>

using namespace std;

void initAppData(AppData& data) {
    data.userCount = 0;
}

void loadUsersFromFile(AppData& data, const string& filePath) {
    ifstream file(filePath.c_str());
    if (!file.is_open()) {
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        size_t p1 = line.find('|');
        if (p1 == string::npos) {
            continue;
        }

        size_t p2 = line.find('|', p1 + 1);
        if (p2 == string::npos) {
            continue;
        }

        if (data.userCount >= MAX_USERS) {
            break;
        }

        data.users[data.userCount].nama = line.substr(0, p1);
        data.users[data.userCount].username = line.substr(p1 + 1, p2 - p1 - 1);
        data.users[data.userCount].password = line.substr(p2 + 1);
        data.userCount++;
    }
}

bool saveUsersToFile(const AppData& data, const string& filePath) {
    ofstream file(filePath.c_str());
    if (!file.is_open()) {
        return false;
    }

    for (int i = 0; i < data.userCount; i++) {
        file << data.users[i].nama << '|'
             << data.users[i].username << '|'
             << data.users[i].password << '\n';
    }

    return true;
}
