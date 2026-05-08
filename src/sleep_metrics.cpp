#include "sleep_metrics.h"

#include <iomanip>
#include <iostream>

using namespace std;

bool formatJamValid(const string& jam) {
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

int konversiJamKeMenit(const string& jam) {
    int jamAngka = (jam[0] - '0') * 10 + (jam[1] - '0');
    int menitAngka = (jam[3] - '0') * 10 + (jam[4] - '0');
    return jamAngka * 60 + menitAngka;
}

int hitungSelisihMenit(int menitMulai, int menitSelesai) {
    if (menitSelesai < menitMulai) {
        menitSelesai += 24 * 60;
    }
    return menitSelesai - menitMulai;
}

int hitungSOL(const SleepRecord& record) {
    int mulaiSesiMalam = konversiJamKeMenit(record.jamMulaiSesiMalam);
    int mulaiTidurFinal = konversiJamKeMenit(record.jamMulaiTidurFinal);
    return hitungSelisihMenit(mulaiSesiMalam, mulaiTidurFinal);
}

int hitungWASO(const SleepRecord& record) {
    return record.totalTerjagaMenit;
}

int hitungNumberOfAwakenings(const SleepRecord& record) {
    return record.jumlahTerbangun;
}

int hitungWaktuDiTempatTidur(const SleepRecord& record) {
    int mulaiSesiMalam = konversiJamKeMenit(record.jamMulaiSesiMalam);
    int jamBangun = konversiJamKeMenit(record.jamBangun);
    return hitungSelisihMenit(mulaiSesiMalam, jamBangun);
}

int hitungTST(const SleepRecord& record) {
    int mulaiTidurFinal = konversiJamKeMenit(record.jamMulaiTidurFinal);
    int jamBangun = konversiJamKeMenit(record.jamBangun);
    int durasiDariMulaiTidurFinal = hitungSelisihMenit(mulaiTidurFinal, jamBangun);
    int tst = durasiDariMulaiTidurFinal - hitungWASO(record);
    return tst < 0 ? 0 : tst;
}

double hitungSE(const SleepRecord& record) {
    int tib = hitungWaktuDiTempatTidur(record);
    if (tib <= 0) {
        return 0.0;
    }
    return static_cast<double>(hitungTST(record)) / tib * 100.0;
}

bool recordSiapDihitung(const SleepRecord& record) {
    if (!record.sudahInputPagi) {
        return false;
    }

    return formatJamValid(record.jamMulaiSesiMalam) &&
           formatJamValid(record.jamMulaiTidurFinal) &&
           formatJamValid(record.jamBangun);
}

void tampilkanIndikatorSleepDiary(const SleepRecord& record) {
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

void tampilkanIndikatorSingkatRecord(const SleepRecord& record) {
    cout << "  SOL=" << hitungSOL(record) << " mnt";
    cout << ", WASO=" << hitungWASO(record) << " mnt";
    cout << ", NoA=" << hitungNumberOfAwakenings(record);
    cout << ", TST=" << hitungTST(record) << " mnt";
    cout << ", SE=" << fixed << setprecision(2) << hitungSE(record) << "%\n";
    cout << defaultfloat;
}
