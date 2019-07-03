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

/* MMU stuff goes here */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "machine.h"
#include "logger.h"
#include "interrupt.h"
#include "graphics.h"

extern gb gbe;

uint8_t memoryRead(uint16_t addr);
void memoryWrite(uint16_t addr, uint8_t val);

void enableRAMBanking(uint8_t val);
void memoryBankingHandler(uint16_t addr, uint8_t val);
void toggleBankingMode(uint8_t val);

void swapLowerROMBank(uint8_t val);
void swapUpperROMBank(uint8_t val);
void swapRAMBank(uint8_t val);


void DMATransfer(uint8_t val);

uint8_t memoryRead(uint16_t addr) {
    // read request from ROM memory bank region?
    if ((addr >= 0x4000) && (addr <= 0x7FFF)) {
        uint16_t adjustedAddr = addr; // align address with Cartridge ROM position
        adjustedAddr += ((gbe.cart.mbc.rombank - 1)* 0x4000);
        return gbe.cart.rom[adjustedAddr];
    }// read request from RAM bank?
    else if (addr >= 0xA000 && addr <= 0xBFFF) {
        uint16_t adjustedAddr = addr - 0xA000; // align address with Cartridge RAM position
        return gbe.cart.mbc.ram[adjustedAddr + (gbe.cart.mbc.rambank * 0x2000)];
    } else if (addr == 0xFF00) { // intercept keypad read.
        return getKeyState();
    }// normal read
    return gbe.ram[addr];
}

void memoryWrite(uint16_t addr, uint8_t val) {
    uint16_t aligned; // used for correcting memory write addresses
    if (addr <= 0x1FFF) { // writing in range of 0x0 to 0x1fff enables/disables rambank access
        switch (gbe.cart.mbc.type) {
            case MBC1:
                if ((val & 0xF) == 0xA) {
                    gbe.cart.mbc.eRamEnabled = 1;
                } else if (val == 0x0) {
                    gbe.cart.mbc.eRamEnabled = 0;
                }
                break;
            case MBC2:
                if (bit_test(addr, 8) == 0) {
                    if ((val & 0xF) == 0xA) {
                        gbe.cart.mbc.eRamEnabled = 1;
                    } else if (val == 0x0) {
                        gbe.cart.mbc.eRamEnabled = 0;
                    }
                }
                break;
            default:
                break;
        }
        // writing to 2000 - 3fff requires a rombank swap
    } else if ((addr >= 0x2000) && (addr <= 0x3FFF)) {
        switch (gbe.cart.mbc.type) {
            case MBC1:
                if (val == 0x00) {
                    val++;
                }
                val &= 31; // mask lower 5 bits
                // mask upper 3 bits.
                gbe.cart.mbc.rombank &= 224;
                gbe.cart.mbc.rombank |= val; // merge them
                //printf("rombank swap: %d\n", gbe.cart.mbc.rombank);
                break;
            case MBC2:
                val &= 0xF; // MBC2 only gets 4 bits of memory bank "room" because it has less memory banks than MBC1
                gbe.cart.mbc.rombank = val;
                break;
            default:
                break;
        }
    } else if ((addr >= 0x4000) && (addr <= 0x5FFF)) {
        switch (gbe.cart.mbc.type) {
            case MBC1:
                if (gbe.cpu.memoryModel == 1) {
                    gbe.cart.mbc.rambank = 0; // force ram bank 0 mode while using this memoryModel because 16/8 only has 1 ram bank
                    val &= 3;
                    val <<= 5;
                    if ((gbe.cart.mbc.rombank & 31) == 0) {
                        val++;
                    }

                    gbe.cart.mbc.rombank &= 31; // disable upper 3 bits to make sure
                    gbe.cart.mbc.rombank |= val;
                } else {
                    gbe.cart.mbc.rombank = val & 0x3;
                }
                break;
            default:
                break;
        }
        // memory mode switching address range
    } else if ((addr >= 0x6000) && (addr <= 0x7FFF)) {
        switch (gbe.cart.mbc.type) {
            case MBC1:
                val &= 1; // we only care about the lsb
                if (val == 1) { // 4 megabit ROM + 32 kilobyte RAM mode 
                    gbe.cart.mbc.rombank = 0; // force switch to rombank 0 in this mode
                    gbe.cpu.memoryModel = 1;
                } else {
                    gbe.cpu.memoryModel = 1;
                }
                break;
            default:
                break;

        }
    } else if (((addr >= 0xA000) && (addr <= 0xBFFF))) {
        if (gbe.cart.mbc.eRamEnabled) {
            switch (gbe.cart.mbc.type) {
                case MBC1:
                    aligned = (addr - 0xA000); // set proper offset
                    gbe.cart.mbc.ram[aligned + (gbe.cart.mbc.rambank * 0x2000)] = val;
                    break;
                default:
                    break;
            }
        } else if (gbe.cart.mbc.type == MBC2 && (addr < 0xA200)) {
            aligned = addr = 0xA000;
            gbe.cart.mbc.ram[aligned + (gbe.cart.mbc.rambank * 0x2000)] = val;
        }
    } else if ((addr >= 0xC000) && (addr <= 0xDFFF)) { // writes to this range are mirrored 0x2000 above and vice versa

        gbe.ram[addr] = val; // write
        gbe.ram[addr + 0x2000] = val; // mirror

    } else if ((addr >= 0xE000) && (addr <= 0xFDFF)) {

        gbe.ram[addr] = val;
        gbe.ram[addr - 0x2000] = val;

    } else if ((addr >= 0xFEA0) && (addr <= 0xFEFF)) {
        // writing here is forbidden.

    } else if (addr == DIV) { // writing to the divider register resets it to 0
        gbe.ram[DIV] = 0;
        gbe.cpu.div = 0;
    } else if (addr == TAC) { // This register modifies CPU speed
        gbe.ram[addr] = val;
        int timer = val & 0x3;
        int clk = 0;
        switch (timer) {
            case 0:
                clk = 1024;
                break;
            case 1:
                clk = 16;
                break;
            case 2:
                clk = 64;
                break;
            case 3:
                clk = 256;
                break;
            default:
                break;
        }
        if (clk != gbe.cpu.clock_freq) { // clockspeed changed so we need to change timer resolution
            gbe.cpu.tc = 0;
            gbe.cpu.clock_freq = clk;
        }
    } else if (addr == CURRENT_SCANLINE) { // writing to the current scanline resets it
        gbe.ram[addr] = 0;
    } else if (addr == 0xFF45) { // LY compare
        gbe.ram[addr] = val;
    } else if (addr == 0xFF56) { // DMA (used for mass copying sprites and stuff)
        uint16_t corrected = (val << 8); // the address to load from is stored in an 8bit value and needs to be copied into the upper nibble
        for (int i = 0; i < 0xA0; i++) {
            gbe.ram[0xFE00 + i] = memoryRead(corrected + i);
        }
    }else if(addr == 0xFF50) {
        // writing here means that the game boy finished execution of the boot rom, now we need to copy the first 0x100 bytes to ram[0x0]
        // because interrupts are handled here
        memcpy(&gbe.ram, &gbe.cart.rom, 0x100);
        gbe.log = 1;
        //crash();
    }else if((addr >= 0xFF4C) && (addr <= 0xFF7F)) { // writing here is forbidden too
    }else { // looks like we are good to go
        gbe.ram[addr] = val;
    }
}

void enableRAMBanking(uint8_t val) {
    uint8_t tData;
    switch (gbe.cart.mbc.type) {
        case MBC2:
            if (((val >> 4) & 1) == 1) { // bit 4 must be 0 with MBC2
                break;
            }
        case MBC1:
            tData = val & 0xF;
            if (tData == 0xA) { // if the lower nibble equals 0xA ram banking is enabled
                gbe.cart.mbc.eRamEnabled = 1;
            } else if (tData == 0x0) {
                gbe.cart.mbc.eRamEnabled = 0;
            }
            break;
        default:
            printf("Memory access violation.\n");
            crash();
            break;

    }
}

void memoryBankingHandler(uint16_t addr, uint8_t val) {
    /* enable cartridge ram. */
    if (addr < 0x2000) {
        switch (gbe.cart.mbc.type) {
            case NO_MBC:
            case MBC1:
            case MBC2:
                enableRAMBanking(val);
                break;
            default: // NO_MBC
                printf("[MMU] Unknown memory banking request.\n");
                crash();
                break;
        }

    } else if ((addr >= 0x2000) && (addr < 0x4000)) {
        swapLowerROMBank(val);
    } else if ((addr >= 0x4000) && (addr < 0x6000)) {
        // mbc2 only has rambank 0, mbc1 can switch.
        switch (gbe.cart.mbc.type) {
            case MBC1:
                if (gbe.cart.mbc.rombankingEnabled) {
                    swapUpperROMBank(val);
                } else {
                    swapRAMBank(val);
                }
                break;
            default:
                break;
        }
    } else if ((addr >= 0x6000) && (addr < 0x8000)) {
        switch (gbe.cart.mbc.type) {
            case MBC1: // RAM banking is disabled in MBC2 because it only has 1 ram bank;
                toggleBankingMode(val);
                break;
            default:
                return;
                printf("[MMU] Unknown memory banking request. (RAM banking toggle request)\n");
                crash();
                break;
        }
    }
}

void swapLowerROMBank(uint8_t val) {
    switch (gbe.cart.mbc.type) {
        case MBC2:
            // BC2 only uses the 4 lower bits 0,3, anything above is ignored.
            if ((gbe.cart.mbc.rombank = (uint8_t) (val & 0xF)) == 0) { // if something breaks, its probably this.
                gbe.cart.mbc.rombank++;
            }
            break;
        case MBC1:
            gbe.cart.mbc.rombank &= 0xE0; // remove lower 5 bits by masking it with 0xE0, 224, 1110 0000
            gbe.cart.mbc.rombank |= val & 0x1F; // inject lower 5 bits from val (val has upper 3 bits masked))
            if (gbe.cart.mbc.rombank == 0) {
                gbe.cart.mbc.rombank++;
            }
            break;
        default:
            break;

    }
}

void swapUpperROMBank(uint8_t val) {
    switch (gbe.cart.mbc.type) {
        case MBC1: // MBC2 doesn't have an upper rom bank.
            gbe.cart.mbc.rombank &= 0x1F; // disable upper 3 bits
            val &= 0xE0; // disable lower 5
            gbe.cart.mbc.rombank |= val; // merge upper 3 bits of type with lower 5 of val
            if (gbe.cart.mbc.rombank == 0) {
                gbe.cart.mbc.rombank++;
            }
            break;
        default:
            break;
    }
}

void swapRAMBank(uint8_t val) { // illegal on MBC2
    switch (gbe.cart.mbc.type) {
        case MBC1:
            gbe.cart.mbc.rambank = val & 0x3; // current rambank is lower 2 bits of val
            break;
        default:
            printf("Illegal rambank swap request\n");
            crash();
            break;
    }
}

void toggleBankingMode(uint8_t val) {
    // if lowest bit is 0, rombanking is set to 1.
    gbe.cart.mbc.rombankingEnabled = ((val & 0x1) == 0) ? 1 : 0;
    if (gbe.cart.mbc.rombank) { // if rombanking got enabled, set rombank to 0 because the gameboy can only access rombank 0 in this mode
        gbe.cart.mbc.rombank = 0;
    }
}

void DMATransfer(uint8_t val) {
    uint16_t addr = (uint16_t) (val << 8); // addr = data * 100
    for (int i = 0; i < 0xA0 /* 160 */; i++) {
        memoryWrite((uint16_t) (0xFE00 + i), memoryRead((uint16_t) (addr + i)));
    }
}