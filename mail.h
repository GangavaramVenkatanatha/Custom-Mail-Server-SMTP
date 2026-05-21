#ifndef MAIL_H
#define MAIL_H

void save_mail(const char *sender,
               const char *receiver,
               const char *data);


int count_messages(const char *username,
                   const char *mode);


int retrieve_message(const char *username,
                     const char *mode,
                     int msg_no,
                     char *output);

#endif
