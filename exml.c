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

void encrypt(char *a1, unsigned int size) {
    int i;
    for (i = 0; i < size; ++i) {
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
    char *content = (char *) malloc(file_size);
    fread(content, 1, file_size, file);
    fclose(file);

    encrypt(content, file_size);

    fwrite(content, 1, file_size, stdout);
    fflush(stdout);

    free(content);
    return 0;
}
