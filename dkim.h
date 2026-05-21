#ifndef DKIM_H
#define DKIM_H

int sign_mail(const char *data,
              unsigned char **signature,
              size_t *sig_len);

int verify_mail(const char *data,
                unsigned char *signature,
                size_t sig_len);

char *base64_encode(const unsigned char *input, int length);
unsigned char *base64_decode(const char *input, int *out_len);

#endif
