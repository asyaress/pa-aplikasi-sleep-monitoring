#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <string>
#include "user.h"

void initAppData(AppData& data);
void loadUsersFromFile(AppData& data, const std::string& filePath);
bool saveUsersToFile(const AppData& data, const std::string& filePath);

#endif
