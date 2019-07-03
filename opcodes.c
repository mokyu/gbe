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

#include <stdint.h>
#include <stdlib.h>
#include "lr35902.h"
#include "machine.h"
#include "memory.h"
int execute(uint8_t opcode);
extern void Execute8bitExtension(void); // extended 8bit opcodes
extern void setFlag(int flag);
extern void toggleFlag(int flag);
extern uint8_t testFlag(int flag);
void debug(uint8_t opcode, char buf[]);

int execute(uint8_t opcode) {
    uint16_t uval16t; // 16 bit value used for generic stuff (can't declare inside switch))
    uint8_t uval8t;
    int8_t sval8t;
    // jumptable
    char buf[30];
    buf[0] = '\0';
    int cycles = 0;
    switch (opcode) {
            // NOP
        case 0x00: // literally nothing
            sprintf(buf, "NOP");
            debug(opcode, buf);
            cycles = 4;
            break;

            // 8 bit load instructions
        case 0x06:
            sprintf(buf, "LD B, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode, buf);
            cpu_8bit_load(&gbe.cpu.BC.hi);
            break;
        case 0x0E:
            sprintf(buf, "LD C, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode,buf);
            cpu_8bit_load(&gbe.cpu.BC.lo);
            break;
        case 0x16:
            sprintf(buf, "LD D, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode,buf);
            cpu_8bit_load(&gbe.cpu.DE.hi);
            break;
        case 0x1E:
            sprintf(buf, "LD E, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode,buf);
            cpu_8bit_load(&gbe.cpu.DE.lo);
            break;
        case 0x26:
            sprintf(buf, "LD H, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode,buf);
            cpu_8bit_load(&gbe.cpu.HL.hi);
            break;
        case 0x2E:
            sprintf(buf, "LD C, %02x", memoryRead(gbe.cpu.PC + 1));
            debug(opcode,buf);
            cpu_8bit_load(&gbe.cpu.HL.lo);
            break;

            // 8bit register to register loads
        case 0x7F:
            sprintf(buf, "LD A, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4);
            break;
        case 0x78:
            sprintf(buf, "LD A, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4);
            break;
        case 0x79:
            sprintf(buf, "LD A, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4);
            break;
        case 0x7A:
            sprintf(buf, "LD A, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4);
            break;
        case 0x7B:
            sprintf(buf, "LD A, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4);
            break;
        case 0x7C:
            sprintf(buf, "LD A, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4);
            break;
        case 0x7D:
            sprintf(buf, "LD A, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4);
            break;
        case 0x40:
            sprintf(buf, "LD B, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.BC.hi, 4);
            break;
        case 0x41:
            sprintf(buf, "LD B, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.BC.lo, 4);
            break;
        case 0x42:
            sprintf(buf, "LD B, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.DE.hi, 4);
            break;
        case 0x43:
            sprintf(buf, "LD B, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.DE.lo, 4);
            break;
        case 0x44:
            sprintf(buf, "LD B, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.HL.hi, 4);
            break;
        case 0x45:
            sprintf(buf, "LD B, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.HL.lo, 4);
            break;
        case 0x48:
            sprintf(buf, "LD C, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.BC.hi, 4);
            break;
        case 0x49:
            sprintf(buf, "LD C, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.BC.lo, 4);
            break;
        case 0x4A:
            sprintf(buf, "LD C, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.DE.hi, 4);
            break;
        case 0x4B:
            sprintf(buf, "LD C, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.DE.lo, 4);
            break;
        case 0x4C:
            sprintf(buf, "LD C, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.HL.hi, 4);
            break;
        case 0x4D:
            sprintf(buf, "LD C, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.HL.lo, 4);
            break;
        case 0x50:
            sprintf(buf, "LD D, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.BC.hi, 4);
            break;
        case 0x51:
            sprintf(buf, "LD D, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.BC.lo, 4);
            break;
        case 0x52:
            sprintf(buf, "LD D, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.DE.hi, 4);
            break;
        case 0x53:
            sprintf(buf, "LD D, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.DE.lo, 4);
            break;
        case 0x54:
            sprintf(buf, "LD D, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.HL.hi, 4);
            break;
        case 0x55:
            sprintf(buf, "LD D, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.HL.lo, 4);
            break;
        case 0x58:
            sprintf(buf, "LD E, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.BC.hi, 4);
            break;
        case 0x59:
            sprintf(buf, "LD E, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.BC.lo, 4);
            break;
        case 0x5A:
            sprintf(buf, "LD E, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.DE.hi, 4);
            break;
        case 0x5B:
            sprintf(buf, "LD E, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.DE.lo, 4);
            break;
        case 0x5C:
            sprintf(buf, "LD E, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.HL.hi, 4);
            break;
        case 0x5D:
            sprintf(buf, "LD E, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.HL.lo, 4);
            break;
        case 0x60:
            sprintf(buf, "LD H, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.BC.hi, 4);
            break;
        case 0x61:
            sprintf(buf, "LD H, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.BC.lo, 4);
            break;
        case 0x62:
            sprintf(buf, "LD H, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.DE.hi, 4);
            break;
        case 0x63:
            sprintf(buf, "LD H, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.DE.lo, 4);
            break;
        case 0x64:
            sprintf(buf, "LD H, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.HL.hi, 4);
            break;
        case 0x65:
            sprintf(buf, "LD H, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.HL.lo, 4);
            break;
        case 0x68:
            sprintf(buf, "LD L, B");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.BC.hi, 4);
            break;
        case 0x69:
            sprintf(buf, "LD L, C");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.BC.lo, 4);
            break;
        case 0x6A:
            sprintf(buf, "LD L, D");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.DE.hi, 4);
            break;
        case 0x6B:
            sprintf(buf, "LD L, E");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.DE.lo, 4);
            break;
        case 0x6C:
            sprintf(buf, "LD L, H");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.HL.hi, 4);
            break;
        case 0x6D:
            sprintf(buf, "LD L, L");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.HL.lo, 4);
            break;

            // register to RAM writes
        case 0x70:
            sprintf(buf, "LD (HL), B");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.BC.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0x71:
            sprintf(buf, "LD (HL), C");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.BC.lo);
            gbe.cpu.cycles += 8;
            break;
        case 0x72:
            sprintf(buf, "LD (HL), D");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.DE.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0x73:
            sprintf(buf, "LD (HL), E");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.DE.lo);
            gbe.cpu.cycles += 8;
            break;
        case 0x74:
            sprintf(buf, "LD (HL), H");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.HL.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0x75:
            sprintf(buf, "LD (HL), L");
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.HL.lo);
            gbe.cpu.cycles += 8;
            break;

            // ram to register writes;
            // write memory to reg
        case 0x7E:
            sprintf(buf, "LD A, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, gbe.cpu.HL.full);
            break;
        case 0x46:
            sprintf(buf, "LD B, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.BC.hi, gbe.cpu.HL.full);
            break;
        case 0x4E:
            sprintf(buf, "LD C, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.BC.lo, gbe.cpu.HL.full);
            break;
        case 0x56:
            sprintf(buf, "LD D, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.DE.hi, gbe.cpu.HL.full);
            break;
        case 0x5E:
            sprintf(buf, "LD E, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.DE.lo, gbe.cpu.HL.full);
            break;
        case 0x66:
            sprintf(buf, "LD H, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.HL.hi, gbe.cpu.HL.full);
            break;
        case 0x6E:
            sprintf(buf, "LD L, (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.HL.lo, gbe.cpu.HL.full);
            break;
        case 0x0A:
            sprintf(buf, "LD A, (BC) 0x%04x", gbe.cpu.BC.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, gbe.cpu.BC.full);
            break;
        case 0x1A:
            sprintf(buf, "LD B, (BC) 0x%04x", gbe.cpu.BC.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, gbe.cpu.DE.full);
            break;
        case 0xF2:
            sprintf(buf, "LD A, (C) 0x%04x", (0xFF00 + gbe.cpu.BC.lo));
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, (0xFF00 + gbe.cpu.BC.lo));
            break;

            // load value of accumulator (A) into register
        case 0x47:
            sprintf(buf, "LD B, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.hi, gbe.cpu.AF.hi, 4);
            break;
        case 0x4F:
            sprintf(buf, "LD C, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.BC.lo, gbe.cpu.AF.hi, 4);
            break;
        case 0x57:
            sprintf(buf, "LD D, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.hi, gbe.cpu.AF.hi, 4);
            break;
        case 0x5F:
            sprintf(buf, "LD E, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.DE.lo, gbe.cpu.AF.hi, 4);
            break;
        case 0x67:
            sprintf(buf, "LD H, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.hi, gbe.cpu.AF.hi, 4);
            break;
        case 0x6F:
            sprintf(buf, "LD L, A");
            debug(opcode,buf);
            cpu_reg_load(&gbe.cpu.HL.lo, gbe.cpu.AF.hi, 4);
            break;

            // store accumulator (A) in RAM at address;
        case 0x02:
            sprintf(buf, "LD (BC) 0x%04x, A", gbe.cpu.BC.full);
            debug(opcode,buf);
            memoryWrite(gbe.cpu.BC.full, gbe.cpu.AF.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0x12:
            sprintf(buf, "LD (DE) 0x%04x, A", gbe.cpu.DE.full);
            debug(opcode,buf);
            memoryWrite(gbe.cpu.DE.full, gbe.cpu.AF.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0x77:
            sprintf(buf, "LD (HL) 0x%04x, A", gbe.cpu.HL.full);
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.AF.hi);
            gbe.cpu.cycles += 8;
            break;
        case 0xE2:
            sprintf(buf, "LD (C) 0x%04x, A", (0xFF00 + gbe.cpu.BC.lo));
            debug(opcode,buf);
            memoryWrite((0xFF00 + gbe.cpu.BC.lo), gbe.cpu.AF.hi);
            gbe.cpu.cycles += 8;
            break;

            // load memory into A and increment/decrement it
        case 0x3A:
            sprintf(buf, "LD A , (HL-) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, gbe.cpu.HL.full);
            cpu_16bit_dec(&gbe.cpu.HL.full, 0);
            break;
        case 0x2A:
            sprintf(buf, "LD A , (HL+) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            cpu_reg_load_direct(&gbe.cpu.AF.hi, gbe.cpu.HL.full);
            cpu_16bit_inc(&gbe.cpu.HL.full, 0);
            break;

            // put accumulator into ram
        case 0x32:
            sprintf(buf, "LD (HL-) 0x%04x, A", gbe.cpu.HL.full);
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.AF.hi);
            cpu_16bit_dec(&gbe.cpu.HL.full, 0);
            gbe.cpu.cycles += 8;
            break;
        case 0x22:
            sprintf(buf, "LD (HL+) 0x%04x, A", gbe.cpu.HL.full);
            debug(opcode,buf);
            memoryWrite(gbe.cpu.HL.full, gbe.cpu.AF.hi);
            cpu_16bit_inc(&gbe.cpu.HL.full, 0);
            gbe.cpu.cycles += 8;
            break;

            // 16 bit register loads
        case 0x01:
            sprintf(buf, "LD BC, %d", read_16bit());
            debug(opcode,buf);
            cpu_16bit_load(&gbe.cpu.BC.full);
            break;
        case 0x11:
            sprintf(buf, "LD DE, %d", read_16bit());
            debug(opcode,buf);
            cpu_16bit_load(&gbe.cpu.DE.full);
            break;
        case 0x21:
            sprintf(buf, "LD HL, %d", read_16bit());
            debug(opcode,buf);
            cpu_16bit_load(&gbe.cpu.HL.full);
            break;
        case 0x31: // load value into stack pointer
            sprintf(buf, "LD SP, %d", read_16bit());
            debug(opcode,buf);
            cpu_16bit_load(&gbe.cpu.SP.full);
            break;
        case 0xF9:
            sprintf(buf, "LD SP, HL");
            debug(opcode,buf);
            gbe.cpu.SP.full = gbe.cpu.HL.full;
            gbe.cpu.cycles += 8;
            break;

            // push registers to stack
        case 0xF5:
            sprintf(buf, "PUSH AF");
            debug(opcode,buf);
            stackPush(gbe.cpu.AF.full);
            gbe.cpu.cycles += 16;
            break;
        case 0xC5:
            sprintf(buf, "PUSH BC");
            debug(opcode,buf);
            stackPush(gbe.cpu.BC.full);
            gbe.cpu.cycles += 16;
            break;
        case 0xD5:
            sprintf(buf, "PUSH DE");
            debug(opcode,buf);
            stackPush(gbe.cpu.DE.full);
            gbe.cpu.cycles += 16;
            break;
        case 0xE5:
            sprintf(buf, "PUSH HL");
            debug(opcode,buf);
            stackPush(gbe.cpu.HL.full);
            gbe.cpu.cycles += 16;
            break;

            // pop stack into register
        case 0xF1:
            sprintf(buf, "POP AF");
            debug(opcode,buf);
            gbe.cpu.AF.full = stackPop();
            gbe.cpu.cycles += 12;
            break;
        case 0xC1:
            sprintf(buf, "POP BC");
            debug(opcode,buf);
            gbe.cpu.BC.full = stackPop();
            gbe.cpu.cycles += 12;
            break;
        case 0xD1:
            sprintf(buf, "POP DE");
            debug(opcode,buf);
            gbe.cpu.DE.full = stackPop();
            gbe.cpu.cycles += 12;
            break;
        case 0xE1:
            sprintf(buf, "POP HL");
            debug(opcode,buf);
            gbe.cpu.HL.full = stackPop();
            gbe.cpu.cycles += 12;
            break;
            // 8bit add
        case 0x87:
            sprintf(buf, "ADD A,A");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0, 0);
            break;
        case 0x80:
            sprintf(buf, "ADD A,B");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0, 0);
            break;
        case 0x81:
            sprintf(buf, "ADD A,C");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0, 0);
            break;
        case 0x82:
            sprintf(buf, "ADD A,D");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0, 0);
            break;
        case 0x83:
            sprintf(buf, "ADD A,E");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0, 0);
            break;
        case 0x84:
            sprintf(buf, "ADD A,H");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0, 0);
            break;
        case 0x85:
            sprintf(buf, "ADD A,L");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0, 0);
            break;
        case 0x86:
            sprintf(buf, "ADD A,(HL) 0x%02x%02x ", memoryRead(gbe.cpu.PC + 1), memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0, 0);
            break;
        case 0xC6:
            sprintf(buf, "ADD A, %d ", read_16bit());
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, 0, 8, 1, 0);
            break;
            // 8bit add with carry
        case 0x8F:
            sprintf(buf, "ADC A,A");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0, 1);
            break;
        case 0x88:
            sprintf(buf, "ADC A,B");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0, 1);
            break;
        case 0x89:
            sprintf(buf, "ADC A,C");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0, 1);
            break;
        case 0x8A:
            sprintf(buf, "ADC A,D");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0, 1);
            break;
        case 0x8B:
            sprintf(buf, "ADC A,E");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0, 1);
            break;
        case 0x8C:
            sprintf(buf, "ADC A,H");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0, 1);
            break;
        case 0x8D:
            sprintf(buf, "ADC A,L");
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0, 1);
            break;
        case 0x8E:
            sprintf(buf, "ADC A,(HL) %d", read_16bit());
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0, 1);
            //cpu_8bit_adc(memoryRead(gbe.cpu.))
            cpu_8bit_adc(memoryRead(gbe.cpu.HL.full));
            break;
        case 0xCE:
            sprintf(buf, "ADC A,%d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_add(&gbe.cpu.AF.hi, 0, 8, 1, 1);
            break;
            // 8bit substract
        case 0x97:
            sprintf(buf, "SUB A, A");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0, 0);
            break;
        case 0x90:
            sprintf(buf, "SUB A, B");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0, 0);
            break;
        case 0x91:
            sprintf(buf, "SUB A, C");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0, 0);
            break;
        case 0x92:
            sprintf(buf, "SUB A, D");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0, 0);
            break;
        case 0x93:
            sprintf(buf, "SUB A, E");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0, 0);
            break;
        case 0x94:
            sprintf(buf, "SUB A, H");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0, 0);
            break;
        case 0x95:
            sprintf(buf, "SUB A, L");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0, 0);
            break;
        case 0x96:
            sprintf(buf, "SUB A, (HL) %d", read_16bit());
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0, 0);
            break;
        case 0xD6:
            sprintf(buf, "SUB A, %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, 0, 8, 1, 0);
            break;
            // 8bit subtract with carry
        case 0x9F:
            sprintf(buf, "SBC A, A");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0, 1);
            break;
        case 0x98:
            sprintf(buf, "SBC A, B");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0, 1);
            break;
        case 0x99:
            sprintf(buf, "SBC A, C");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0, 1);
            break;
        case 0x9A:
            sprintf(buf, "SBC A, D");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0, 1);
            break;
        case 0x9B:
            sprintf(buf, "SBC A, E");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0, 1);
            break;
        case 0x9C:
            sprintf(buf, "SBC A, H");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0, 1);
            break;
        case 0x9D:
            sprintf(buf, "SBC A, L");
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0, 1);
            break;
        case 0x9E:
            sprintf(buf, "SBC A, (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0, 1);
            break;
        case 0xDE:
            sprintf(buf, "SBC A, %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_sub(&gbe.cpu.AF.hi, 0, 8, 1, 1);
            break;
            // 8-bit AND operation on registers
        case 0xA7:
            sprintf(buf, "AND A");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0);
            break;
        case 0xA0:
            sprintf(buf, "AND B");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0);
            break;
        case 0xA1:
            sprintf(buf, "AND C");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0);
            break;
        case 0xA2:
            sprintf(buf, "AND D");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0);
            break;
        case 0xA3:
            sprintf(buf, "AND E");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0);
            break;
        case 0xA4:
            sprintf(buf, "AND H");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0);
            break;
        case 0xA5:
            sprintf(buf, "AND L");
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0);
            break;
        case 0xA6:
            sprintf(buf, "AND (HL), %04x ", read_16bit());
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0);
            break;
        case 0xE6:
            sprintf(buf, "AND %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_and(&gbe.cpu.AF.hi, 0, 8, 1);
            break;
            // 8bit OR operation on registers
        case 0xB7:
            sprintf(buf, "OR A");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0);
            break;
        case 0xB0:
            sprintf(buf, "OR B");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0);
            break;
        case 0xB1:
            sprintf(buf, "OR C");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0);
            break;
        case 0xB2:
            sprintf(buf, "OR D");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0);
            break;
        case 0xB3:
            sprintf(buf, "OR E");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0);
            break;
        case 0xB4:
            sprintf(buf, "OR H");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0);
            break;
        case 0xB5:
            sprintf(buf, "OR L");
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0);
            break;
        case 0xB6:
            sprintf(buf, "OR (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0);
            break;
        case 0xF6:
            sprintf(buf, "OR %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_or(&gbe.cpu.AF.hi, 0, 8, 1);
            break;

            // 8bit XOR on registers
        case 0xAF:
            sprintf(buf, "XOR A");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0);
            break;
        case 0xA8:
            sprintf(buf, "XOR B");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0);
            break;
        case 0xA9:
            sprintf(buf, "XOR C");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0);
            break;
        case 0xAA:
            sprintf(buf, "XOR D");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0);
            break;
        case 0xAB:
            sprintf(buf, "XOR E");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0);
            break;
        case 0xAC:
            sprintf(buf, "XOR H");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0);
            break;
        case 0xAD:
            sprintf(buf, "XOR L");
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0);
            break;
        case 0xAE:
            sprintf(buf, "XOR (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0);
            break;
        case 0xEE:
            sprintf(buf, "XOR %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_xor(&gbe.cpu.AF.hi, 0, 8, 1);
            break;

            // 8bit compare on registers
        case 0xBF:
            sprintf(buf, "CP A");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.AF.hi, 4, 0);
            break;
        case 0xB8:
            sprintf(buf, "CP B");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.BC.hi, 4, 0);
            break;
        case 0xB9:
            sprintf(buf, "CP C");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.BC.lo, 4, 0);
            break;
        case 0xBA:
            sprintf(buf, "CP D");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.DE.hi, 4, 0);
            break;
        case 0xBB:
            sprintf(buf, "CP E");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.DE.lo, 4, 0);
            break;
        case 0xBC:
            sprintf(buf, "CP H");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.HL.hi, 4, 0);
            break;
        case 0xBD:
            sprintf(buf, "CP L");
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, gbe.cpu.HL.lo, 4, 0);
            break;
        case 0xBE:
            sprintf(buf, "CP (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, memoryRead(gbe.cpu.HL.full), 8, 0);
            break;
        case 0xFE:
            sprintf(buf, "CP %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_8bit_cmp(gbe.cpu.AF.hi, 0, 8, 1);
            break;

            // 8bit increment
        case 0x3C:
            sprintf(buf, "INC A");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.AF.hi, 4);
            break;
        case 0x04:
            sprintf(buf, "INC B");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.BC.hi, 4);
            break;
        case 0x0C:
            sprintf(buf, "INC C");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.BC.lo, 4);
            break;
        case 0x14:
            sprintf(buf, "INC D");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.DE.hi, 4);
            break;
        case 0x1C:
            sprintf(buf, "INC E");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.DE.lo, 4);
            break;
        case 0x24:
            sprintf(buf, "INC H");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.HL.hi, 4);
            break;
        case 0x2C:
            sprintf(buf, "INC L");
            debug(opcode,buf);
            cpu_8bit_inc(&gbe.cpu.HL.lo, 4);
            break;
        case 0x34:
            sprintf(buf, "INC (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            mem_8bit_inc(gbe.cpu.HL.full, 12);
            break;

            // 8bit decrement
        case 0x3D:
            sprintf(buf, "DEC A");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.AF.hi, 4);
            break;
        case 0x05:
            sprintf(buf, "DEC B");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.BC.hi, 4);
            break;
        case 0x0D:
            sprintf(buf, "DEC C");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.BC.lo, 4);
            break;
        case 0x15:
            sprintf(buf, "DEC D");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.DE.hi, 4);
            break;
        case 0x1D:
            sprintf(buf, "DEC E");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.DE.lo, 4);
            break;
        case 0x25:
            sprintf(buf, "DEC H");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.HL.hi, 4);
            break;
        case 0x2D:
            sprintf(buf, "DEC L");
            debug(opcode,buf);
            cpu_8bit_dec(&gbe.cpu.HL.lo, 4);
            break;
        case 0x35:
            sprintf(buf, "DEC (HL) %d", memoryRead(gbe.cpu.HL.full));
            debug(opcode,buf);
            mem_8bit_dec(gbe.cpu.HL.full, 12);
            break;

            // 16bit add
        case 0x09:
            sprintf(buf, "ADD HL, BC");
            debug(opcode,buf);
            cpu_16bit_add(&gbe.cpu.HL.full, gbe.cpu.BC.full, 8);
            break;
        case 0x19:
            sprintf(buf, "ADD HL, DE");
            debug(opcode,buf);
            cpu_16bit_add(&gbe.cpu.HL.full, gbe.cpu.DE.full, 8);
            break;
        case 0x29:
            sprintf(buf, "ADD HL, HL");
            debug(opcode,buf);
            cpu_16bit_add(&gbe.cpu.HL.full, gbe.cpu.HL.full, 8);
            break;
        case 0x39:
            sprintf(buf, "ADD HL, SP");
            debug(opcode,buf);
            cpu_16bit_add(&gbe.cpu.HL.full, gbe.cpu.SP.full, 8);
            break;

            // increment 16bit register
        case 0x03:
            sprintf(buf, "INC BC");
            debug(opcode,buf);
            cpu_16bit_inc(&gbe.cpu.BC.full, 8);
            break;
        case 0x13:
            sprintf(buf, "INC DE");
            debug(opcode,buf);
            cpu_16bit_inc(&gbe.cpu.DE.full, 8);
            break;
        case 0x23:
            sprintf(buf, "INC HL");
            debug(opcode,buf);
            cpu_16bit_inc(&gbe.cpu.HL.full, 8);
            break;
        case 0x33:
            sprintf(buf, "INC SP");
            debug(opcode,buf);
            cpu_16bit_inc(&gbe.cpu.SP.full, 8);
            break;

            // decrement 16bit register
        case 0x0B:
            sprintf(buf, "DEC BC");
            debug(opcode,buf);
            cpu_16bit_dec(&gbe.cpu.BC.full, 8);
            break;
        case 0x1B:
            sprintf(buf, "DEC DE");
            debug(opcode,buf);
            cpu_16bit_dec(&gbe.cpu.DE.full, 8);
            break;
        case 0x2B:
            sprintf(buf, "DEC HL");
            debug(opcode,buf);
            cpu_16bit_dec(&gbe.cpu.HL.full, 8);
            break;
        case 0x3B:
            sprintf(buf, "DEC SP");
            debug(opcode,buf);
            cpu_16bit_dec(&gbe.cpu.SP.full, 8);
            break;

            // jumps
        case 0xE9: // unconditional HL jump
            sprintf(buf, "JP (HL) 0x%04x", gbe.cpu.HL.full);
            debug(opcode,buf);
            gbe.cpu.cycles += 4;
            gbe.cpu.PC = gbe.cpu.HL.full;
            break;
        case 0xC3:
            sprintf(buf, "JP, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_jmp(0, 0, 0);
            break;
        case 0xC2:
            sprintf(buf, "JP NZ, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_jmp(1, FLAG_Z, 0);
            break;
        case 0xCA:
            sprintf(buf, "JP Z, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_jmp(1, FLAG_Z, 1);
            break;
        case 0xD2:
            sprintf(buf, "JP NC, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_jmp(1, FLAG_C, 0);
            break;
        case 0xDA:
            sprintf(buf, "JP C, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_jmp(1, FLAG_C, 1);
            break;

            // direct jumps
        case 0x18:
            sprintf(buf, "JR 0x%04x", read_16bit() + (int8_t) memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_jmp_direct(0, 0, 0);
            break;
        case 0x20:
            sprintf(buf, "JR NZ, 0x%04x", read_16bit() + (int8_t) memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_jmp_direct(1, FLAG_Z, 0);
            break;
        case 0x28:
            sprintf(buf, "JR Z, 0x%04x", read_16bit() + (int8_t) memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_jmp_direct(1, FLAG_Z, 1);
            break;
        case 0x30:
            sprintf(buf, "JR NC, 0x%04x", read_16bit() + (int8_t) memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_jmp_direct(1, FLAG_C, 0);
            break;
        case 0x38:
            sprintf(buf, "JR Z, 0x%04x", read_16bit() + (int8_t) memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            cpu_jmp_direct(1, FLAG_C, 1);
            break;

            // calls
        case 0xCD:
            sprintf(buf, "CALL 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_call(0, 0, 0);
            break;
        case 0xC4:
            sprintf(buf, "CALL NZ, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_call(1, FLAG_Z, 0);
            break;
        case 0xCC:
            sprintf(buf, "CALL Z, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_call(1, FLAG_Z, 1);
            break;
        case 0xD4:
            sprintf(buf, "CALL NC, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_call(1, FLAG_C, 0);
            break;
        case 0xDC:
            sprintf(buf, "CALL C, 0x%04x", read_16bit());
            debug(opcode,buf);
            cpu_call(1, FLAG_C, 1);
            break;

            // returns
        case 0xC9:
            sprintf(buf, "RET");
            debug(opcode,buf);
            cpu_ret(0, 0, 0);
            break;
        case 0xC0:
            sprintf(buf, "RET NZ");
            debug(opcode,buf);
            cpu_ret(1, FLAG_Z, 0);
            break;
        case 0xC8:
            sprintf(buf, "RET Z");
            debug(opcode,buf);
            cpu_ret(1, FLAG_Z, 1);
            break;
        case 0xD0:
            sprintf(buf, "RET NC");
            debug(opcode,buf);
            cpu_ret(1, FLAG_C, 0);
            break;
        case 0xD8:
            sprintf(buf, "RET C");
            debug(opcode,buf);
            cpu_ret(1, FLAG_C, 1);
            break;

            // cpu "restarts"
        case 0xC7:
            sprintf(buf, "RST 0x00");
            debug(opcode,buf);
            cpu_rst(0x00);
            break;
        case 0xCF:
            sprintf(buf, "RST 0x08");
            debug(opcode,buf);
            cpu_rst(0x08);
            break;
        case 0xD7:
            sprintf(buf, "RST 0x10");
            debug(opcode,buf);
            cpu_rst(0x10);
            break;
        case 0xDF:
            sprintf(buf, "RST 0x18");
            debug(opcode,buf);
            cpu_rst(0x18);
            break;
        case 0xE7:
            sprintf(buf, "RST 0x20");
            debug(opcode,buf);
            cpu_rst(0x20);
            break;
        case 0xEF:
            sprintf(buf, "RST 0x28");
            debug(opcode,buf);
            cpu_rst(0x28);
            break;
        case 0xF7:
            sprintf(buf, "RST 0x30");
            debug(opcode,buf);
            cpu_rst(0x30);
            break;
        case 0xFF:
            sprintf(buf, "RST 0x38");
            debug(opcode,buf);
            cpu_rst(0x38);
            break;
        case 0x27:
            sprintf(buf, "DAA");
            debug(opcode,buf);
            cpu_daa();
            break;
            // decimal adjust accumulator
        case 0xCB:
            Execute8bitExtension();
            break;

            // niche instructions (check these first if emulation goes south!)
        case 0x07:
            sprintf(buf, "RLCA");
            debug(opcode,buf);
            cpu_rlc(&gbe.cpu.AF.hi);
            break;
        case 0x0F:
            sprintf(buf, "RRCA");
            debug(opcode,buf);
            cpu_rrc(&gbe.cpu.AF.hi);
            break;
        case 0x17:
            sprintf(buf, "RLA");
            debug(opcode,buf);
            cpu_rl(&gbe.cpu.AF.hi);
            break;
        case 0x1F:
            sprintf(buf, "RRA");
            debug(opcode,buf);
            cpu_rr(&gbe.cpu.AF.hi);
            break;

        case 0xD9: // return from interrupt
            sprintf(buf, "RETI");
            debug(opcode,buf);
            gbe.cpu.PC = stackPop();
            gbe.cpu.ime = 1;
            gbe.cpu.cycles += 16;
            break;

        case 0x08:
            sprintf(buf, "LD (0x%04x), SP", read_16bit());
            debug(opcode,buf);
            uval16t = read_16bit();
            gbe.cpu.PC += 2;
            memoryWrite(uval16t, gbe.cpu.SP.lo);
            uval16t++;
            memoryWrite(uval16t, gbe.cpu.SP.hi);
            gbe.cpu.cycles += 20;
            break;
        case 0x36: // special load 8bit value in address specified in HL register
            sprintf(buf, "LD (HL) 0x%04x, %d", gbe.cpu.HL.full, memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            sprintf(buf, "RLCA");
            debug(opcode,buf);
            gbe.cpu.cycles += 12;
            uval8t = memoryRead(gbe.cpu.PC);
            memoryWrite(gbe.cpu.HL.full, uval8t);
            gbe.cpu.PC++;
            break;
        case 0xFA: // read byte from PC+1 and store its value in register F
            sprintf(buf, "LD A, 0x%04x", read_16bit());
            debug(opcode,buf);
            gbe.cpu.cycles += 16;
            gbe.cpu.AF.hi = memoryRead(read_16bit());
            gbe.cpu.PC += 2;
            break;
        case 0xFB: // enable Interrupt master enable
            sprintf(buf, "EI");
            debug(opcode,buf);
            gbe.cpu.imePendingEnable = 1;
            gbe.cpu.cycles += 4;
            break;
        case 0x3E: // 8 bit load into A
            sprintf(buf, "LD A, %d", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            gbe.cpu.cycles += 8;
            uval8t = memoryRead(gbe.cpu.PC);
            gbe.cpu.PC++;
            gbe.cpu.AF.hi = uval8t;
            break;
        case 0xEA: // write 8 bit value into F from address stored in memory
            sprintf(buf, "LD 0x%04x, A", read_16bit());
            debug(opcode,buf);
            gbe.cpu.cycles += 16;
            uval16t = read_16bit();
            gbe.cpu.PC += 2;
            memoryWrite(uval16t, gbe.cpu.AF.hi);
            break;
        case 0xF3:
            sprintf(buf, "DI");
            debug(opcode,buf);
            gbe.cpu.imePendingDisable = 1;
            gbe.cpu.cycles += 4;
            break;
        case 0xE8: // SP +n
            sprintf(buf, "ADD SP, 0x%02x", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            gbe.cpu.cycles += 16;
            gbe.cpu.SP.full += memoryRead(gbe.cpu.PC);
            gbe.cpu.PC++;
            break;
        case 0xE0:
            sprintf(buf, "LDH 0x%02x, A", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            uval8t = memoryRead(gbe.cpu.PC);
            gbe.cpu.PC++;
            uint16_t address = 0xFF00 + uval8t;
            memoryWrite(address, gbe.cpu.AF.hi);
            gbe.cpu.cycles += 12;
            break;

        case 0xF0:
            uval8t = memoryRead(gbe.cpu.PC);
            gbe.cpu.PC++;
            uval16t = 0xFF00 + uval8t;
            gbe.cpu.AF.hi = memoryRead(uval16t);
            gbe.cpu.cycles += 12;
            sprintf(buf, "LDH A, 0x%04x", uval16t);
            debug(opcode,buf);
            break;

        case 0x2F:
            sprintf(buf, "CPL");
            debug(opcode,buf);
            gbe.cpu.cycles += 4;
            gbe.cpu.AF.hi = ~gbe.cpu.AF.hi;

            gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_N);
            gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
            break;

        case 0x76: // CPU HALT
            sprintf(buf, "HALT");
            debug(opcode,buf);
            gbe.cpu.cycles += 4;
            gbe.cpu.halted = 1;
            break;

        case 0x37:
            sprintf(buf, "SCF");
            debug(opcode,buf);
            gbe.cpu.cycles += 4;
            gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);
            break;
        case 0x3F:
            sprintf(buf, "CCF");
            debug(opcode,buf);
            gbe.cpu.cycles += 4;
            if (bit_test(gbe.cpu.AF.lo, FLAG_C)) {
                gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_C);
            } else {
                gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
            }
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);
            break;

        case 0xF8:
            sprintf(buf, "LD HL, SP + %02x", memoryRead(gbe.cpu.PC));
            debug(opcode,buf);
            sval8t = memoryRead(gbe.cpu.PC);
            uint16_t sp = gbe.cpu.SP.full;
            gbe.cpu.PC++;
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
            gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);


            int result = sp + sval8t;
            gbe.cpu.AF.lo = 0;
            if ((result & 0x10000) != 0) {
                gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
            }
            if (((sp & 0xFFF) + (sval8t & 0xFFF) > 0xFFF)) {
                gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
            }
            gbe.cpu.HL.full = (uint16_t) result;
            break;

        case 0x10:
            sprintf(buf, "STOP");
            debug(opcode,buf);
            gbe.cpu.PC++;
            gbe.cpu.cycles += 4;
            break;
        default:
            printf("[DEF] 0x%02x unknown opcode\n", opcode);
            exit(1);
            break;
    }
    return cycles;
}

void Execute8bitExtension() {
    uint8_t opcode = gbe.ram[gbe.cpu.PC];
    if (((gbe.cpu.PC >= 0x4000 && gbe.cpu.PC <= 0x7FFF) || (gbe.cpu.PC >= 0xA000 && gbe.cpu.PC <= 0xBFFF))) { // if out of bounds, let memoryRead() decide
        opcode = memoryRead(gbe.cpu.PC);
    }
    //printf("E 0x%02x PC: 0x%04x SP: 0x%04x A: 0x%02x F: 0x%02x BC: 0x%04x DE: 0x%04x HL: 0x%04x\n", opcode, gbe.cpu.PC - 1, gbe.cpu.SP.full, gbe.cpu.AF.lo, gbe.cpu.AF.hi, gbe.cpu.BC.full, gbe.cpu.DE.full, gbe.cpu.HL.full);

    gbe.cpu.PC++;

    switch (opcode) {
            // rotate left through carry
        case 0x0: cpu_rlc(&gbe.cpu.BC.hi);
            break;
        case 0x1: cpu_rlc(&gbe.cpu.BC.lo);
            break;
        case 0x2: cpu_rlc(&gbe.cpu.DE.hi);
            break;
        case 0x3: cpu_rlc(&gbe.cpu.DE.lo);
            break;
        case 0x4: cpu_rlc(&gbe.cpu.HL.hi);
            break;
        case 0x5: cpu_rlc(&gbe.cpu.HL.lo);
            break;
        case 0x6: mem_rlc(gbe.cpu.HL.full);
            break;
        case 0x7: cpu_rlc(&gbe.cpu.AF.hi);
            break;

            // rotate right through carry
        case 0x8: cpu_rrc(&gbe.cpu.BC.hi);
            break;
        case 0x9: cpu_rrc(&gbe.cpu.BC.lo);
            break;
        case 0xA: cpu_rrc(&gbe.cpu.DE.hi);
            break;
        case 0xB: cpu_rrc(&gbe.cpu.DE.lo);
            break;
        case 0xC: cpu_rrc(&gbe.cpu.HL.hi);
            break;
        case 0xD: cpu_rrc(&gbe.cpu.HL.lo);
            break;
        case 0xE: mem_rrc(gbe.cpu.HL.full);
            break;
        case 0xF: cpu_rrc(&gbe.cpu.AF.hi);
            break;

            // rotate left
        case 0x10: cpu_rl(&gbe.cpu.BC.hi);
            break;
        case 0x11: cpu_rl(&gbe.cpu.BC.lo);
            break;
        case 0x12: cpu_rl(&gbe.cpu.DE.hi);
            break;
        case 0x13: cpu_rl(&gbe.cpu.DE.lo);
            break;
        case 0x14: cpu_rl(&gbe.cpu.HL.hi);
            break;
        case 0x15: cpu_rl(&gbe.cpu.HL.lo);
            break;
        case 0x16: mem_rl(gbe.cpu.HL.full);
            break;
        case 0x17: cpu_rl(&gbe.cpu.AF.hi);
            break;

            // rotate right
        case 0x18: cpu_rr(&gbe.cpu.BC.hi);
            break;
        case 0x19: cpu_rr(&gbe.cpu.BC.lo);
            break;
        case 0x1A: cpu_rr(&gbe.cpu.DE.hi);
            break;
        case 0x1B: cpu_rr(&gbe.cpu.DE.lo);
            break;
        case 0x1C: cpu_rr(&gbe.cpu.HL.hi);
            break;
        case 0x1D: cpu_rr(&gbe.cpu.HL.lo);
            break;
        case 0x1E: mem_rr(gbe.cpu.HL.full);
            break;
        case 0x1F: cpu_rr(&gbe.cpu.AF.hi);
            break;

        case 0x20: cpu_sla(&gbe.cpu.BC.hi);
            break;
        case 0x21: cpu_sla(&gbe.cpu.BC.lo);
            break;
        case 0x22: cpu_sla(&gbe.cpu.DE.hi);
            break;
        case 0x23: cpu_sla(&gbe.cpu.DE.lo);
            break;
        case 0x24: cpu_sla(&gbe.cpu.HL.hi);
            break;
        case 0x25: cpu_sla(&gbe.cpu.HL.lo);
            break;
        case 0x26: mem_sla(gbe.cpu.HL.full);
            break;
        case 0x27: cpu_sla(&gbe.cpu.AF.hi);
            break;

        case 0x28: cpu_sra(&gbe.cpu.BC.hi);
            break;
        case 0x29: cpu_sra(&gbe.cpu.BC.lo);
            break;
        case 0x2A: cpu_sra(&gbe.cpu.DE.hi);
            break;
        case 0x2B: cpu_sra(&gbe.cpu.DE.lo);
            break;
        case 0x2C: cpu_sra(&gbe.cpu.HL.hi);
            break;
        case 0x2D: cpu_sra(&gbe.cpu.HL.lo);
            break;
        case 0x2E: mem_sra(gbe.cpu.HL.full);
            break;
        case 0x2F: cpu_sra(&gbe.cpu.AF.hi);
            break;

        case 0x38: cpu_srl(&gbe.cpu.BC.hi);
            break;
        case 0x39: cpu_srl(&gbe.cpu.BC.lo);
            break;
        case 0x3A: cpu_srl(&gbe.cpu.DE.hi);
            break;
        case 0x3B: cpu_srl(&gbe.cpu.DE.lo);
            break;
        case 0x3C: cpu_srl(&gbe.cpu.HL.hi);
            break;
        case 0x3D: cpu_srl(&gbe.cpu.HL.lo);
            break;
        case 0x3E: mem_srl(gbe.cpu.HL.full);
            break;
        case 0x3F: cpu_srl(&gbe.cpu.AF.hi);
            break;

            // swap nibbles
        case 0x37: cpu_nibble_swap(&gbe.cpu.AF.hi);
            break;
        case 0x30: cpu_nibble_swap(&gbe.cpu.BC.hi);
            break;
        case 0x31: cpu_nibble_swap(&gbe.cpu.BC.lo);
            break;
        case 0x32: cpu_nibble_swap(&gbe.cpu.DE.hi);
            break;
        case 0x33: cpu_nibble_swap(&gbe.cpu.DE.lo);
            break;
        case 0x34: cpu_nibble_swap(&gbe.cpu.HL.hi);
            break;
        case 0x35: cpu_nibble_swap(&gbe.cpu.HL.lo);
            break;
        case 0x36: mem_nibble_swap(gbe.cpu.HL.full);
            break;

            // test bit
        case 0x40: cpu_test_bit(&gbe.cpu.BC.hi, 0, 8);
            break;
        case 0x41: cpu_test_bit(&gbe.cpu.BC.lo, 0, 8);
            break;
        case 0x42: cpu_test_bit(&gbe.cpu.DE.hi, 0, 8);
            break;
        case 0x43: cpu_test_bit(&gbe.cpu.DE.lo, 0, 8);
            break;
        case 0x44: cpu_test_bit(&gbe.cpu.HL.hi, 0, 8);
            break;
        case 0x45: cpu_test_bit(&gbe.cpu.HL.lo, 0, 8);
            break;
        case 0x46: mem_test_bit(gbe.cpu.HL.full, 0, 16);
            break;
        case 0x47: cpu_test_bit(&gbe.cpu.AF.hi, 0, 8);
            break;
        case 0x48: cpu_test_bit(&gbe.cpu.BC.hi, 1, 8);
            break;
        case 0x49: cpu_test_bit(&gbe.cpu.BC.lo, 1, 8);
            break;
        case 0x4A: cpu_test_bit(&gbe.cpu.DE.hi, 1, 8);
            break;
        case 0x4B: cpu_test_bit(&gbe.cpu.DE.lo, 1, 8);
            break;
        case 0x4C: cpu_test_bit(&gbe.cpu.HL.hi, 1, 8);
            break;
        case 0x4D: cpu_test_bit(&gbe.cpu.HL.lo, 1, 8);
            break;
        case 0x4E: mem_test_bit(gbe.cpu.HL.full, 1, 16);
            break;
        case 0x4F: cpu_test_bit(&gbe.cpu.AF.hi, 1, 8);
            break;
        case 0x50: cpu_test_bit(&gbe.cpu.BC.hi, 2, 8);
            break;
        case 0x51: cpu_test_bit(&gbe.cpu.BC.lo, 2, 8);
            break;
        case 0x52: cpu_test_bit(&gbe.cpu.DE.hi, 2, 8);
            break;
        case 0x53: cpu_test_bit(&gbe.cpu.DE.lo, 2, 8);
            break;
        case 0x54: cpu_test_bit(&gbe.cpu.HL.hi, 2, 8);
            break;
        case 0x55: cpu_test_bit(&gbe.cpu.HL.lo, 2, 8);
            break;
        case 0x56: mem_test_bit(gbe.cpu.HL.full, 2, 16);
            break;
        case 0x57: cpu_test_bit(&gbe.cpu.AF.hi, 2, 8);
            break;
        case 0x58: cpu_test_bit(&gbe.cpu.BC.hi, 3, 8);
            break;
        case 0x59: cpu_test_bit(&gbe.cpu.BC.lo, 3, 8);
            break;
        case 0x5A: cpu_test_bit(&gbe.cpu.DE.hi, 3, 8);
            break;
        case 0x5B: cpu_test_bit(&gbe.cpu.DE.lo, 3, 8);
            break;
        case 0x5C: cpu_test_bit(&gbe.cpu.HL.hi, 3, 8);
            break;
        case 0x5D: cpu_test_bit(&gbe.cpu.HL.lo, 3, 8);
            break;
        case 0x5E: mem_test_bit(gbe.cpu.HL.full, 3, 16);
            break;
        case 0x5F: cpu_test_bit(&gbe.cpu.AF.hi, 3, 8);
            break;
        case 0x60: cpu_test_bit(&gbe.cpu.BC.hi, 4, 8);
            break;
        case 0x61: cpu_test_bit(&gbe.cpu.BC.lo, 4, 8);
            break;
        case 0x62: cpu_test_bit(&gbe.cpu.DE.hi, 4, 8);
            break;
        case 0x63: cpu_test_bit(&gbe.cpu.DE.lo, 4, 8);
            break;
        case 0x64: cpu_test_bit(&gbe.cpu.HL.hi, 4, 8);
            break;
        case 0x65: cpu_test_bit(&gbe.cpu.HL.lo, 4, 8);
            break;
        case 0x66: mem_test_bit(gbe.cpu.HL.full, 4, 16);
            break;
        case 0x67: cpu_test_bit(&gbe.cpu.AF.hi, 4, 8);
            break;
        case 0x68: cpu_test_bit(&gbe.cpu.BC.hi, 5, 8);
            break;
        case 0x69: cpu_test_bit(&gbe.cpu.BC.lo, 5, 8);
            break;
        case 0x6A: cpu_test_bit(&gbe.cpu.DE.hi, 5, 8);
            break;
        case 0x6B: cpu_test_bit(&gbe.cpu.DE.lo, 5, 8);
            break;
        case 0x6C: cpu_test_bit(&gbe.cpu.HL.hi, 5, 8);
            break;
        case 0x6D: cpu_test_bit(&gbe.cpu.HL.lo, 5, 8);
            break;
        case 0x6E: mem_test_bit(gbe.cpu.HL.full, 5, 16);
            break;
        case 0x6F: cpu_test_bit(&gbe.cpu.AF.hi, 5, 8);
            break;
        case 0x70: cpu_test_bit(&gbe.cpu.BC.hi, 6, 8);
            break;
        case 0x71: cpu_test_bit(&gbe.cpu.BC.lo, 6, 8);
            break;
        case 0x72: cpu_test_bit(&gbe.cpu.DE.hi, 6, 8);
            break;
        case 0x73: cpu_test_bit(&gbe.cpu.DE.lo, 6, 8);
            break;
        case 0x74: cpu_test_bit(&gbe.cpu.HL.hi, 6, 8);
            break;
        case 0x75: cpu_test_bit(&gbe.cpu.HL.lo, 6, 8);
            break;
        case 0x76: mem_test_bit(gbe.cpu.HL.full, 6, 16);
            break;
        case 0x77: cpu_test_bit(&gbe.cpu.AF.hi, 6, 8);
            break;
        case 0x78: cpu_test_bit(&gbe.cpu.BC.hi, 7, 8);
            break;
        case 0x79: cpu_test_bit(&gbe.cpu.BC.lo, 7, 8);
            break;
        case 0x7A: cpu_test_bit(&gbe.cpu.DE.hi, 7, 8);
            break;
        case 0x7B: cpu_test_bit(&gbe.cpu.DE.lo, 7, 8);
            break;
        case 0x7C: cpu_test_bit(&gbe.cpu.HL.hi, 7, 8);
            break;
        case 0x7D: cpu_test_bit(&gbe.cpu.HL.lo, 7, 8);
            break;
        case 0x7E: mem_test_bit(gbe.cpu.HL.full, 7, 16);
            break;
        case 0x7F: cpu_test_bit(&gbe.cpu.AF.hi, 7, 8);
            break;

            // reset bit
        case 0x80: cpu_reset_bit(&gbe.cpu.BC.hi, 0);
            break;
        case 0x81: cpu_reset_bit(&gbe.cpu.BC.lo, 0);
            break;
        case 0x82: cpu_reset_bit(&gbe.cpu.DE.hi, 0);
            break;
        case 0x83: cpu_reset_bit(&gbe.cpu.DE.lo, 0);
            break;
        case 0x84: cpu_reset_bit(&gbe.cpu.HL.hi, 0);
            break;
        case 0x85: cpu_reset_bit(&gbe.cpu.HL.lo, 0);
            break;
        case 0x86: mem_reset_bit(gbe.cpu.HL.full, 0);
            break;
        case 0x87: cpu_reset_bit(&gbe.cpu.AF.hi, 0);
            break;
        case 0x88: cpu_reset_bit(&gbe.cpu.BC.hi, 1);
            break;
        case 0x89: cpu_reset_bit(&gbe.cpu.BC.lo, 1);
            break;
        case 0x8A: cpu_reset_bit(&gbe.cpu.DE.hi, 1);
            break;
        case 0x8B: cpu_reset_bit(&gbe.cpu.DE.lo, 1);
            break;
        case 0x8C: cpu_reset_bit(&gbe.cpu.HL.hi, 1);
            break;
        case 0x8D: cpu_reset_bit(&gbe.cpu.HL.lo, 1);
            break;
        case 0x8E: mem_reset_bit(gbe.cpu.HL.full, 1);
            break;
        case 0x8F: cpu_reset_bit(&gbe.cpu.AF.hi, 1);
            break;
        case 0x90: cpu_reset_bit(&gbe.cpu.BC.hi, 2);
            break;
        case 0x91: cpu_reset_bit(&gbe.cpu.BC.lo, 2);
            break;
        case 0x92: cpu_reset_bit(&gbe.cpu.DE.hi, 2);
            break;
        case 0x93: cpu_reset_bit(&gbe.cpu.DE.lo, 2);
            break;
        case 0x94: cpu_reset_bit(&gbe.cpu.HL.hi, 2);
            break;
        case 0x95: cpu_reset_bit(&gbe.cpu.HL.lo, 2);
            break;
        case 0x96: mem_reset_bit(gbe.cpu.HL.full, 2);
            break;
        case 0x97: cpu_reset_bit(&gbe.cpu.AF.hi, 2);
            break;
        case 0x98: cpu_reset_bit(&gbe.cpu.BC.hi, 3);
            break;
        case 0x99: cpu_reset_bit(&gbe.cpu.BC.lo, 3);
            break;
        case 0x9A: cpu_reset_bit(&gbe.cpu.DE.hi, 3);
            break;
        case 0x9B: cpu_reset_bit(&gbe.cpu.DE.lo, 3);
            break;
        case 0x9C: cpu_reset_bit(&gbe.cpu.HL.hi, 3);
            break;
        case 0x9D: cpu_reset_bit(&gbe.cpu.HL.lo, 3);
            break;
        case 0x9E: mem_reset_bit(gbe.cpu.HL.full, 3);
            break;
        case 0x9F: cpu_reset_bit(&gbe.cpu.AF.hi, 3);
            break;
        case 0xA0: cpu_reset_bit(&gbe.cpu.BC.hi, 4);
            break;
        case 0xA1: cpu_reset_bit(&gbe.cpu.BC.lo, 4);
            break;
        case 0xA2: cpu_reset_bit(&gbe.cpu.DE.hi, 4);
            break;
        case 0xA3: cpu_reset_bit(&gbe.cpu.DE.lo, 4);
            break;
        case 0xA4: cpu_reset_bit(&gbe.cpu.HL.hi, 4);
            break;
        case 0xA5: cpu_reset_bit(&gbe.cpu.HL.lo, 4);
            break;
        case 0xA6: mem_reset_bit(gbe.cpu.HL.full, 4);
            break;
        case 0xA7: cpu_reset_bit(&gbe.cpu.AF.hi, 4);
            break;
        case 0xA8: cpu_reset_bit(&gbe.cpu.BC.hi, 5);
            break;
        case 0xA9: cpu_reset_bit(&gbe.cpu.BC.lo, 5);
            break;
        case 0xAA: cpu_reset_bit(&gbe.cpu.DE.hi, 5);
            break;
        case 0xAB: cpu_reset_bit(&gbe.cpu.DE.lo, 5);
            break;
        case 0xAC: cpu_reset_bit(&gbe.cpu.HL.hi, 5);
            break;
        case 0xAD: cpu_reset_bit(&gbe.cpu.HL.lo, 5);
            break;
        case 0xAE: mem_reset_bit(gbe.cpu.HL.full, 5);
            break;
        case 0xAF: cpu_reset_bit(&gbe.cpu.AF.hi, 5);
            break;
        case 0xB0: cpu_reset_bit(&gbe.cpu.BC.hi, 6);
            break;
        case 0xB1: cpu_reset_bit(&gbe.cpu.BC.lo, 6);
            break;
        case 0xB2: cpu_reset_bit(&gbe.cpu.DE.hi, 6);
            break;
        case 0xB3: cpu_reset_bit(&gbe.cpu.DE.lo, 6);
            break;
        case 0xB4: cpu_reset_bit(&gbe.cpu.HL.hi, 6);
            break;
        case 0xB5: cpu_reset_bit(&gbe.cpu.HL.lo, 6);
            break;
        case 0xB6: mem_reset_bit(gbe.cpu.HL.full, 6);
            break;
        case 0xB7: cpu_reset_bit(&gbe.cpu.AF.hi, 6);
            break;
        case 0xB8: cpu_reset_bit(&gbe.cpu.BC.hi, 7);
            break;
        case 0xB9: cpu_reset_bit(&gbe.cpu.BC.lo, 7);
            break;
        case 0xBA: cpu_reset_bit(&gbe.cpu.DE.hi, 7);
            break;
        case 0xBB: cpu_reset_bit(&gbe.cpu.DE.lo, 7);
            break;
        case 0xBC: cpu_reset_bit(&gbe.cpu.HL.hi, 7);
            break;
        case 0xBD: cpu_reset_bit(&gbe.cpu.HL.lo, 7);
            break;
        case 0xBE: mem_reset_bit(gbe.cpu.HL.full, 7);
            break;
        case 0xBF: cpu_reset_bit(&gbe.cpu.AF.hi, 7);
            break;


            // set bit
        case 0xC0: cpu_set_bit(&gbe.cpu.BC.hi, 0);
            break;
        case 0xC1: cpu_set_bit(&gbe.cpu.BC.lo, 0);
            break;
        case 0xC2: cpu_set_bit(&gbe.cpu.DE.hi, 0);
            break;
        case 0xC3: cpu_set_bit(&gbe.cpu.DE.lo, 0);
            break;
        case 0xC4: cpu_set_bit(&gbe.cpu.HL.hi, 0);
            break;
        case 0xC5: cpu_set_bit(&gbe.cpu.HL.lo, 0);
            break;
        case 0xC6: mem_set_bit(gbe.cpu.HL.full, 0);
            break;
        case 0xC7: cpu_set_bit(&gbe.cpu.AF.hi, 0);
            break;
        case 0xC8: cpu_set_bit(&gbe.cpu.BC.hi, 1);
            break;
        case 0xC9: cpu_set_bit(&gbe.cpu.BC.lo, 1);
            break;
        case 0xCA: cpu_set_bit(&gbe.cpu.DE.hi, 1);
            break;
        case 0xCB: cpu_set_bit(&gbe.cpu.DE.lo, 1);
            break;
        case 0xCC: cpu_set_bit(&gbe.cpu.HL.hi, 1);
            break;
        case 0xCD: cpu_set_bit(&gbe.cpu.HL.lo, 1);
            break;
        case 0xCE: mem_set_bit(gbe.cpu.HL.full, 1);
            break;
        case 0xCF: cpu_set_bit(&gbe.cpu.AF.hi, 1);
            break;
        case 0xD0: cpu_set_bit(&gbe.cpu.BC.hi, 2);
            break;
        case 0xD1: cpu_set_bit(&gbe.cpu.BC.lo, 2);
            break;
        case 0xD2: cpu_set_bit(&gbe.cpu.DE.hi, 2);
            break;
        case 0xD3: cpu_set_bit(&gbe.cpu.DE.lo, 2);
            break;
        case 0xD4: cpu_set_bit(&gbe.cpu.HL.hi, 2);
            break;
        case 0xD5: cpu_set_bit(&gbe.cpu.HL.lo, 2);
            break;
        case 0xD6:mem_set_bit(gbe.cpu.HL.full, 2);
            break;
        case 0xD7: cpu_set_bit(&gbe.cpu.AF.hi, 2);
            break;
        case 0xD8: cpu_set_bit(&gbe.cpu.BC.hi, 3);
            break;
        case 0xD9: cpu_set_bit(&gbe.cpu.BC.lo, 3);
            break;
        case 0xDA: cpu_set_bit(&gbe.cpu.DE.hi, 3);
            break;
        case 0xDB: cpu_set_bit(&gbe.cpu.DE.lo, 3);
            break;
        case 0xDC: cpu_set_bit(&gbe.cpu.HL.hi, 3);
            break;
        case 0xDD: cpu_set_bit(&gbe.cpu.HL.lo, 3);
            break;
        case 0xDE: mem_set_bit(gbe.cpu.HL.full, 3);
            break;
        case 0xDF: cpu_set_bit(&gbe.cpu.AF.hi, 3);
            break;
        case 0xE0: cpu_set_bit(&gbe.cpu.BC.hi, 4);
            break;
        case 0xE1: cpu_set_bit(&gbe.cpu.BC.lo, 4);
            break;
        case 0xE2: cpu_set_bit(&gbe.cpu.DE.hi, 4);
            break;
        case 0xE3: cpu_set_bit(&gbe.cpu.DE.lo, 4);
            break;
        case 0xE4: cpu_set_bit(&gbe.cpu.HL.hi, 4);
            break;
        case 0xE5: cpu_set_bit(&gbe.cpu.HL.lo, 4);
            break;
        case 0xE6: mem_set_bit(gbe.cpu.HL.full, 4);
            break;
        case 0xE7: cpu_set_bit(&gbe.cpu.AF.hi, 4);
            break;
        case 0xE8: cpu_set_bit(&gbe.cpu.BC.hi, 5);
            break;
        case 0xE9: cpu_set_bit(&gbe.cpu.BC.lo, 5);
            break;
        case 0xEA: cpu_set_bit(&gbe.cpu.DE.hi, 5);
            break;
        case 0xEB: cpu_set_bit(&gbe.cpu.DE.lo, 5);
            break;
        case 0xEC: cpu_set_bit(&gbe.cpu.HL.hi, 5);
            break;
        case 0xED: cpu_set_bit(&gbe.cpu.HL.lo, 5);
            break;
        case 0xEE: mem_set_bit(gbe.cpu.HL.full, 5);
            break;
        case 0xEF: cpu_set_bit(&gbe.cpu.AF.hi, 5);
            break;
        case 0xF0: cpu_set_bit(&gbe.cpu.BC.hi, 6);
            break;
        case 0xF1: cpu_set_bit(&gbe.cpu.BC.lo, 6);
            break;
        case 0xF2: cpu_set_bit(&gbe.cpu.DE.hi, 6);
            break;
        case 0xF3: cpu_set_bit(&gbe.cpu.DE.lo, 6);
            break;
        case 0xF4: cpu_set_bit(&gbe.cpu.HL.hi, 6);
            break;
        case 0xF5: cpu_set_bit(&gbe.cpu.HL.lo, 6);
            break;
        case 0xF6: mem_set_bit(gbe.cpu.HL.full, 6);
            break;
        case 0xF7: cpu_set_bit(&gbe.cpu.AF.hi, 6);
            break;
        case 0xF8: cpu_set_bit(&gbe.cpu.BC.hi, 7);
            break;
        case 0xF9: cpu_set_bit(&gbe.cpu.BC.lo, 7);
            break;
        case 0xFA: cpu_set_bit(&gbe.cpu.DE.hi, 7);
            break;
        case 0xFB: cpu_set_bit(&gbe.cpu.DE.lo, 7);
            break;
        case 0xFC: cpu_set_bit(&gbe.cpu.HL.hi, 7);
            break;
        case 0xFD: cpu_set_bit(&gbe.cpu.HL.lo, 7);
            break;
        case 0xFE: mem_set_bit(gbe.cpu.HL.full, 7);
            break;
        case 0xFF: cpu_set_bit(&gbe.cpu.AF.hi, 7);
            break;

        default:
            if (gbe.log) {
                //printf("[EXT] 0x%02x  PC: 0x%04x SP: 0x%04x A: 0x%02x F: 0x%02x BC: 0x%02x DE: 0x%02x HL: 0x%02x\n", opcode, gbe.cpu.PC, gbe.cpu.SP.full, gbe.cpu.AF.lo, gbe.cpu.AF.hi, gbe.cpu.BC.full, gbe.cpu.DE.full, gbe.cpu.HL.full);
            }
            exit(1);
    }

}

void debug(uint8_t opcode, char buf[]) {
    if (gbe.log) {
        //printf("A:%02x F:%c%c%c%c BC:%04x DE:%04x HL:%04x SP:%04x PC:%04x OP:%02x %s\n", gbe.cpu.AF.hi, testFlag(FLAG_Z) ? 'Z' : '-', testFlag(FLAG_N) ? 'N' : '-', testFlag(FLAG_H) ? 'H' : '-', testFlag(FLAG_C) ? 'C' : '-', gbe.cpu.BC.full, gbe.cpu.DE.full, gbe.cpu.HL.full, gbe.cpu.SP.full, gbe.cpu.PC -1, opcode, buf);

    }
}