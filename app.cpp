#include "app.h"

#include <iostream>

#include "auth/auth.h"
#include "data_store.h"
#include "menus.h"

using namespace std;

void jalankanAplikasi() {
    const string USER_FILE_PATH = "users.txt";

    AppData data;
    initAppData(data);
    loadUsersFromFile(data, USER_FILE_PATH);

    int pilihan;
    do {
        tampilkanJudul();
        cout << "1. Login Pasien\n";
        cout << "2. Login Dokter\n";
        cout << "3. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR] Pilihan harus angka.\n";
            continue;
        }

        cin.ignore(10000, '\n');

        switch (pilihan) {
            case 1: {
                int userIndex = loginPasien(data);
                if (userIndex != -1) {
                    menuPasien(data, userIndex);
                }
                break;
            }
            case 2:
                if (loginDokter()) {
                    menuDokter(data, USER_FILE_PATH);
                }
                break;
            case 3:
                cout << "\nProgram selesai.\n";
                break;
            default:
                cout << "\n[ERROR] Menu utama tidak tersedia.\n";
                break;
        }
    } while (pilihan != 3);
}
