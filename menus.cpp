#include "menus.h"

#include <ctime>
#include <iostream>
#include <sstream>

#include "auth/auth.h"
#include "data_store.h"

using namespace std;

void tampilkanJudul() {
    cout << "\n===============================================\n";
    cout << " APLIKASI MONITORING & JOURNALING TIDUR (C++)\n";
    cout << "===============================================\n";
}

static string inputBarisMenu(const string& pesan) {
    string nilai;
    cout << pesan;
    getline(cin, nilai);
    return nilai;
}

static string ambilJamSekarang() {
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

static bool berisiKarakterTerlarang(const string& nilai) {
    return nilai.find('|') != string::npos;
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

static void isiJurnalMalam(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    if (data.sleepRecordCount >= MAX_SLEEP_RECORDS) {
        cout << "\n[ERROR] Kapasitas data tidur penuh.\n";
        return;
    }

    cout << "\n--- JURNAL MALAM ---\n";

    string tanggal = inputBarisMenu("Tanggal (contoh 30-04-2026): ");
    string catatanMalam = inputBarisMenu("Catatan sebelum tidur       : ");

    if (tanggal.empty() || catatanMalam.empty()) {
        cout << "\n[ERROR] Semua field wajib diisi.\n";
        return;
    }

    if (berisiKarakterTerlarang(tanggal) || berisiKarakterTerlarang(catatanMalam)) {
        cout << "\n[ERROR] Karakter '|' tidak boleh dipakai.\n";
        return;
    }

    SleepRecord& record = data.sleepRecords[data.sleepRecordCount];
    record.usernamePasien = data.users[userIndex].username;
    record.tanggal = tanggal;
    record.jamMulaiSesiMalam = ambilJamSekarang();
    record.catatanMalam = catatanMalam;
    record.jamMulaiTidurFinal = "-";
    record.jamBangun = "-";
    record.jumlahTerbangun = 0;
    record.totalTerjagaMenit = 0;
    record.kualitasTidur = 0;
    record.kondisiBangun = "-";
    record.sudahInputPagi = false;
    data.sleepRecordCount++;

    if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
        cout << "\n[ERROR] Data jurnal malam gagal disimpan.\n";
        return;
    }

    cout << "\n[SUKSES] Jurnal malam tersimpan.\n";
    cout << "Jam mulai sesi malam otomatis: " << record.jamMulaiSesiMalam << "\n";
}

static int cariJurnalMalamBelumLengkap(const AppData& data, const string& usernamePasien) {
    for (int i = data.sleepRecordCount - 1; i >= 0; i--) {
        if (data.sleepRecords[i].usernamePasien == usernamePasien && !data.sleepRecords[i].sudahInputPagi) {
            return i;
        }
    }
    return -1;
}

static void inputDataPagi(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    string usernamePasien = data.users[userIndex].username;
    int recordIndex = cariJurnalMalamBelumLengkap(data, usernamePasien);

    if (recordIndex == -1) {
        cout << "\n[ERROR] Belum ada jurnal malam yang perlu dilengkapi.\n";
        return;
    }

    SleepRecord& record = data.sleepRecords[recordIndex];

    cout << "\n--- INPUT DATA SETELAH BANGUN ---\n";
    cout << "Tanggal jurnal malam : " << record.tanggal << "\n";
    cout << "Jam sesi malam       : " << record.jamMulaiSesiMalam << "\n";

    string jamMulaiTidurFinal = inputBarisMenu("Jam mulai tidur final        : ");
    string jamBangun = inputBarisMenu("Jam bangun                   : ");
    int jumlahTerbangun;
    int totalTerjagaMenit;
    int kualitasTidur;

    cout << "Jumlah terbangun             : ";
    cin >> jumlahTerbangun;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n[ERROR] Jumlah terbangun harus angka.\n";
        return;
    }

    cout << "Total lama terjaga (menit)   : ";
    cin >> totalTerjagaMenit;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n[ERROR] Total terjaga harus angka.\n";
        return;
    }

    cout << "Kualitas tidur (1-10)        : ";
    cin >> kualitasTidur;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n[ERROR] Kualitas tidur harus angka.\n";
        return;
    }

    cin.ignore(10000, '\n');
    string kondisiBangun = inputBarisMenu("Kondisi saat bangun          : ");

    if (jamMulaiTidurFinal.empty() || jamBangun.empty() || kondisiBangun.empty()) {
        cout << "\n[ERROR] Semua field wajib diisi.\n";
        return;
    }

    if (jumlahTerbangun < 0 || totalTerjagaMenit < 0 || kualitasTidur < 1 || kualitasTidur > 10) {
        cout << "\n[ERROR] Input angka tidak valid.\n";
        return;
    }

    if (berisiKarakterTerlarang(jamMulaiTidurFinal) || berisiKarakterTerlarang(jamBangun) || berisiKarakterTerlarang(kondisiBangun)) {
        cout << "\n[ERROR] Karakter '|' tidak boleh dipakai.\n";
        return;
    }

    record.jamMulaiTidurFinal = jamMulaiTidurFinal;
    record.jamBangun = jamBangun;
    record.jumlahTerbangun = jumlahTerbangun;
    record.totalTerjagaMenit = totalTerjagaMenit;
    record.kualitasTidur = kualitasTidur;
    record.kondisiBangun = kondisiBangun;
    record.sudahInputPagi = true;

    if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
        cout << "\n[ERROR] Data pagi gagal disimpan.\n";
        return;
    }

    cout << "\n[SUKSES] Data pagi tersimpan dan tergabung dengan jurnal malam.\n";
}

void menuPasien(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    int pilihan;

    do {
        cout << "\n========== MENU PASIEN ==========";
        cout << "\nLogin sebagai : " << data.users[userIndex].nama;
        cout << "\n1. Lihat profil singkat";
        cout << "\n2. Isi jurnal malam";
        cout << "\n3. Input data setelah bangun";
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
                cout << "\n--- PROFIL PASIEN ---\n";
                cout << "Nama     : " << data.users[userIndex].nama << "\n";
                cout << "Username : " << data.users[userIndex].username << "\n";
                break;
            case 2:
                isiJurnalMalam(data, userIndex, sleepRecordFilePath);
                break;
            case 3:
                inputDataPagi(data, userIndex, sleepRecordFilePath);
                break;
            case 4:
                cout << "\nLogout pasien berhasil.\n";
                break;
            default:
                cout << "\n[ERROR] Menu tidak tersedia.\n";
                break;
        }
    } while (pilihan != 4);
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
