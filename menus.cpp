#include "menus.h"

#include <ctime>
#include <iomanip>
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

static bool formatJamValid(const string& jam) {
    if (jam.length() != 5 || jam[2] != ':') {
        return false;
    }

    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            continue;
        }
        if (jam[i] < '0' || jam[i] > '9') {
            return false;
        }
    }

    int jamAngka = (jam[0] - '0') * 10 + (jam[1] - '0');
    int menitAngka = (jam[3] - '0') * 10 + (jam[4] - '0');
    return jamAngka >= 0 && jamAngka <= 23 && menitAngka >= 0 && menitAngka <= 59;
}

static int konversiJamKeMenit(const string& jam) {
    int jamAngka = (jam[0] - '0') * 10 + (jam[1] - '0');
    int menitAngka = (jam[3] - '0') * 10 + (jam[4] - '0');
    return jamAngka * 60 + menitAngka;
}

static int hitungSelisihMenit(int menitMulai, int menitSelesai) {
    if (menitSelesai < menitMulai) {
        menitSelesai += 24 * 60;
    }
    return menitSelesai - menitMulai;
}

static int hitungSOL(const SleepRecord& record) {
    int mulaiSesiMalam = konversiJamKeMenit(record.jamMulaiSesiMalam);
    int mulaiTidurFinal = konversiJamKeMenit(record.jamMulaiTidurFinal);
    return hitungSelisihMenit(mulaiSesiMalam, mulaiTidurFinal);
}

static int hitungWASO(const SleepRecord& record) {
    return record.totalTerjagaMenit;
}

static int hitungNumberOfAwakenings(const SleepRecord& record) {
    return record.jumlahTerbangun;
}

static int hitungWaktuDiTempatTidur(const SleepRecord& record) {
    int mulaiSesiMalam = konversiJamKeMenit(record.jamMulaiSesiMalam);
    int jamBangun = konversiJamKeMenit(record.jamBangun);
    return hitungSelisihMenit(mulaiSesiMalam, jamBangun);
}

static int hitungTST(const SleepRecord& record) {
    int mulaiTidurFinal = konversiJamKeMenit(record.jamMulaiTidurFinal);
    int jamBangun = konversiJamKeMenit(record.jamBangun);
    int durasiDariMulaiTidurFinal = hitungSelisihMenit(mulaiTidurFinal, jamBangun);
    int tst = durasiDariMulaiTidurFinal - hitungWASO(record);
    return tst < 0 ? 0 : tst;
}

static double hitungSE(const SleepRecord& record) {
    int tib = hitungWaktuDiTempatTidur(record);
    if (tib <= 0) {
        return 0.0;
    }
    return static_cast<double>(hitungTST(record)) / tib * 100.0;
}

static void tampilkanIndikatorSleepDiary(const SleepRecord& record) {
    int sol = hitungSOL(record);
    int waso = hitungWASO(record);
    int noa = hitungNumberOfAwakenings(record);
    int tib = hitungWaktuDiTempatTidur(record);
    int tst = hitungTST(record);
    double se = hitungSE(record);

    cout << "\n--- HASIL INDIKATOR SLEEP DIARY ---\n";
    cout << "SOL (menit)                  : " << sol << "\n";
    cout << "WASO (menit)                 : " << waso << "\n";
    cout << "Number of Awakenings         : " << noa << "\n";
    cout << "Waktu di tempat tidur (TIB)  : " << tib << " menit\n";
    cout << "Total Sleep Time (TST)       : " << tst << " menit\n";
    cout << "Sleep Efficiency (SE)        : " << fixed << setprecision(2) << se << "%\n";
    cout << defaultfloat;
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

static bool recordSiapDihitung(const SleepRecord& record) {
    if (!record.sudahInputPagi) {
        return false;
    }

    return formatJamValid(record.jamMulaiSesiMalam) &&
           formatJamValid(record.jamMulaiTidurFinal) &&
           formatJamValid(record.jamBangun);
}

static void tampilkanIndikatorSingkatRecord(const SleepRecord& record) {
    cout << "  SOL=" << hitungSOL(record) << " mnt";
    cout << ", WASO=" << hitungWASO(record) << " mnt";
    cout << ", NoA=" << hitungNumberOfAwakenings(record);
    cout << ", TST=" << hitungTST(record) << " mnt";
    cout << ", SE=" << fixed << setprecision(2) << hitungSE(record) << "%\n";
    cout << defaultfloat;
}

static int pilihPasienUntukMonitoring(const AppData& data) {
    if (data.userCount == 0) {
        cout << "\n[ERROR] Belum ada pasien terdaftar.\n";
        return -1;
    }

    tampilkanDaftarPasienSingkat(data);
    cout << "Pilih nomor pasien: ";
    int pilihan;
    cin >> pilihan;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n[ERROR] Nomor pasien harus angka.\n";
        return -1;
    }

    cin.ignore(10000, '\n');

    if (pilihan < 1 || pilihan > data.userCount) {
        cout << "\n[ERROR] Nomor pasien tidak valid.\n";
        return -1;
    }

    return pilihan - 1;
}

static void tampilkanSemuaDataTidurPasien(const AppData& data, const User& pasien) {
    cout << "\n--- DATA TIDUR PASIEN ---\n";
    cout << "Nama     : " << pasien.nama << "\n";
    cout << "Username : " << pasien.username << "\n";

    int nomor = 0;
    for (int i = 0; i < data.sleepRecordCount; i++) {
        const SleepRecord& record = data.sleepRecords[i];
        if (record.usernamePasien != pasien.username) {
            continue;
        }

        nomor++;
        cout << "\n[" << nomor << "] Tanggal : " << record.tanggal << "\n";
        cout << "Jam sesi malam      : " << record.jamMulaiSesiMalam << "\n";
        cout << "Jam tidur final     : " << record.jamMulaiTidurFinal << "\n";
        cout << "Jam bangun          : " << record.jamBangun << "\n";
        cout << "Jumlah terbangun    : " << record.jumlahTerbangun << "\n";
        cout << "Total terjaga       : " << record.totalTerjagaMenit << " menit\n";
        cout << "Kualitas tidur      : " << record.kualitasTidur << "/10\n";
        cout << "Kondisi bangun      : " << record.kondisiBangun << "\n";

        if (recordSiapDihitung(record)) {
            tampilkanIndikatorSingkatRecord(record);
        } else {
            cout << "Indikator           : Belum bisa dihitung (data belum lengkap).\n";
        }
    }

    if (nomor == 0) {
        cout << "\nBelum ada data tidur untuk pasien ini.\n";
    }
}

static void tampilkanRingkasanPasien(const AppData& data, const User& pasien) {
    int jumlahRecordLengkap = 0;
    int totalTST = 0;
    double totalSE = 0.0;
    int totalKualitas = 0;
    int totalTerbangun = 0;

    for (int i = 0; i < data.sleepRecordCount; i++) {
        const SleepRecord& record = data.sleepRecords[i];
        if (record.usernamePasien != pasien.username || !recordSiapDihitung(record)) {
            continue;
        }

        jumlahRecordLengkap++;
        totalTST += hitungTST(record);
        totalSE += hitungSE(record);
        totalKualitas += record.kualitasTidur;
        totalTerbangun += record.jumlahTerbangun;
    }

    cout << "\n--- RINGKASAN PASIEN ---\n";
    cout << "Nama     : " << pasien.nama << "\n";
    cout << "Username : " << pasien.username << "\n";

    if (jumlahRecordLengkap == 0) {
        cout << "Belum ada data tidur lengkap untuk diringkas.\n";
        return;
    }

    cout << "Jumlah data lengkap          : " << jumlahRecordLengkap << "\n";
    cout << "Rata-rata TST                : "
         << fixed << setprecision(2) << static_cast<double>(totalTST) / jumlahRecordLengkap
         << " menit\n";
    cout << "Rata-rata SE                 : "
         << fixed << setprecision(2) << totalSE / jumlahRecordLengkap
         << "%\n";
    cout << "Rata-rata kualitas tidur     : "
         << fixed << setprecision(2) << static_cast<double>(totalKualitas) / jumlahRecordLengkap
         << "/10\n";
    cout << "Frekuensi terbangun (rata2)  : "
         << fixed << setprecision(2) << static_cast<double>(totalTerbangun) / jumlahRecordLengkap
         << " kali/malam\n";
    cout << "Frekuensi terbangun (total)  : " << totalTerbangun << " kali\n";
    cout << defaultfloat;
}

static int hapusSemuaDataTidurPasien(AppData& data, const string& usernamePasien) {
    int tulis = 0;
    int awal = data.sleepRecordCount;

    for (int baca = 0; baca < data.sleepRecordCount; baca++) {
        if (data.sleepRecords[baca].usernamePasien != usernamePasien) {
            data.sleepRecords[tulis] = data.sleepRecords[baca];
            tulis++;
        }
    }

    data.sleepRecordCount = tulis;
    return awal - tulis;
}

static void monitoringPasienOlehDokter(AppData& data, const string& sleepRecordFilePath) {
    int pasienIndex = pilihPasienUntukMonitoring(data);
    if (pasienIndex == -1) {
        return;
    }

    User pasienDipilih = data.users[pasienIndex];
    int pilihan;

    do {
        cout << "\n===== MONITORING PASIEN =====\n";
        cout << "Pasien: " << pasienDipilih.nama << " (" << pasienDipilih.username << ")\n";
        cout << "1. Lihat semua data tidur pasien\n";
        cout << "2. Lihat ringkasan pasien\n";
        cout << "3. Hapus semua data tidur pasien\n";
        cout << "4. Kembali ke menu dokter\n";
        cout << "Pilihan: ";
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
                tampilkanSemuaDataTidurPasien(data, pasienDipilih);
                break;
            case 2:
                tampilkanRingkasanPasien(data, pasienDipilih);
                break;
            case 3: {
                string konfirmasi = inputBarisMenu("Ketik Y untuk konfirmasi hapus semua data tidur pasien: ");
                if (konfirmasi != "Y" && konfirmasi != "y") {
                    cout << "\nAksi hapus dibatalkan.\n";
                    break;
                }

                int jumlahDihapus = hapusSemuaDataTidurPasien(data, pasienDipilih.username);
                if (jumlahDihapus == 0) {
                    cout << "\nTidak ada data tidur yang bisa dihapus.\n";
                    break;
                }

                if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
                    cout << "\n[ERROR] Gagal menyimpan penghapusan data tidur.\n";
                    break;
                }

                cout << "\n[SUKSES] " << jumlahDihapus << " data tidur pasien berhasil dihapus.\n";
                break;
            }
            case 4:
                cout << "\nKembali ke menu dokter.\n";
                break;
            default:
                cout << "\n[ERROR] Menu monitoring tidak tersedia.\n";
                break;
        }
    } while (pilihan != 4);
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

    if (!formatJamValid(jamMulaiTidurFinal) || !formatJamValid(jamBangun)) {
        cout << "\n[ERROR] Format jam harus HH:MM (24 jam).\n";
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

    int mulaiTidurFinalMenit = konversiJamKeMenit(record.jamMulaiTidurFinal);
    int jamBangunMenit = konversiJamKeMenit(record.jamBangun);
    int durasiTidurDasar = hitungSelisihMenit(mulaiTidurFinalMenit, jamBangunMenit);
    if (record.totalTerjagaMenit > durasiTidurDasar) {
        cout << "\n[ERROR] Total terjaga melebihi durasi dari mulai tidur final hingga bangun.\n";
        record.jamMulaiTidurFinal = "-";
        record.jamBangun = "-";
        record.jumlahTerbangun = 0;
        record.totalTerjagaMenit = 0;
        record.kualitasTidur = 0;
        record.kondisiBangun = "-";
        record.sudahInputPagi = false;
        return;
    }

    if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
        cout << "\n[ERROR] Data pagi gagal disimpan.\n";
        return;
    }

    cout << "\n[SUKSES] Data pagi tersimpan dan tergabung dengan jurnal malam.\n";
    tampilkanIndikatorSleepDiary(record);
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

void menuDokter(AppData& data, const string& userFilePath, const string& sleepRecordFilePath) {
    int pilihan;

    do {
        cout << "\n========== MENU DOKTER ==========";
        cout << "\n1. Buat akun pasien";
        cout << "\n2. Lihat jumlah pasien";
        cout << "\n3. Lihat daftar pasien";
        cout << "\n4. Monitoring pasien";
        cout << "\n5. Logout";
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
                monitoringPasienOlehDokter(data, sleepRecordFilePath);
                break;
            case 5:
                cout << "\nLogout dokter berhasil.\n";
                break;
            default:
                cout << "\n[ERROR] Menu tidak tersedia.\n";
                break;
        }
    } while (pilihan != 5);
}
