#ifndef AUTH_H
#define AUTH_H

#include <string>
#include "user.h"

bool usernameSudahAda(const AppData& data, const std::string& username);
bool buatAkunPasienOlehDokter(AppData& data, const std::string& userFilePath);
int loginPasien(const AppData& data);
bool loginDokter();

#endif
