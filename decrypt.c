/*
  Copyright (C) 2016 Yann Diorcet

  This file is part of IDS.  IDS is free software: you can
  redistribute it and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation, version 2.
 
  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
 
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

#define EVP_CHECK(q, r) {int ret = q;if(ret != r){fprintf(stderr, "Error executing %s result: %d != %d\n", #q, ret, r); goto fail;}}
#define TRY_FREE(b) {if(b!=NULL){free(b);}}

long int get_file_size(FILE *file) {
    fseek(file, 0L, SEEK_END);
    long int l = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return l;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        fprintf(stderr, "No arg\n");
        return -1;
    }
    FILE *password_file = fopen(argv[1], "rb");
    long int password_size = get_file_size(password_file);
    char *password = (char *) malloc(password_size);
    fread(password, 1, password_size, password_file);
    fclose(password_file);

    FILE *file = fopen(argv[2], "rb");
    long int file_size = get_file_size(file);
    char *content = (char *) malloc(file_size);
    fread(content, 1, file_size, file);
    fclose(file);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = EVP_des_ede3_cbc();
    const EVP_MD *md = EVP_md5();

    char salt[8];
    memcpy(salt, content + 8, 8);
    char *key = (char *) malloc(32);
    char *iv = (char *) malloc(16);
    char *out = NULL;
    EVP_CHECK(EVP_BytesToKey(cipher, md, salt, password, password_size, 1, key, iv), 24);

    EVP_CHECK(EVP_DecryptInit_ex(ctx, cipher, 0, key, iv), 1);
    size_t ctxbz = EVP_CIPHER_CTX_block_size(ctx);

    int outl;
    out = (char *) malloc(file_size + ctxbz);
    EVP_CHECK(EVP_DecryptUpdate(ctx, out, &outl, content + 0x10, file_size - 0x10), 1);

    int outl2;
    EVP_CHECK(EVP_DecryptFinal_ex(ctx, out + outl, &outl2), 1);

    EVP_CIPHER_CTX_free(ctx);

    fwrite(out, 1, outl + outl2, stdout);
    fflush(stdout);
    goto exit;

    fail:
    ret = 1;
    exit:
    TRY_FREE(password);
    TRY_FREE(key);
    TRY_FREE(iv);
    TRY_FREE(out);
    free(content);
    return ret;
}
