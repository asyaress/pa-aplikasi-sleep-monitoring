#ifndef MENUS_H
#define MENUS_H

#include "user.h"

void tampilkanJudul();
void menuPasien(const AppData& data, int userIndex);
void menuDokter(AppData& data, const std::string& userFilePath);

#endif
