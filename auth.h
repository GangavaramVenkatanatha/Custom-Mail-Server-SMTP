#ifndef AUTH_H
#define AUTH_H

int register_user(const char *username, const char *password);
int verify_user(const char *username, const char *password);
int user_exists(const char *username);

#endif
