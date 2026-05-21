#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/auth.h"

int user_exists(const char *username) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;

    char user[100], pass[100];

    while (fscanf(fp, "%s %s", user, pass) != EOF) {
        if (strcmp(user, username) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int register_user(const char *username, const char *password) {

    if (!username || !password) return 0;
    if (strlen(username) == 0 || strlen(password) == 0) return 0;
    if (strchr(username, ' ') || strchr(password, ' ')) return 0;
    if (user_exists(username)) return 0;

    FILE *fp = fopen("users.txt", "a");
    if (!fp) return 0;

    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);

    char path[256];

    mkdir("mailboxes", 0777);

    sprintf(path, "mailboxes/%s", username);
    mkdir(path, 0777);

    sprintf(path, "mailboxes/%s/inbox", username);
    mkdir(path, 0777);

    sprintf(path, "mailboxes/%s/sent", username);
    mkdir(path, 0777);

    return 1;
}

int verify_user(const char *username, const char *password) {

    if (!username || !password) return 0;
    if (strlen(username) == 0 || strlen(password) == 0) return 0;

    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;

    char user[100], pass[100];

    while (fscanf(fp, "%s %s", user, pass) != EOF) {
        if (strcmp(user, username) == 0 &&
            strcmp(pass, password) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}
