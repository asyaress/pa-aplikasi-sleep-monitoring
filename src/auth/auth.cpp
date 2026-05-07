#include "auth/auth.h"

#include <iostream>

#include "data_store.h"

using namespace std;

const string DOKTER_USERNAME = "dokter";
const string DOKTER_PASSWORD = "dokter123";

static string inputBaris(const string& pesan) {
    string nilai;
    cout << pesan;
    getline(cin, nilai);
    return nilai;
}

bool usernameSudahAda(const AppData& data, const string& username) {
    for (int i = 0; i < data.userCount; i++) {
        if (data.users[i].username == username) {
            return true;
        }
    }
    return false;
}

bool buatAkunPasienOlehDokter(AppData& data, const string& userFilePath) {
    if (data.userCount >= MAX_USERS) {
        cout << "\n[ERROR] Kapasitas user penuh.\n";
        return false;
    }

    cout << "\n--- BUAT AKUN PASIEN (OLEH DOKTER) ---\n";

    string nama = inputBaris("Nama lengkap   : ");
    string username = inputBaris("Username       : ");
    string password = inputBaris("Password       : ");

    if (nama.empty() || username.empty() || password.empty()) {
        cout << "\n[ERROR] Semua field wajib diisi.\n";
        return false;
    }

    if (password.length() < 6) {
        cout << "\n[ERROR] Password minimal 6 karakter.\n";
        return false;
    }

    if (username.find('|') != string::npos || nama.find('|') != string::npos || password.find('|') != string::npos) {
        cout << "\n[ERROR] Karakter '|' tidak boleh dipakai.\n";
        return false;
    }

    if (usernameSudahAda(data, username)) {
        cout << "\n[ERROR] Username sudah dipakai. Gunakan username lain.\n";
        return false;
    }

    data.users[data.userCount].nama = nama;
    data.users[data.userCount].username = username;
    data.users[data.userCount].password = password;
    data.userCount++;

    if (!saveUsersToFile(data, userFilePath)) {
        cout << "\n[ERROR] Data user gagal disimpan ke file.\n";
        return false;
    }

    cout << "\n[SUKSES] Akun pasien berhasil dibuat dan tersimpan di file TXT.\n";
    return true;
}

int loginPasien(const AppData& data) {
    cout << "\n--- LOGIN PASIEN ---\n";
    string username = inputBaris("Username : ");
    string password = inputBaris("Password : ");

    for (int i = 0; i < data.userCount; i++) {
        if (data.users[i].username == username && data.users[i].password == password) {
            cout << "\n[SUKSES] Login pasien berhasil.\n";
            return i;
        }
    }

    cout << "\n[ERROR] Login pasien gagal. Username/password salah.\n";
    return -1;
}

bool loginDokter() {
    cout << "\n--- LOGIN DOKTER ---\n";
    string username = inputBaris("Username : ");
    string password = inputBaris("Password : ");

    if (username == DOKTER_USERNAME && password == DOKTER_PASSWORD) {
        cout << "\n[SUKSES] Login dokter berhasil.\n";
        return true;
    }

    cout << "\n[ERROR] Login dokter gagal. Username/password salah.\n";
    return false;
}


