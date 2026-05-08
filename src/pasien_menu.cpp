#include "pasien_menu.h"

#include <iostream>

#include "data_store.h"
#include "menu_common.h"
#include "sleep_metrics.h"

using namespace std;

static int cariJurnalMalamBelumLengkap(const AppData& data, const string& usernamePasien) {
    for (int i = data.sleepRecordCount - 1; i >= 0; i--) {
        if (data.sleepRecords[i].usernamePasien == usernamePasien && !data.sleepRecords[i].sudahInputPagi) {
            return i;
        }
    }
    return -1;
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

static void lihatsleepdairypasien(const AppData& data, int userIndex) {
    string usernamePasien = data.users[userIndex].username;
    bool adaRecord = false;

    cout << "\n--- DATA SLEEP DIARY PASIEN ---\n";
    for (int i = 0; i < data.sleepRecordCount; i++) {
        if (data.sleepRecords[i].usernamePasien == usernamePasien) {
            const SleepRecord& record = data.sleepRecords[i];
            cout << "\n================= DATA SLEEP DIARY [" << i + 1 << "] =================\n";
            cout << "\nTanggal          : " << record.tanggal << "\n";
            cout << "Jam sesi malam   : " << record.jamMulaiSesiMalam << "\n";
            cout << "Catatan malam    : " << record.catatanMalam << "\n";
            if (record.sudahInputPagi) {
                cout << "Jam mulai tidur  : " << record.jamMulaiTidurFinal << "\n";
                cout << "Jam bangun       : " << record.jamBangun << "\n";
                cout << "Jumlah terbangun : " << record.jumlahTerbangun << "\n";
                cout << "Total terjaga    : " << record.totalTerjagaMenit << " menit\n";
                cout << "Kualitas tidur   : " << record.kualitasTidur << "/10\n";
                cout << "Kondisi bangun   : " << record.kondisiBangun << "\n";
                if (recordSiapDihitung(record)) {
                    tampilkanIndikatorSleepDiary(record);
                } else {
                    cout << "[INFO] Indikator belum bisa dihitung karena format jam tidak valid.\n";
                }
            } else {
                cout << "[INFO] Data pagi belum diinput untuk tanggal ini.\n";
            }
            adaRecord = true;
        }
    }

    if (!adaRecord) {
        cout << "[INFO] Belum ada data sleep diary untuk pasien ini.\n";
    }
}

static void editjurnal(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    string usernamePasien = data.users[userIndex].username;

    int daftarIndex[MAX_SLEEP_RECORDS];
    int jumlah = 0;

    cout << "\n--- PILIH JURNAL YANG INGIN DIEDIT ---\n";
    for (int i = 0; i < data.sleepRecordCount; i++) {
        if (data.sleepRecords[i].usernamePasien == usernamePasien) {
            string status = data.sleepRecords[i].sudahInputPagi ? "[Lengkap]" : "[Belum ada data pagi]";
            cout << jumlah + 1 << ". Tanggal: " << data.sleepRecords[i].tanggal << " " << status << "\n";
            daftarIndex[jumlah++] = i;
        }
    }

    if (jumlah == 0) {
        cout << "\n[ERROR] Tidak ada jurnal yang ditemukan.\n";
        return;
    }

    int pilih;
    cout << "Pilih nomor jurnal (Enter untuk batal): ";
    string inputpilihan;
    getline(cin, inputpilihan);

    if (inputpilihan.empty()) {
        cout << "\n[INFO] Edit dibatalkan.\n";
        return;
    }

    try {
        pilih = stoi(inputpilihan);
    } catch (...) {
        cout << "\n[ERROR] Pilihan harus berupa angka.\n";
        return;
    }

    if (pilih < 1 || pilih > jumlah) {
        cout << "\n[ERROR] Pilihan tidak valid.\n";
        return;
    }

    SleepRecord& record = data.sleepRecords[daftarIndex[pilih - 1]];

    cout << "\n--- EDIT JURNAL TANGGAL: " << record.tanggal << " ---\n";
    cout << "1. Edit jurnal malam (tanggal & catatan malam)\n";
    cout << "2. Edit data pagi (jam tidur, jam bangun, dll)\n";
    cout << "3. Batal\n";
    cout << "Pilihan: ";

    int pilihanEdit;
    cin >> pilihanEdit;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "\n[ERROR] Pilihan harus angka.\n";
        return;
    }
    cin.ignore(10000, '\n');

    if (pilihanEdit == 1) {
        cout << "\n--- EDIT JURNAL MALAM ---\n";
        cout << "Tanggal saat ini     : " << record.tanggal << "\n";
        cout << "Catatan saat ini     : " << record.catatanMalam << "\n";
        cout << "(Tekan Enter untuk tidak mengubah)\n\n";

        string tanggalBaru = inputBarisMenu("Tanggal baru         : ");
        string catatanBaru = inputBarisMenu("Catatan malam baru   : ");

        if (berisiKarakterTerlarang(tanggalBaru) || berisiKarakterTerlarang(catatanBaru)) {
            cout << "\n[ERROR] Karakter '|' tidak boleh dipakai.\n";
            return;
        }

        if (!tanggalBaru.empty()) record.tanggal = tanggalBaru;
        if (!catatanBaru.empty()) record.catatanMalam = catatanBaru;

        if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
            cout << "\n[ERROR] Gagal menyimpan perubahan.\n";
            return;
        }
        cout << "\n[SUKSES] Jurnal malam berhasil diperbarui.\n";
    } else if (pilihanEdit == 2) {
        if (!record.sudahInputPagi) {
            cout << "\n[INFO] Data pagi belum pernah diisi. Gunakan menu 'Input data setelah bangun'.\n";
            return;
        }

        cout << "\n--- EDIT DATA PAGI ---\n";
        cout << "Jam mulai tidur saat ini   : " << record.jamMulaiTidurFinal << "\n";
        cout << "Jam bangun saat ini        : " << record.jamBangun << "\n";
        cout << "Jumlah terbangun saat ini  : " << record.jumlahTerbangun << "\n";
        cout << "Total terjaga saat ini     : " << record.totalTerjagaMenit << " menit\n";
        cout << "Kualitas tidur saat ini    : " << record.kualitasTidur << "/10\n";
        cout << "Kondisi bangun saat ini    : " << record.kondisiBangun << "\n";
        cout << "(Tekan Enter untuk tidak mengubah)\n\n";

        string jamTidurBaru = inputBarisMenu("Jam mulai tidur baru       : ");
        string jamBangunBaru = inputBarisMenu("Jam bangun baru            : ");
        string inputTerbangun = inputBarisMenu("Jumlah terbangun baru      : ");
        string inputTerjaga = inputBarisMenu("Total terjaga menit baru   : ");
        string inputKualitas = inputBarisMenu("Kualitas tidur baru (1-10) : ");
        string kondisiBaru = inputBarisMenu("Kondisi bangun baru        : ");

        if (!jamTidurBaru.empty() && !formatJamValid(jamTidurBaru)) {
            cout << "\n[ERROR] Format jam mulai tidur harus HH:MM.\n";
            return;
        }
        if (!jamBangunBaru.empty() && !formatJamValid(jamBangunBaru)) {
            cout << "\n[ERROR] Format jam bangun harus HH:MM.\n";
            return;
        }

        int terbangunBaru = record.jumlahTerbangun;
        if (!inputTerbangun.empty()) {
            try {
                terbangunBaru = stoi(inputTerbangun);
            } catch (...) {
                cout << "\n[ERROR] Jumlah terbangun harus angka.\n";
                return;
            }
            if (terbangunBaru < 0) {
                cout << "\n[ERROR] Jumlah terbangun tidak boleh negatif.\n";
                return;
            }
        }

        int terjagaBaru = record.totalTerjagaMenit;
        if (!inputTerjaga.empty()) {
            try {
                terjagaBaru = stoi(inputTerjaga);
            } catch (...) {
                cout << "\n[ERROR] Total terjaga harus angka.\n";
                return;
            }
            if (terjagaBaru < 0) {
                cout << "\n[ERROR] Total terjaga tidak boleh negatif.\n";
                return;
            }
        }

        int kualitasBaru = record.kualitasTidur;
        if (!inputKualitas.empty()) {
            try {
                kualitasBaru = stoi(inputKualitas);
            } catch (...) {
                cout << "\n[ERROR] Kualitas tidur harus angka.\n";
                return;
            }
            if (kualitasBaru < 1 || kualitasBaru > 10) {
                cout << "\n[ERROR] Kualitas tidur harus antara 1-10.\n";
                return;
            }
        }

        if (berisiKarakterTerlarang(jamTidurBaru) || berisiKarakterTerlarang(jamBangunBaru) ||
            berisiKarakterTerlarang(kondisiBaru)) {
            cout << "\n[ERROR] Karakter '|' tidak boleh dipakai.\n";
            return;
        }

        string jamTidurFinal = jamTidurBaru.empty() ? record.jamMulaiTidurFinal : jamTidurBaru;
        string jamBangunFinal = jamBangunBaru.empty() ? record.jamBangun : jamBangunBaru;

        if (!formatJamValid(jamTidurFinal) || !formatJamValid(jamBangunFinal)) {
            cout << "\n[ERROR] Data jam tersimpan tidak valid. Periksa dan isi ulang format HH:MM.\n";
            return;
        }

        int durasiDasar = hitungSelisihMenit(konversiJamKeMenit(jamTidurFinal), konversiJamKeMenit(jamBangunFinal));
        if (terjagaBaru > durasiDasar) {
            cout << "\n[ERROR] Total terjaga melebihi durasi dari mulai tidur final hingga bangun.\n";
            return;
        }

        record.jamMulaiTidurFinal = jamTidurFinal;
        record.jamBangun = jamBangunFinal;
        record.jumlahTerbangun = terbangunBaru;
        record.totalTerjagaMenit = terjagaBaru;
        record.kualitasTidur = kualitasBaru;
        if (!kondisiBaru.empty()) record.kondisiBangun = kondisiBaru;

        if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
            cout << "\n[ERROR] Gagal menyimpan perubahan.\n";
            return;
        }
        cout << "\n[SUKSES] Data pagi berhasil diperbarui.\n";
        tampilkanIndikatorSleepDiary(record);
    } else if (pilihanEdit == 3) {
        cout << "\n[INFO] Edit dibatalkan.\n";
    } else {
        cout << "\n[ERROR] Pilihan tidak valid.\n";
    }
}

static void hapusdatajurnal(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    string usernamePasien = data.users[userIndex].username;

    cout << "\n--- PILIH JURNAL YANG MAU DIHAPUS ---\n";

    int daftarIndex[MAX_SLEEP_RECORDS];
    int jumlah = 0;

    for (int i = 0; i < data.sleepRecordCount; i++) {
        if (data.sleepRecords[i].usernamePasien == usernamePasien) {
            cout << jumlah + 1 << ". Tanggal: " << data.sleepRecords[i].tanggal << endl;
            daftarIndex[jumlah++] = i;
        }
    }

    if (jumlah == 0) {
        cout << "\n[ERROR] Tidak ada jurnal yang ditemukan.\n";
        return;
    }

    int pilih;
    cout << "Pilih nomor jurnal (Enter untuk batal): ";
    string inputpilihan;
    getline(cin, inputpilihan);

    if (inputpilihan.empty()) {
        cout << "\n[INFO] Edit dibatalkan.\n";
        return;
    }

    try {
        pilih = stoi(inputpilihan);
    } catch (...) {
        cout << "\n[ERROR] Pilihan harus berupa angka.\n";
        return;
    }

    if (pilih < 1 || pilih > jumlah) {
        cout << "\n[ERROR] Pilihan tidak valid.\n";
        return;
    }

    int recordIndex = daftarIndex[pilih - 1];

    string inputKonfirmasi;
    cout << "Yakin ingin menghapus jurnal tanggal " << data.sleepRecords[recordIndex].tanggal << "? (y/n): ";

    do {
        getline(cin, inputKonfirmasi);
    } while (inputKonfirmasi != "y" && inputKonfirmasi != "Y" &&
             inputKonfirmasi != "n" && inputKonfirmasi != "N");

    if (inputKonfirmasi == "n" || inputKonfirmasi == "N") {
        cout << "\n[INFO] Penghapusan dibatalkan.\n";
        return;
    }

    for (int i = recordIndex; i < data.sleepRecordCount - 1; i++) {
        data.sleepRecords[i] = data.sleepRecords[i + 1];
    }
    data.sleepRecordCount--;

    if (!saveSleepRecordsToFile(data, sleepRecordFilePath)) {
        cout << "\n[ERROR] Gagal menyimpan perubahan setelah hapus.\n";
        return;
    }

    cout << "\n[SUKSES] Jurnal berhasil dihapus.\n";
}

void menuPasien(AppData& data, int userIndex, const string& sleepRecordFilePath) {
    int pilihan;

    do {
        cout << "\n========== MENU PASIEN ==========";
        cout << "\nLogin sebagai : " << data.users[userIndex].nama << endl;
        cout << "\n1. Lihat profil singkat";
        cout << "\n2. Isi jurnal malam";
        cout << "\n3. Input data setelah bangun";
        cout << "\n4. Lihat seluruh data sleep diary";
        cout << "\n5. Edit jurnal";
        cout << "\n6. Hapus data jurnal";
        cout << "\n7. Logout";
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
                lihatsleepdairypasien(data, userIndex);
                break;
            case 5:
                editjurnal(data, userIndex, sleepRecordFilePath);
                break;
            case 6:
                hapusdatajurnal(data, userIndex, sleepRecordFilePath);
                break;
            case 7:
                cout << "\nLogout pasien berhasil.\n";
                break;
            default:
                cout << "\n[ERROR] Menu tidak tersedia.\n";
                break;
        }
    } while (pilihan != 7);
}
