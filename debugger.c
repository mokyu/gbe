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
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "debugger.h"
#include "machine.h"

void drawWindow(void);
void drawContents(void);
void drawMenuItems(void);

void debuggerCycle(void);
void debuggerLoop(void);
void debuggerTasks(void);
void debuggerLogic(void);

void initializeDebugger(void);
void initializeContents(void);

void setMemPush(uint16_t addr, uint8_t val, char* comment, int enabled);
void setMemRemoveSelected(void);

void breakpointPush(uint16_t addr, char* comment, int enabled);
void breakpointRemoveSelected(void);

void createEntry(unsigned int target, char* str);

void refreshDisplay(void);

state_t state;
extern gb gbe;
WINDOW *menu; // menu bar shows user which menu is active.
WINDOW *data; // shows information on selected menu item
WINDOW *status; // status bar

/* Builds the whole window from scratch */
void drawWindow() {
    curs_set(0); // make cursor invisible if supported.
    initscr(); // initialize curses mode
    noecho(); // don't print entered characters to screen.
    cbreak(); // disable line buffering
    nodelay(stdscr, true); // don't wait for getch()
    menu = newwin(MENU_HEIGHT, MENU_WIDTH, 0, 0);
    data = newwin(DATA_HEIGHT, DATA_WIDTH, 0, MENU_WIDTH);
    status = newwin(STATUS_HEIGHT, STATUS_WIDTH, LINES - STATUS_HEIGHT, 0);
    box(menu, ACS_VLINE, ACS_HLINE); // ACS_<WHATEVER> is the line style, in this case, just a vertical and horizontal line
    box(data, ACS_VLINE, ACS_HLINE);
    box(status, ACS_VLINE, ACS_HLINE);
    drawContents();
    refresh();
    wrefresh(menu); // refresh windows after populating.
    wrefresh(data);
    wrefresh(status);
}

/* Draw the contents of all menus */
void drawContents() {
    int rowmax = getmaxy(data);
    /* Draw the menu */
    int col = 0;
    int row = 0;
    wmove(menu, 0, (col += 3)); // add an extra column to make title align with entries
    wprintw(menu, " MENU ");
    wmove(menu, row += 2, col);
    if (state.menuItems->selected == 1) { // print first menu item
        wattron(menu, A_STANDOUT);
        wprintw(menu, " %s ", state.menuItems->name);
        wattroff(menu, A_STANDOUT);
    } else {
        wprintw(menu, " %s ", state.menuItems->name);
    }
    menuItem *current = state.menuItems; // print following menu items
    while (current->next != NULL) {
        current = current->next;
        wmove(menu, row += 2, col);
        if (current->selected == 1) {
            wattron(menu, A_STANDOUT);
            wprintw(menu, " %s ", current->name);
            wattroff(menu, A_STANDOUT);
        } else {
            wprintw(menu, " %s ", current->name);
        }
    }
    /* Draw the menu stuff */
    col = 0;
    row = 0;
    wmove(data, 0, (col += 3) + 1);
    current = state.menuItems; // cycle through menu entries to find the active menu so we can print relevant information in the switch below
    if (current->selected != 1) {
        while (current->selected != 1) {
            current = current->next;
            if (current == NULL) {
                break;
            }
        }
    }
    row = 0;
    wprintw(data, " %s ", current->name);
    switch (current->index) {
        case 0: // status
            // print information in status window
            wmove(status, 1, 1);
            wprintw(status, "UP/DOWN arrows = navigate menu, q = quit, r = reset, F4 = 1 instruction cycle");
            wmove(data, row += 1, 1);
            wprintw(data, "Cartridge information");
            wmove(data, row += 2, 1);
            wprintw(data, "path:  %s", gbe.cart.rompath);
            wmove(data, row += 1, 1);
            wprintw(data, "size:  %zu bytes", gbe.cart.dumpSize);
            wmove(data, row += 1, 1);
            wprintw(data, "title: %s", gbe.cart.title);
            wmove(data, row += 1, 1);
            wprintw(data, "type:  0x%02x", gbe.cart.type);
            wmove(data, row += 1, 1);
            wprintw(data, "rom:   0x%02x    ram:   0x%02x", gbe.cart.romSize, gbe.cart.ramSize);
            wmove(data, row += 1, 1);
            wprintw(data, "CGB:   0x%02x    SGB:   0x%02x", gbe.cart.CGBFlag, gbe.cart.SGBFlag);
            wmove(data, row += 1, 1);
            wprintw(data, "DCode: 0x%02x    hChk:  0x%02x", gbe.cart.destinationCode);
            break;
        case 1: // CPU registers
            // print information in status window
            wmove(status, 1, 1);
            wprintw(status, "LEFT/RIGHT arrow = cycle children, SPACE = toggle, N = new, X = delete");
            // print cpu information
            wmove(data, row += 1, 1);
            wprintw(data, "CPU Registers", current->index);
            wmove(data, row += 2, 1);
            wprintw(data, "           A: 0x%02x F: 0x%02x", gbe.cpu.AF.hi, gbe.cpu.AF.lo);
            wmove(data, row += 1, 1);
            wprintw(data, "BC: 0x%04x B: 0x%02x C: 0x%02x", gbe.cpu.BC, gbe.cpu.AF.hi, gbe.cpu.AF.lo);
            wmove(data, row += 1, 1);
            wprintw(data, "DE: 0x%04x D: 0x%02x E: 0x%02x", gbe.cpu.DE, gbe.cpu.DE.hi, gbe.cpu.DE.lo);
            wmove(data, row += 1, 1);
            wprintw(data, "HL: 0x%04x H: 0x%02x L: 0x%02x", gbe.cpu.HL, gbe.cpu.HL.hi, gbe.cpu.HL.lo);
            wmove(data, row += 2, 1);
            wprintw(data, "SP: 0x%04x (size = %d) PC: 0x%04x", gbe.cpu.SP, (0xFFFE - gbe.cpu.SP.full), gbe.cpu.PC);
            wmove(data, row += 2, 1);
            wprintw(data, "-breakpoints-");
            wmove(data, row += 1, 1);
            // print until maxrows - rows = 1
            breakpointItem *item = state.breakpoints;
            int depth = 1;
            if (item == NULL) {
                return;
            }
            if ((rowmax - row) > 3) {
                while (item != NULL) {
                    if (item->selected) {
                        wmove(data, row += 1, 1);
                        wattron(data, A_STANDOUT);
                        if (item->enabled) {
                            wprintw(data, "[X]%d 0x%04x \"%s\"", depth, item->addr, item->note);
                        } else {
                            wprintw(data, "[ ]%d 0x%04x \"%s\"", depth, item->addr, item->note);
                        }
                        wmove(data, row += 1, 1);
                        wattroff(data, A_STANDOUT);
                        while (rowmax > (row + 2)) {
                            depth++;
                            item = item->next;
                            if (item != NULL) {
                                if (item->enabled) {
                                    wprintw(data, "[X]%d 0x%04x \"%s\"", depth, item->addr, item->note);
                                } else {
                                    wprintw(data, "[ ]%d 0x%04x \"%s\"", depth, item->addr, item->note);
                                }
                                wmove(data, row += 1, 1);
                            } else {
                                return;
                            }
                        }
                        if (item->next != NULL) {
                            wprintw(data, "+");
                            return;
                        }
                    } else { // only triggered when the first item isnt the highlighted one, overwrites the same line on repeat
                        wprintw(data, "-");
                        wmove(data, row, 1);
                    }
                    item = item->next;
                    depth++;
                }
            }
            break;
        case 2: // Memory
            // print information in status window
            wmove(status, 1, 1);
            wprintw(status, "LEFT/RIGHT arrow = cycle children, SPACE = toggle, N = new, X = delete");
            wmove(data, row += 1, 1);
            wprintw(data, "Interrupt Registers");
            wmove(data, row += 2, 1);
            wprintw(data, "TMA(0x%04x): 0x%02x\tTIMA(0x%04x): 0x%02x", TMA, gbe.ram[TMA], TIMA, gbe.ram[TIMA]);
            wmove(data, row += 1, 1);
            wprintw(data, "TAC(0x%04x): 0x%02x\tDIV(0x%04x):  0x%02x", TAC, gbe.ram[TAC], DIV, gbe.ram[DIV]);
            wmove(data, row += 1, 1);
            wprintw(data, "IF(0x%04x):  0x%02x\tIE(0x%04x):   0x%02x", IF, gbe.ram[IF], IE, gbe.ram[IE]);
            wmove(data, row += 2, 1);
            wprintw(data, "Current ROM bank: %d\tCurrent RAM bank: %d", gbe.cart.mbc.rombank, gbe.cart.mbc.rambank);
            wmove(data, row += 2, 1);
            wprintw(data, "-memory editor-");
            wmove(data, row += 2, 1);
            // TODOD, memory adding, editing and stuff
            depth = 1;
            setMemItem *memItem = state.pSetMems;
            if (memItem == NULL) {
                return;
            }
            if ((rowmax - row) > 3) {
                while (memItem != NULL) {
                    if (memItem->selected) {
                        wmove(data, row += 1, 1);
                        wattron(data, A_STANDOUT);
                        if (memItem->enabled) {
                            wprintw(data, "[X]%d 0x%04x 0x%02x(%02x) \"%s\"", depth, memItem->addr, gbe.ram[memItem->addr], memItem->value, memItem->note);
                        } else {
                            wprintw(data, "[ ]%d 0x%04x 0x%02x(%02x) \"%s\"", depth, memItem->addr, gbe.ram[memItem->addr], memItem->value, memItem->note);
                        }
                        wmove(data, row += 1, 1);
                        wattroff(data, A_STANDOUT);
                        while (rowmax > (row + 2)) {
                            depth++;
                            memItem = memItem->next;
                            if (memItem != NULL) {
                                if (memItem->enabled) {
                                    wprintw(data, "[X]%d 0x%04x 0x%02x(%02x) \"%s\"", depth, memItem->addr, gbe.ram[memItem->addr], memItem->value, memItem->note);
                                } else {
                                    wprintw(data, "[ ]%d 0x%04x 0x%02x(%02x) \"%s\"", depth, memItem->addr, gbe.ram[memItem->addr], memItem->value, memItem->note);
                                }
                                wmove(data, row += 1, 1);
                            } else {
                                return;
                            }
                        }
                        if (memItem->next != NULL) {
                            wprintw(data, "+");
                            return;
                        }
                    } else { // only triggered when the first memItem isnt the highlighted one, overwrites the same line on repeat
                        wprintw(data, "-");
                        wmove(data, row, 1);
                    }
                    memItem = memItem->next;
                    depth++;
                }
            }

            break;
        case 3: // Machine
            wmove(data, 1, 1);
            wprintw(data, "Machine Stuff %d", current->index);
            // TODO, Reset, set emulation speed?
            break;
        default:
            break;
    }
}

/* Ran the first time to initialize the struct containing all information
 * No need for a lot of dynamic memory allocation shenanigans */
void initializeContents() {
    keypad(menu, TRUE);
    const char *items[] = {"INFO", "CPU", "MEMORY", "MACHINE"}; /* All menu entries */
    state.pSetMems = NULL; // initialize later
    state.menuItems = NULL;
    /* Initialize menu items */
    state.menuItems = malloc(sizeof (menuItem));
    state.menuItems->index = 0;
    state.menuItems->selected = 1; // first item is always selected
    state.menuItems->name = items[0];
    state.menuItems->next = NULL;
    menuItem *current = state.menuItems;
    for (unsigned int i = 1; i < (sizeof (items) / sizeof (items[0])); i++) {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = malloc(sizeof (menuItem));
        current->next->index = i;
        current->next->name = items[i];
        current->next->selected = 0;
        current->next->next = NULL;
    }
}

void debuggerLogic() {
    int ch;
    menuItem *current = state.menuItems;
    menuItem *previous = NULL;
    menuItem *selectedMenu = state.menuItems;

    setMemItem *currentMem = state.pSetMems;
    setMemItem *previousMem = NULL;

    breakpointItem *currentBp = state.breakpoints;
    breakpointItem *previousBp = NULL;

    char result[100]; // used for storing yad output
    FILE * fp; // used for yad dialog
    if (current->selected != 1) {
        while (current->selected != 1) {
            current = current->next;
            if (current == NULL) {
                break;
            }
        }
        selectedMenu = current;
    }
    current = state.menuItems; // reset current
    ch = getch();
    switch (ch) {
        case 66: // down key
            while (current->next != NULL && current->selected == 0) {
                current = current->next;
            }
            current->selected = 0; // remove selection
            if (current->next == NULL) {
                state.menuItems->selected = 1;
            } else {
                current->next->selected = 1;
            }
            break;
        case 65: // up key
            while (current->next != NULL && current->selected == 0) {
                previous = current;
                current = current->next;
            }
            current->selected = 0; // remove selection
            if (previous == NULL) { // when going up as the first item, highlight the last
                current = state.menuItems;
                while (current->next != NULL) {
                    current = current->next;
                };
                current->selected = 1;
            } else {
                previous->selected = 1;
            }
            break;
        case 68: // left
            switch (selectedMenu->index) {
                case 1: // cpu
                    if (currentBp == NULL) {
                        return;
                    }
                    while (currentBp->next != NULL && currentBp->selected == 0) {
                        previousBp = currentBp;
                        currentBp = currentBp->next;
                    }
                    currentBp->selected = 0; // remove selection
                    if (previousBp == NULL) { // when going up as the first item do nothing.
                        while (currentBp->next != NULL) {
                            currentBp = currentBp->next;
                        }
                        currentBp->selected = 1;

                    } else {
                        previousBp->selected = 1;
                    }
                    break;
                case 2: // memory;
                    if (currentMem == NULL) {
                        return;
                    }
                    while (currentMem->next != NULL && currentMem->selected == 0) {
                        previousMem = currentMem;
                        currentMem = currentMem->next;
                    }
                    currentMem->selected = 0; // remove selection
                    if (previousMem == NULL) { // when going up as the first item do nothing.
                        while (currentMem->next != NULL) {
                            currentMem = currentMem->next;
                        }
                        currentMem->selected = 1;

                    } else {
                        previousMem->selected = 1;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 67: // right key
            switch (selectedMenu->index) {
                case 1: // cpu
                    if (currentBp == NULL) {
                        return;
                    }
                    while (currentBp->next != NULL && currentBp->selected == 0) {
                        currentBp = currentBp->next;
                    }
                    currentBp->selected = 0; // remove selection
                    if (currentBp->next == NULL) {
                        state.breakpoints->selected = 1; // select the first item;
                    } else {
                        currentBp->next->selected = 1; // select the next item
                    }
                    break;
                case 2:
                    if (currentMem == NULL) {
                        return;
                    }
                    while (currentMem->next != NULL && currentMem->selected == 0) {
                        currentMem = currentMem->next;
                    }
                    currentMem->selected = 0; // remove selection
                    if (currentMem->next == NULL) {
                        state.pSetMems->selected = 1; // select the first item;
                    } else {
                        currentMem->next->selected = 1; // select the next item
                    }
                    break;
                default:
                    break;
            }
            break;
        case 110: // n
        case 78: // N
            switch (selectedMenu->index) {
                case 1:
                    endwin();
                    fp = popen("yad --title \"breakpoint\"\
                                --text=\"create new breakpoint\"\
                                --form\
                                --field=\"address (0xFFFF syntax)\"\
                                --field=\"comment\"\
                                --field=\"active:CHK\"", "r");
                    if (fp == NULL) {
                        printf("Failed to execute yad.\n");
                        exit(1);
                    }
                    while (fgets(result, sizeof (result) - 1, fp) != NULL) {
                    }
                    createEntry(selectedMenu->index, result);
                    endwin();
                    refresh();
                    break;
                case 2:
                    endwin();
                    fp = popen("yad --title \"memory\"\
                                --text=\"modify new address\"\
                                --form\
                                --field=\"address (0xFFFF syntax)\"\
                                --field=\"value (0xFF syntax)\"\
                                --field=\"comment\"\
                                --field=\"active:CHK\"", "r");
                    if (fp == NULL) {
                        printf("Failed to execute yad.\n");
                        exit(1);
                    }
                    while (fgets(result, sizeof (result) - 1, fp) != NULL) {
                    }
                    createEntry(selectedMenu->index, result);
                    endwin();
                    refresh();
                    break;
                default:
                    break;
            }
            break;
        case 120: // x
        case 88: // X
            switch (selectedMenu->index) {
                case 1:
                    breakpointRemoveSelected();
                    break;
                case 2:
                    setMemRemoveSelected();
                    break;
                default:
                    break;
            }
            break;
        case 32: // spacebar
            switch (selectedMenu->index) {
                case 1:
                    while (currentBp != NULL) {
                        if (currentBp->selected) {
                            currentBp->enabled = !currentBp->enabled;
                        }
                        currentBp = currentBp->next;
                    }
                    break;
                case 2:
                    while (currentMem != NULL) {
                        if (currentMem->selected) {
                            currentMem->enabled = !currentMem->enabled;
                        }
                        currentMem = currentMem->next;
                    }
                    break;
                default:
                    break;
            }
            break;
        case -1:
            break; // This means getch didn't find anything, just return and  don't update.
        default: // Assume(tm) it's a sigwinch and refresh
            break;
    }
    refreshDisplay();
}

/* Runs every instruction cycle, checks if breakpoints are reached and restores "frozen" memory*/
void debuggerTasks() {
    // TODO Create this.
    // Simply loop over all entries, and set the memory regions where applicable.
    // for the breakpoints, simply iterate over them and check if PC matches with their addr
}

/* push something on the memory linked list */
void setMemPush(uint16_t addr, uint8_t val, char* comment, int enabled) {
    if (state.pSetMems == NULL) { // initialize when empty
        state.pSetMems = malloc(sizeof (setMemItem));
        state.pSetMems->addr = addr;
        state.pSetMems->value = val;
        state.pSetMems->note = comment;
        state.pSetMems->enabled = enabled;
        state.pSetMems->selected = 1; // the only item, mark as selected.
        state.pSetMems->next = NULL;
    } else {
        setMemItem *current = state.pSetMems;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = malloc(sizeof (setMemItem));
        current->next->addr = addr;
        current->next->value = val;
        current->next->note = comment;
        current->next->selected = 0;
        current->next->next = NULL;
    }
}

void breakpointPush(uint16_t addr, char* comment, int enabled) {
    if (state.breakpoints == NULL) {
        state.breakpoints = malloc(sizeof (breakpointItem));
        state.breakpoints->addr = addr;
        state.breakpoints->note = comment;
        state.breakpoints->enabled = enabled;
        state.breakpoints->selected = 1; // first item has to be selected.
    } else {
        breakpointItem *current = state.breakpoints;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = malloc(sizeof (breakpointItem));
        current->next->addr = addr;
        current->next->note = comment;
        current->next->enabled = enabled;
        current->next->enabled = 0;
    }
}

void setMemRemoveSelected() {
    if (state.pSetMems == NULL) { // can't remove what's not there
        return;
    }
    setMemItem *previous = NULL;
    setMemItem *current = state.pSetMems;
    int depth = 0;
    do {
        if (current->selected == 1) {
            if (previous == NULL) { // Only used for when we need to remove the root entry
                current = current->next;
                free(state.pSetMems);
                state.pSetMems = current;
                if (current != NULL) {
                    current->selected = 1;
                }
                return;
            } else {
                previous->next = current->next;
                free(current);
                previous->selected = 1;
                return;
            }
        } else {
            previous = current;
            current = current->next;
        }
        depth++;


    } while (current != NULL);

}

void breakpointRemoveSelected() {
    if (state.breakpoints == NULL) { // can't remove what's not there
        return;
    }
    breakpointItem *previous = NULL;
    breakpointItem *current = state.breakpoints;
    int depth = 0;
    do {
        if (current->selected == 1) {
            if (previous == NULL) { // Only used for when we need to remove the root entry
                current = current->next;
                free(state.breakpoints);
                state.breakpoints = current;
                if (current != NULL) {
                    current->selected = 1;
                }
                return;
            } else {
                previous->next = current->next;
                free(current);
                previous->selected = 1;
                return;
            }
        } else {
            previous = current;
            current = current->next;
        }
        depth++;


    } while (current != NULL);

}

void createEntry(unsigned int target, char* str) {
    char *end = NULL, *found = NULL, *comment = NULL, *check = NULL ;
    int depth, active = 0;
    uint8_t val = 0;
    uint16_t addr = 0;
    depth = 0;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0) // All spaces?
        return;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator
    *(end + 1) = 0;
    switch (target) {
        case 1:
            while ((found = strsep(&str, "|")) != NULL) {
                switch (depth) {
                    case 0: // address
                        addr = (uint16_t) strtol(found, &check, 0);
                        if (*check != '\0' || addr == 0) {
                            printf("Invalid input for \"address\" entry.\n Press return to continue\n");
                            getchar();
                            return;
                        }
                        break;
                    case 1:
                        comment = strdup(found);
                        break;
                    case 2:
                        if (strcmp(found, "FALSE") == 0) {
                            active = 0;
                        } else {
                            active = 1;
                        }
                        break;
                    default:
                        // do nothing
                        break;

                }
                depth++;

            }
            breakpointPush(addr, comment, active); // all values are always filled, don't worry.
            break;
        case 2:
            while ((found = strsep(&str, "|")) != NULL) {
                switch (depth) {
                    case 0: // address
                        addr = (uint16_t) strtol(found, &check, 0);
                        if (*check != '\0' || addr == 0) {
                            printf("Invalid input for \"address\" entry.\n Press return to continue\n");
                            getchar();
                            return;
                        }
                        break;
                    case 1:
                        val = (uint8_t) strtol(found, &check, 0);
                        if (*check != '\0' || addr == 0) {
                            printf("Invalid input for \"value\" entry.\n Press return to continue\n");
                            getchar();
                            return;
                        }
                        break;
                    case 2:
                        comment = strdup(found);
                        break;
                    case 3:
                        if (strcmp(found, "FALSE") == 0) {
                            active = 0;
                        } else {
                            active = 1;
                        }
                        break;
                    default:
                        // do nothing
                        break;

                }
                depth++;

            }
            setMemPush(addr, val, comment, active); // all values are always filled, you can safely ignore this warning.
            break;
        default:
            break;
    }
}

void initializeDebugger() {
    initializeContents();
    drawWindow();
}

void refreshDisplay() {
    // clear screen
    wclear(menu);
    wclear(data);
    wclear(status);
    // recreate our window objects
    menu = newwin(MENU_HEIGHT, MENU_WIDTH, 0, 0);
    data = newwin(DATA_HEIGHT, DATA_WIDTH, 0, MENU_WIDTH);
    status = newwin(STATUS_HEIGHT, STATUS_WIDTH, LINES - STATUS_HEIGHT, 0);
    // redraw window borders
    box(menu, ACS_VLINE, ACS_HLINE); // ACS_<WHATEVER> is the line style, in this case, just a vertical and horizontal line
    box(data, ACS_VLINE, ACS_HLINE);
    box(status, ACS_VLINE, ACS_HLINE);
    // draw window contents
    drawContents();
    // flush to terminal
    wrefresh(menu);
    wrefresh(data);
    wrefresh(status);
}

/* When debugger is active */
void debuggerLoop() {
    debuggerTasks();
    debuggerLogic();
    usleep(16000);
}

/* used when emulator is running in background*/
void debuggerCycle() {
    debuggerTasks();
}