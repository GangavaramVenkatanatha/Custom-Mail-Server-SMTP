#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../include/auth.h"
#include "../include/mail.h"
#include "../include/dkim.h"


/* ======================================================
   SAVE MAIL (WITH DKIM SIGNING)
   ====================================================== */
void save_mail(const char *sender,
               const char *receiver,
               const char *data)
{
    if (!user_exists(receiver))
        return;

    unsigned char *signature = NULL;
    size_t sig_len;

    if (!sign_mail(data, &signature, &sig_len)) {
        printf("DKIM signing failed\n");
        return;
    }

    char *encoded_sig = base64_encode(signature, sig_len);

    char path[256];
    char filename[300];
    int count = 1;

    /* ===== Receiver Inbox ===== */
    sprintf(path, "mailboxes/%s/inbox", receiver);

    while (1) {
        sprintf(filename, "%s/%d.txt", path, count);
        FILE *check = fopen(filename, "r");
        if (!check) break;
        fclose(check);
        count++;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp,
            "From: %s\n"
            "To: %s\n"
            "DKIM-Signature: %s\n\n"
            "%s\n",
            sender, receiver, encoded_sig, data);

    fclose(fp);

    /* ===== Sender Sent ===== */
    sprintf(path, "mailboxes/%s/sent", sender);
    count = 1;

    while (1) {
        sprintf(filename, "%s/%d.txt", path, count);
        FILE *check = fopen(filename, "r");
        if (!check) break;
        fclose(check);
        count++;
    }

    fp = fopen(filename, "w");
    if (!fp) return;

    fprintf(fp,
            "From: %s\n"
            "To: %s\n"
            "DKIM-Signature: %s\n\n"
            "%s\n",
            sender, receiver, encoded_sig, data);

    fclose(fp);

    free(signature);
    free(encoded_sig);
}
int count_messages(const char *username,
                   const char *mode)
{
    char path[256];
    sprintf(path, "mailboxes/%s/%s", username, mode);

    DIR *dir = opendir(path);
    if (!dir) return 0;

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt"))
            count++;
    }

    closedir(dir);
    return count;
}


int retrieve_message(const char *username,
                     const char *mode,
                     int msg_no,
                     char *output)
{
    char filename[256];

    sprintf(filename,
            "mailboxes/%s/%s/%d.txt",
            username, mode, msg_no);

    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    char line[1024];
    output[0] = '\0';

    while (fgets(line, sizeof(line), fp)) {
        strcat(output, line);
    }

    fclose(fp);
    return 1;
}
