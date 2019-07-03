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
#include <string.h>
#include <unistd.h>

#include "machine.h"
#include "memory.h"
#include "opcodes.h"
#include "logger.h"
#include "debugger.h"
#include "graphics.h"
#include "interrupt.h"
#include "timers.h"

gb gbe;

void initializeEmulator(const char* rompath, int boostrapEnabled, const char* bootstrapPath);

void executionCycle(void);

void fetchDecodeExecute(void);

void stackPush(uint16_t val);
uint16_t stackPop(void);

void initializeEmulator(const char* rompath, int bootstrapEnabled, const char* bootstrapPath) {
    logger(LOG_INFO, "Initializing GBE");
    gbe.cpu.PC = 0x0; // the only sure value of the processor on startup.
    gbe.cpu.clock_freq = 1024; // and the default cpu speed
    if (bootstrapEnabled) { // we load the bootstrap rom instead of setting all values manually.
        logger(LOG_INFO, "Loading bootstrap");
        FILE *handle = fopen(bootstrapPath, "rb");
        if (handle == NULL) {
            logger(LOG_ERR, "read fail\n");
            crash();
        }
        fseek(handle, 0, SEEK_END);
        long size = ftell(handle);
        rewind(handle);
        size_t result = fread(gbe.ram, 1, (unsigned) size, handle);
        if (result != (unsigned) size) {
            logger(LOG_ERR, "Failed to load file into buffer");
        }
    } else { // set everything manually
        logger(LOG_INFO, "Setting machine to post bootstrap gb");
        gbe.cpu.AF.full = 0x01B0;
        gbe.cpu.BC.full = 0x0013;
        gbe.cpu.DE.full = 0x00D8;
        gbe.cpu.HL.full = 0x014D;
        gbe.cpu.SP.full = 0xFFFE;
        gbe.ram[TIMA] = 0x00;
        gbe.ram[TMA] = 0x00;
        gbe.ram[TAC] = 0x00;
        gbe.ram[0xFF10] = 0x80; // NR10
        gbe.ram[0xFF11] = 0XBF; // NR11
        gbe.ram[0xFF12] = 0xF3; // NR12
        gbe.ram[0xFF14] = 0xBF; // NR14
        gbe.ram[0xFF16] = 0x3F; // NR21
        gbe.ram[0xFF17] = 0x00; // NR22
        gbe.ram[0xFF19] = 0xBF; // NR24
        gbe.ram[0xFF1A] = 0x7F; // NR30
        gbe.ram[0xFF1B] = 0xFF; // NR31
        gbe.ram[0xFF1C] = 0x9F; // NR32
        gbe.ram[0xFF1E] = 0xBF; // NR33
        gbe.ram[0xFF20] = 0xFF; // NR41
        gbe.ram[0xFF21] = 0x00; // NR42
        gbe.ram[0xFF22] = 0x00; // NR43
        gbe.ram[0xFF23] = 0xBF; // NR30
        gbe.ram[0xFF24] = 0x77; // NR50
        gbe.ram[0xFF25] = 0xF3; // NR51
        gbe.ram[0xFF26] = 0XF1; // F1 = Game Boy NR52
        gbe.ram[0xFF40] = 0x91; // LCDC
        gbe.ram[0xFF42] = 0x00; // SCY
        gbe.ram[0xFF43] = 0x00; // SCX
        gbe.ram[0xFF45] = 0x00; // LYC
        gbe.ram[0xFF47] = 0xFC; // BGP
        gbe.ram[0xFF48] = 0xFF; // OBP0
        gbe.ram[0xFF49] = 0xFF; // OBP1
        gbe.ram[0xFF4A] = 0x00; // WY
        gbe.ram[0xFF4B] = 0x00; // WX
        gbe.ram[0xFFFF] = 0x00; // IE
    }
    logger(LOG_INFO, "Loading cartridge");
    logger(LOG_INFO, rompath);
    FILE *handle = fopen(rompath, "rb");
    if (handle == NULL) {
        logger(LOG_ERR, "Failed to get handle");
        crash();
    }
    fseek(handle, 0, SEEK_END);
    long size = ftell(handle);
    rewind(handle);
    size_t result = fread(gbe.cart.rom, 1, (unsigned) size, handle);
    if (result != (unsigned) size) {
        logger(LOG_ERR, "Failed to copy buffer to RAM");
    }
    fclose(handle);
    memcpy(gbe.cart.title, gbe.cart.rom + 0x134, 11);
    gbe.cart.title[11] = '\0';
    gbe.cart.CGBFlag = gbe.cart.rom[0x143];
    gbe.cart.SGBFlag = gbe.cart.rom[0x146];
    // copy fixed gamebank to ram
    memcpy(&gbe.ram[0x100], &gbe.cart.rom[0x100], 0x7F00);
    switch (gbe.cart.rom[0x147]) {
        case 0:
            gbe.cart.type = NO_MBC;
            break;
        case 1:
            gbe.cart.type = MBC1;
            break;
        case 2:
            gbe.cart.type = MBC1;
            break;
        case 3:
            gbe.cart.type = MBC1;
            break;
        case 5:
            gbe.cart.type = MBC2;
            break;
        case 6:
            gbe.cart.type = MBC2;
            break;
        default:
            printf("Invalid cartridge MBC\n");
            crash();
    }
    gbe.cart.romSize = gbe.cart.rom[0x148];
    gbe.cart.ramSize = gbe.cart.rom[0x149];
    gbe.cart.destinationCode = gbe.cart.rom[0x14A];
    gbe.cart.headerChecksum = gbe.cart.rom[0x14D];
    gbe.cart.rompath = rompath;
    gbe.cart.dumpSize = result;
    gbe.cart.mbc.rombank = 1;
}

void stackPush(uint16_t val) {
    /* First we decrement the stack pointer by 2 (the stack stores exclusively DWORD (uint16_t) )*/
    /* split our 16 bit value in 2 8 bit values */
    uint8_t low = val & 0xFF; // mask "upper" byte and cast to uint8_t
    uint8_t high = val >> 8; // shift lower byte to the least significant byte and cast to uint8_t
    //printf("0x%04x pushed\n",val);
    gbe.cpu.SP.full--;
    memoryWrite(gbe.cpu.SP.full, high);
    gbe.cpu.SP.full--;
    memoryWrite(gbe.cpu.SP.full, low);
}

uint16_t stackPop() {
    uint16_t spval = gbe.ram[gbe.cpu.SP.full + 1] << 8;
    spval |= gbe.ram[gbe.cpu.SP.full];
    gbe.cpu.SP.full += 2;
    //printf("0x%04x popped\n",spval);
    return spval;

}

// STUB

void fetchDecodeExecute() {
    uint8_t opcode = gbe.ram[gbe.cpu.PC];

    if (((gbe.cpu.PC >= 0x4000 && gbe.cpu.PC <= 0x7FFF) || (gbe.cpu.PC >= 0xA000 && gbe.cpu.PC <= 0xBFFF))) { // if out of bounds, let memoryRead() decide
        gbe.cpu.PC = memoryRead(gbe.cpu.PC);
    }
    if (!gbe.cpu.halted) {
        {
            if (opcode != 0xCB) { // ignore extended opcode operator
                if (gbe.log) {
                    //printf("I 0x%02x PC: 0x%04x SP: 0x%04x A: 0x%02x F: 0x%02x BC: 0x%04x DE: 0x%04x HL: 0x%04x\n", opcode, gbe.cpu.PC, gbe.cpu.SP.full, gbe.cpu.AF.hi, gbe.cpu.AF.lo, gbe.cpu.BC.full, gbe.cpu.DE.full, gbe.cpu.HL.full);
                }
            }
            gbe.cpu.cycles += 4;
        }
        // processor needs to be prepared before starting
        if (gbe.cpu.PC == 0x0000) {
            gbe.cpu.SP.full = 0xFFFE; // reset stack pointer address
            gbe.cpu.PC = 0x0003; // set program counter to 0x0003, skipping the instruction
        } else {
            gbe.cpu.PC++;
            execute(opcode);
        }
    }else {
        gbe.cpu.cycles += 4;
    }
    if (gbe.cpu.imePendingDisable) { // when interrupt disable is called it will be done after the next opcode
        if (memoryRead(gbe.cpu.PC - 1) != 0xF3) { // make sure the previous opcode wasn't the ime disable opcode
            gbe.cpu.imePendingDisable = 0;
            gbe.cpu.ime = 0;
        }
    }
    if (gbe.cpu.imePendingEnable) {
        if (memoryRead(gbe.cpu.PC - 1) != 0xFB) { // same story goes for interrupt enable
            gbe.cpu.imePendingEnable = 0;
            gbe.cpu.ime = 1;

        }
    }

}

void executionCycle() {
    gbe.cpu.cycles = 0;
    while (gbe.cpu.cycles < MAX_CYCLES) {
        fetchDecodeExecute();
        //debuggerCycle();
        updateTimers(gbe.cpu.cycles);
        updateFramebuffer(gbe.cpu.cycles);
        handleKeyboard();
        handleInterrupts();

        //usleep(1000);
    }
    drawFramebuffer();
}