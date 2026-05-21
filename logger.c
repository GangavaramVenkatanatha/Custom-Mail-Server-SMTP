#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../include/logger.h"

void log_event(const char *event) {

    FILE *fp = fopen("logs.txt", "a");
    if (!fp) return;

    time_t now = time(NULL);
    char *t = ctime(&now);
    t[strlen(t)-1] = '\0';

    fprintf(fp, "[%s] %s\n", t, event);
    fclose(fp);
}

