/*
 * Copyright (C) 2017 emiel
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include "crashhandler.h"
#include "machine.h"

void crash(void) {
    printf("\nunrecoverable error\ncreating crash dump.\n");
    FILE* file = fopen("/home/emiel/gb/dump.bin", "wb");
    fwrite(gbe.ram, 1, 0x10000, file);
    fclose(file);
    exit(1);
}