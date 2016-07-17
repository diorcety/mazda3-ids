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
#include <windows.h>
#include <stdio.h>

typedef char *(__stdcall *f_get_key)();

typedef void (__stdcall *f_free_key)(char *);

int main(int argc, char *argv[]) {
    int ret = 0;
    if (argc != 3) {
        fprintf(stderr, "No arg\n");
        return -1;
    }
    SetDllDirectory(argv[1]);

    FILE *password_file = fopen(argv[2], "wb");

    HMODULE dllHandle = LoadLibrary("Fnpss.dll");
    if (dllHandle == NULL) {
        fprintf(stderr, "Can't load library %d", GetLastError());
        ret = 1;
        goto end;
    }

    void *activate_license = GetProcAddress(dllHandle, "Fnpss_ActivateLicense");
    if (activate_license == NULL) {
        fprintf(stderr, "Can't load function %d", GetLastError());
    }

    f_get_key get_key = (f_get_key) (activate_license + (0x10011340 - 0x1001CF80));
    f_free_key free_key = (f_free_key) (activate_license + (0x10011860 - 0x1001CF80));
    char *data = get_key();
    fwrite(data + 0x4, 1, 0x7C, password_file);
    free_key(data);
    end:
    if (dllHandle != NULL) {
        FreeLibrary(dllHandle);
    }
    if (password_file != NULL) {
        fclose(password_file);
    }
    return ret;
}
