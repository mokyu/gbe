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
 * File:   machine.h
 * Author: emiel
 *
 * Created on January 26, 2017, 12:20 PM
 */

#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>
#include <stdio.h>
#include "bitops.h"
#include "crashhandler.h"
// function declarations

extern void initializeEmulator(const char* romPath, int bootstrapEnabled, const char* bootstrapPath);
extern void executionCycle(void);
extern void setClockspeed(void);
extern uint8_t getClockspeed(void);
extern int clockEnabled(void);
extern void stackPush(uint16_t val);
extern uint16_t stackPop(void);

/* Memory bank controller types */
#define NO_MBC  0
#define MBC1    1
#define MBC2    2
#define MBC3    3

/* Important memory regions*/
#define DIV     0xFF04  // divider register, increments at 16384Hz, writing to this register resets it to 0x00, frequency doubles in CGB double speed mode
#define TIMA    0xFF05  // Timer counter, gets incremented by TAC, after overflow interrupt will be request and is value is set to TMA
#define TMA     0xFF06  // Timer Modulo, TIMA will be set to the value on this address after overflowing
#define TAC     0xFF07  /*
                Bit  2   - Timer Enable
                Bits 1-0 - Input Clock Select
               00: CPU Clock / 1024 (DMG, CGB:   4096 Hz, SGB:   ~4194 Hz)
               01: CPU Clock / 16   (DMG, CGB: 262144 Hz, SGB: ~268400 Hz)
               10: CPU Clock / 64   (DMG, CGB:  65536 Hz, SGB:  ~67110 Hz)
               11: CPU Clock / 256  (DMG, CGB:  16384 Hz, SGB:  ~16780 Hz)

    The "Timer Enable" bit only affects the timer, the divider is ALWAYS counting.*/
#define IF      0xFF0F  /*  Interrupt flag
                            Bit 0: V-Blank  Interrupt Request (INT 40h)  (1=Request)
                            Bit 1: LCD STAT Interrupt Request (INT 48h)  (1=Request)
                            Bit 2: Timer    Interrupt Request (INT 50h)  (1=Request)
                            Bit 3: Serial   Interrupt Request (INT 58h)  (1=Request)
                            Bit 4: Joypad   Interrupt Request (INT 60h)  (1=Request)*/

#define IE      0xFFFF  /*  Interrupt enable flag, same as above but its used for enabling disabling(ignoring) an interrupt*/

typedef struct _f { // CPU Flags
    uint8_t Z; // Set when math result is 0 (or 2 values match with CP operations)
    uint8_t N; // Set after substract operation 
    uint8_t H; // half carry, set when the lower nibble overflows e.g.: 0000 1111 (15) > 0001 0000 (16)
    uint8_t C; // Carry flag, also set under some conditions with CP operations
} _f;

typedef union _r { // Registers
    uint16_t full; // full register, HL BC etc.

    struct {
        uint8_t lo; // least significant 8 bit subregister  A(F)
        uint8_t hi; // most significant 8 bit subregister   (A)F
    };
} _r;

typedef struct _cpu {
    _r AF; // 16 bit register F is the flag register
    _r BC;
    _r DE;
    _r HL;
    _r SP; // stack pointer
    uint16_t PC; // program counter
    uint8_t M; // M timer
    uint8_t T; // T timer
    int tc; // amount of cycles before a timer tick occurs, is used to keep timer accurate when clockspeed has changed
    int div; // divider register
    int ime; // interrupt master enable
    int imePendingEnable; // the way we do interrupts require us to capture IME requests or else they will be executed too late.
    int imePendingDisable;
    int memoryModel; // This only applies to MBC1, by writing to a certain memory range we can switch between 16/8 mode (memoryModel = 1) (16 Mbit rom and 8kbbyte ram) to 4/32 mode (memoryModel = 0) (4 Mbit rom and 32kbyte ram))
    int halted; // if 1 no opcodes are executed until interrupt happens.
    int clock_freq; // cpu frequency
    int cycles; // amount of cycles for an opcode
} _cpu;
typedef struct _mbc {
    uint8_t ram[0x640000]; // extended cartridge ram
    uint8_t rambank; // the current rambank
    uint8_t rombank; // the current rombank
    int eRamEnabled; // set if cartridge ram is enabled.
    int rombankingEnabled; // set to 1 when banking is enabled
    uint8_t type; // MBC type
} _mbc;

typedef struct _cart {
    uint8_t rom[0x800000]; // the only 8mb catridge that exists is "Densha de Go! 2"
    _mbc mbc;
    uint8_t type;
    uint8_t title[12];
    uint8_t romSize;
    uint8_t ramSize;
    uint8_t destinationCode;
    uint8_t headerChecksum;
    uint8_t CGBFlag; // gameboy color only
    uint8_t SGBFlag; // super game boy functions
    const char* rompath;
    size_t dumpSize;
} _cart;

typedef struct _gpu {
    uint8_t FrameBuffer[160][144][3];
    int scanlineCounter; // used for retracing LY
} _gpu;

typedef struct _gb {
    _gpu gpu;
    _cart cart;
    uint8_t buttonState;
    uint8_t ram[0x10000];
    int log; // only enable logging after DMG bootstrap 
    _cpu cpu;
} gb;

extern gb gbe;

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* MACHINE_H */

