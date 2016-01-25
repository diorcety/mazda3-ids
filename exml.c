#include <stdio.h>
#include <stdlib.h>

void encrypt(char *a1, unsigned int size) {
  int i;
  for (i = 0; i < size; ++i ) {
    a1[i] = ((a1[i] & 0xF) << 4) | ((a1[i] & 0xF0) >> 4);
  }
}

long int get_file_size(FILE *file) {
  fseek(file, 0L, SEEK_END);
  long int l = ftell(file);
  fseek(file, 0L, SEEK_SET);
  return l;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "No arg\n");
    return -1;
  }
  FILE *file = fopen(argv[1], "rb");
  long int file_size = get_file_size(file);
  char *content = (char *)malloc(file_size);
  fread(content, 1, file_size, file);
  fclose(file);

  encrypt(content, file_size);

  fwrite(content, 1, file_size, stdout);
  fflush(stdout);

  free(content);
  return 0;
}
