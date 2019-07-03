/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "machine.h"
#include "memory.h"
#include "interrupt.h"
#include "timers.h"
void updateTimers(int cycles);
void setClockspeed(void);
uint8_t getClockspeed(void);
int clockEnabled(void);

void updateTimers(int cycles) {
    uint8_t tac = memoryRead(TAC);
    gbe.cpu.div += cycles;
    if (bit_test(tac, 2)) { // bit 2, timer enable
        gbe.cpu.tc += cycles; // remove cycles from counter, every time its under 0 a timer tick happens
        if (gbe.cpu.tc >= gbe.cpu.clock_freq) {
            gbe.cpu.tc = 0;
            uint8_t overflow = 0;
            if (gbe.ram[TIMA] == 0xFF) {
                overflow = 1;
            }
            gbe.ram[TIMA]++; // increment don't care about overflow
            // on overflow set TIMA to TMA and issue interrupt request
            if (overflow) {
                memoryWrite(TIMA, memoryRead(TMA)); // on overflow of TIMA TIMA becomes TMA
                requestInterrupt(2); // request second bit from interrupt register
            }
        }
    }
    if (gbe.cpu.div >= 256) {
        gbe.cpu.div = 0;
        gbe.ram[DIV]++;
    }
}

void setClockspeed() {
    uint8_t value = getClockspeed();
    int freq = 0;
    switch (value) { // tcounter is the amount of cycles before a timer tick occurs.
        case 0:
            freq = 1024; // 4096 hz
            break;
        case 1:
            freq = 16; // 262144 hz
            break;
        case 2:
            freq = 64; // 65536 hz
            break;
        case 3:
            freq = 256; // 16384 hz 
            break;
        default:
            break;
    }

    if (value != gbe.cpu.clock_freq) {// if value changes update timer " ratio" variable
        gbe.cpu.tc = 0;
        gbe.cpu.clock_freq = freq;
    }
}

uint8_t getClockspeed() {

    return memoryRead(TAC) & 0x3; // mask last 2 bits
}

int clockEnabled() {
    return ((memoryRead(TAC) >> 2) & 1); // return 2nd bit 1 = enabled 0 = disabled
}