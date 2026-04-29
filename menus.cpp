#include "menus.h"

#include <iostream>

#include "auth/auth.h"

using namespace std;

void tampilkanJudul() {
    cout << "\n===============================================\n";
    cout << " APLIKASI MONITORING & JOURNALING TIDUR (C++)\n";
    cout << "===============================================\n";
}

static void tampilkanDaftarPasienSingkat(const AppData& data) {
    cout << "\nDaftar Pasien Terdaftar:\n";
    if (data.userCount == 0) {
        cout << "- Belum ada pasien terdaftar.\n";
        return;
    }

    for (int i = 0; i < data.userCount; i++) {
        cout << i + 1 << ". " << data.users[i].nama << " (" << data.users[i].username << ")\n";
    }
}

void menuPasien(const AppData& data, int userIndex) {
    int pilihan;

    do {
        cout << "\n========== MENU PASIEN ==========";
        cout << "\nLogin sebagai : " << data.users[userIndex].nama;
        cout << "\n1. Lihat profil singkat";
        cout << "\n2. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Pilihan harus angka.\n";
            continue;
        }

        cin.ignore(10000, '\n');

        switch (pilihan) {
            case 1:
                cout << "\n--- PROFIL PASIEN ---\n";
                cout << "Nama     : " << data.users[userIndex].nama << "\n";
                cout << "Username : " << data.users[userIndex].username << "\n";
                break;
            case 2:
                cout << "\nLogout pasien berhasil.\n";
                break;
            default:
                cout << "\n[ERROR] Menu tidak tersedia.\n";
                break;
        }
    } while (pilihan != 2);
}

void menuDokter(AppData& data, const string& userFilePath) {
    int pilihan;

    do {
        cout << "\n========== MENU DOKTER ==========";
        cout << "\n1. Buat akun pasien";
        cout << "\n2. Lihat jumlah pasien";
        cout << "\n3. Lihat daftar pasien";
        cout << "\n4. Logout";
        cout << "\nPilihan: ";
        cin >> pilihan;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Pilihan harus angka.\n";
            continue;
        }

        cin.ignore(10000, '\n');

        switch (pilihan) {
            case 1:
                buatAkunPasienOlehDokter(data, userFilePath);
                break;
            case 2:
                cout << "\nTotal pasien terdaftar: " << data.userCount << "\n";
                break;
            case 3:
                tampilkanDaftarPasienSingkat(data);
                break;
            case 4:
                cout << "\nLogout dokter berhasil.\n";
                break;
            default:
                cout << "\n[ERROR] Menu tidak tersedia.\n";
                break;
        }
    } while (pilihan != 4);
}
