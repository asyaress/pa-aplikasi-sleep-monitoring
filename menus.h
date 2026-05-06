#ifndef MENUS_H
#define MENUS_H

#include "user.h"

void tampilkanJudul();
void menuPasien(AppData& data, int userIndex, const std::string& sleepRecordFilePath);
void menuDokter(AppData& data, const std::string& userFilePath, const std::string& sleepRecordFilePath);

#endif
