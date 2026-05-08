#include "menu_common.h"

#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;

string inputBarisMenu(const string& pesan) {
    string nilai;
    cout << pesan;
    getline(cin, nilai);
    return nilai;
}

string ambilJamSekarang() {
    time_t waktuSekarang = time(0);
    tm* waktuLokal = localtime(&waktuSekarang);
    stringstream hasil;

    if (waktuLokal->tm_hour < 10) {
        hasil << '0';
    }
    hasil << waktuLokal->tm_hour << ':';

    if (waktuLokal->tm_min < 10) {
        hasil << '0';
    }
    hasil << waktuLokal->tm_min;

    return hasil.str();
}

bool berisiKarakterTerlarang(const string& nilai) {
    return nilai.find('|') != string::npos;
}

void tampilkanDaftarPasienSingkat(const AppData& data) {
    cout << "\nDaftar Pasien Terdaftar:\n";
    if (data.userCount == 0) {
        cout << "- Belum ada pasien terdaftar.\n";
        return;
    }

    for (int i = 0; i < data.userCount; i++) {
        cout << i + 1 << ". " << data.users[i].nama << " (" << data.users[i].username << ")\n";
    }
}
