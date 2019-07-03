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

/* 
 * File:   main.c
 * Author: emiel
 *
 * Created on January 25, 2017, 12:02 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "debugger.h"
#include "machine.h"
#include "graphics.h"
/*
 * 
 */
void init(const char* romPath, int bootstrapEnabled, const char* bootstrapPath);
void loop(void);

int main(int argc, char** argv) {
    ///home/emiel/gb/DENSHADEGO2.gbc
    init("./rom.gb", 1, "./bios.gb"); // initialize everything
    while (1) {
        loop();
    }
    return (EXIT_SUCCESS);
}

void init(const char* romPath, int bootstrapEnabled, const char* bootstrapPath) {
    initSDL();
    initializeDebugger();
    initializeEmulator(romPath, bootstrapEnabled, bootstrapPath);
}

void loop() {
    if (windowActive()) {   // if window is active, run, or switch to debugger mode
        executionCycle();
    } else {
        debuggerLoop();
        executionCycle();
        usleep(100000);
    }


}