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
  char *password = (char *)malloc(password_size);
  fread(password, 1, password_size, password_file);
  fclose(password_file);

  FILE *file = fopen(argv[2], "rb");
  long int file_size = get_file_size(file);
  char *content = (char *)malloc(file_size);
  fread(content, 1, file_size, file);
  fclose(file);

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init(&ctx);
  const EVP_CIPHER* cipher = EVP_des_ede3_cbc();
  const EVP_MD* md = EVP_md5();

  char salt[8];
  memset(salt, '\0', 8);
  RAND_bytes(salt, 8);
  char * key = (char *)malloc(32);
  memset(key, '\0', 32);
  char * iv = (char *)malloc(16);
  memset(iv, '\0', 16);
  char * out = NULL;
  EVP_CHECK(EVP_BytesToKey(cipher, md, salt, password, password_size, 1, key, iv), 24);

  EVP_CHECK(EVP_EncryptInit_ex(&ctx, cipher, 0, key, iv), 1);
  size_t ctxbz = EVP_CIPHER_CTX_block_size(&ctx);

  int outl;
  out = (char *)malloc(file_size + ctxbz);
  EVP_CHECK(EVP_EncryptUpdate(&ctx, out, &outl, content, file_size), 1);

  int outl2;
  EVP_CHECK(EVP_EncryptFinal_ex(&ctx, out + outl, &outl2), 1);

  EVP_CHECK(EVP_CIPHER_CTX_cleanup(&ctx), 1);

  fputs("Salted__", stdout);
  fwrite(salt, 1, 8, stdout);
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
