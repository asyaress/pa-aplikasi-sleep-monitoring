#ifndef USER_H
#define USER_H

#include <string>

const int MAX_USERS = 100;

struct User {
    std::string nama;
    std::string username;
    std::string password;
};

struct AppData {
    User users[MAX_USERS];
    int userCount;
};

#endif
