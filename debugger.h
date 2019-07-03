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
 * File:   debugger.h
 * Author: emiel
 *
 * Created on January 26, 2017, 12:03 PM
 */

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "stdint.h"

#define STATUS_WIDTH COLS
#define STATUS_HEIGHT 3                         // takes borders into account, so we have 1 line for text.
#define MENU_WIDTH 15                           // menu is 30 characters wide
#define MENU_HEIGHT LINES - STATUS_HEIGHT       // take the whole screen
#define DATA_WIDTH COLS - MENU_WIDTH
#define DATA_HEIGHT LINES - STATUS_HEIGHT           

void debuggerLoop(void);
void initializeDebugger(void);
void debuggerCycle(void);
// the CPU breakpoints;

typedef struct _breakpoint {
    uint16_t addr;
    int selected;
    int enabled;
    const char* note;
    struct _breakpoint *next;
} breakpointItem;

typedef struct _menuItem {
    unsigned int index; // used to check which item belongs to which screen
    int selected;
    const char* name;
    struct _menuItem *next;
} menuItem;

typedef struct _setMem {
    uint16_t addr; // 16 bit adress
    uint8_t value; // value of memory at address
    int selected; // 1 = yes
    int enabled;
    const char* note; // small note attached
    struct _setMem *next; // next item in linked list
} setMemItem;

typedef struct _debuggerState {
    struct _breakpoint *breakpoints;
    struct _setMem *pSetMems;
    struct _menuItem *menuItems;
} state_t;



#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* DEBUGGER_H */

