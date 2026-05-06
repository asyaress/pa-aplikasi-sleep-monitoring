#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <numeric>

using namespace std;

struct SleepRecord {
    string tanggal;
    string jam_mulai_tidur;
    string jam_bangun;
    int jumlah_terbangun;
    int total_terjaga_menit;
    int kualitas;           // skala 1-10
    string kondisi_bangun;
};

void tampilGaris(char c = '-', int panjang = 70) {
    for (int i = 0; i < panjang; i++) cout << c;
    cout << "\n";
}

string labelKualitas(int k) {
    if (k >= 8) return "Baik";
    if (k >= 5) return "Cukup";
    return "Buruk";
}

void tampilHeader() {
    tampilGaris('=');
    cout << setw(38) << "SLEEP TRACKER - LAPORAN MINGGUAN" << "\n";
    tampilGaris('=');
    cout << "\n";
}

void tampilRingkasan(const vector<SleepRecord>& data) {
    double total_kualitas = 0;
    double total_terbangun = 0;
    double total_terjaga = 0;
    int hari_baik = 0;

    for (const auto& r : data) {
        total_kualitas   += r.kualitas;
        total_terbangun  += r.jumlah_terbangun;
        total_terjaga    += r.total_terjaga_menit;
        if (r.kualitas >= 8) hari_baik++;
    }

    int n = data.size();
    cout << "  RINGKASAN MINGGU INI\n";
    tampilGaris('-');
    cout << "  Rata-rata kualitas tidur  : "
         << fixed << setprecision(1) << total_kualitas / n << " / 10\n";
    cout << "  Rata-rata terbangun/malam : "
         << fixed << setprecision(1) << total_terbangun / n << " kali\n";
    cout << "  Rata-rata terjaga/malam   : "
         << fixed << setprecision(0) << total_terjaga / n << " menit\n";
    cout << "  Hari tidur baik (>=8)     : "
         << hari_baik << " dari " << n << " hari\n";
    tampilGaris('-');
    cout << "\n";
}

void tampilDetailHarian(const vector<SleepRecord>& data) {
    cout << "  DETAIL HARIAN\n";
    tampilGaris('-');

    for (const auto& r : data) {
        cout << "  Tanggal          : " << r.tanggal << "\n";
        cout << "  Jam mulai tidur  : " << r.jam_mulai_tidur << "\n";
        cout << "  Jam bangun       : " << r.jam_bangun << "\n";
        cout << "  Jumlah terbangun : " << r.jumlah_terbangun << " kali\n";
        cout << "  Total terjaga    : " << r.total_terjaga_menit << " menit\n";
        cout << "  Kualitas tidur   : " << r.kualitas << "/10"
             << "  [" << labelKualitas(r.kualitas) << "]\n";
        cout << "  Kondisi bangun   : " << r.kondisi_bangun << "\n";
        tampilGaris('-');
    }
}

int main() {
    vector<SleepRecord> data = {
        { "Senin,   28 Apr 2025", "22:30", "06:00", 2, 18, 8,  "Segar dan bersemangat"         },
        { "Selasa,  29 Apr 2025", "23:45", "05:30", 3, 35, 5,  "Agak mengantuk, butuh kopi"    },
        { "Rabu,    30 Apr 2025", "22:00", "05:00", 1, 10, 9,  "Sangat segar, mood bagus"       },
        { "Kamis,    1 Mei 2025", "01:20", "05:40", 5, 52, 3,  "Sangat lelah, kepala berat"    },
        { "Jumat,    2 Mei 2025", "23:00", "05:30", 2, 20, 7,  "Cukup segar, sedikit ngantuk"  },
        { "Sabtu,    3 Mei 2025", "21:30", "05:30", 1,  8, 9,  "Segar, tidur paling nyenyak"   },
        { "Minggu,   4 Mei 2025", "00:30", "05:42", 4, 40, 4,  "Lemas, susah fokus pagi hari"  },
    };

    tampilHeader();
    tampilRingkasan(data);
    tampilDetailHarian(data);

    cout << "\n";
    return 0;
}