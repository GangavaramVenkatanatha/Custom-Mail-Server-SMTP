#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "../include/dkim.h"

int sign_mail(const char *data,
              unsigned char **signature,
              size_t *sig_len)
{
    FILE *fp = fopen("private.pem", "r");
    if (!fp) return 0;

    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!pkey) return 0;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey);
    EVP_DigestSignUpdate(ctx, data, strlen(data));
    EVP_DigestSignFinal(ctx, NULL, sig_len);

    *signature = malloc(*sig_len);
    EVP_DigestSignFinal(ctx, *signature, sig_len);

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return 1;
}

int verify_mail(const char *data,
                unsigned char *signature,
                size_t sig_len)
{
    FILE *fp = fopen("public.pem", "r");
    if (!fp) return 0;

    EVP_PKEY *pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);
    if (!pkey) return 0;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();

    EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey);
    EVP_DigestVerifyUpdate(ctx, data, strlen(data));

    int ret = EVP_DigestVerifyFinal(ctx, signature, sig_len);

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    return ret == 1;
}

char *base64_encode(const unsigned char *input, int length)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char *buff = malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;

    BIO_free_all(b64);
    return buff;
}

unsigned char *base64_decode(const char *input, int *out_len)
{
    BIO *b64, *bmem;
    int len = strlen(input);

    unsigned char *buffer = malloc(len);
    memset(buffer, 0, len);

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf((void*)input, len);
    bmem = BIO_push(b64, bmem);

    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
    *out_len = BIO_read(bmem, buffer, len);

    BIO_free_all(bmem);
    return buffer;
}
