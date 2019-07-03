/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

// all cpu instructions
// moet nog maar eens na lopen omdat er toch iets mis is


#include <unistd.h>
#include "lr35902.h"
#include "machine.h"
#include "memory.h"

uint16_t read_16bit(void);

void setFlag(int flag);
void toggleFlag(int flag);
uint8_t testFlag(int flag);
void cpu_8bit_load(uint8_t *regPtr);
void cpu_8bit_add(uint8_t *regPtr, uint8_t, int cycles, int direct, int carry);
void cpu_8bit_adc(uint8_t val);
void cpu_8bit_sub(uint8_t *regPtr, uint8_t val, int cycles, int direct, int carry);
void cpu_8bit_and(uint8_t *regPtr, uint8_t val, int cycles, int direct);
void cpu_8bit_or(uint8_t *regPtr, uint8_t val, int cycles, int direct);
void cpu_8bit_xor(uint8_t *regPtr, uint8_t val, int cycles, int direct);
void cpu_8bit_inc(uint8_t *regPtr, int cycles);
void cpu_8bit_dec(uint8_t *regPtr, int cycles);

void mem_8bit_inc(uint16_t addr, int cycles);
void mem_8bit_dec(uint16_t addr, int cycles);

void cpu_16bit_load(uint16_t *regPtr);
void cpu_16bit_add(uint16_t *regPtr, uint16_t val, int cycles);
void cpu_16bit_inc(uint16_t *regPtr, int cycles);
void cpu_16bit_dec(uint16_t *regPtr, int cycles);

void cpu_reg_load(uint8_t *regPtr, uint8_t val, int cycles);
void cpu_reg_load_direct(uint8_t *regPtr, uint16_t addr);

void cpu_jmp(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);
void cpu_jmp_direct(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);

void cpu_call(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);
void cpu_ret(uint8_t returnOnCondition, uint8_t flag, uint8_t returnConditionFlag);
void cpu_rst(uint8_t val);

void cpu_nibble_swap(uint8_t *regPtr);
void mem_nibble_swap(uint16_t addr);
void cpu_reset_bit(uint8_t *regPtr, int bit);
void mem_reset_bit(uint16_t addr, int bit);
void cpu_test_bit(uint8_t *regPtr, int bit, int cycles);
void mem_test_bit(uint16_t addr, int bit, int cycles);

void cpu_daa(void);

void cpu_rrc(uint8_t *regPtr);
void mem_rrc(uint16_t addr);
void cpu_rlc(uint8_t *regPtr);
void mem_rlc(uint16_t addr);
void cpu_rl(uint8_t *regPtr);
void mem_rl(uint16_t addr);
void cpu_rr(uint8_t *regPtr);
void mem_rr(uint16_t addr);
void cpu_sla(uint8_t *regPtr);
void mem_sla(uint16_t addr);
void cpu_sra(uint8_t *regPtr);
void mem_sra(uint16_t addr);
void cpu_srl(uint8_t *regPtr);
void mem_srl(uint16_t addr);

void setFlag(int flag) {
    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, flag);
}

void clearFlag(int flag) {
    bit_clear(gbe.cpu.AF.lo, flag);
}

uint8_t testFlag(int flag) {
    return bit_test(gbe.cpu.AF.lo, flag);
}

void toggleFlag(int flag) {
    gbe.cpu.AF.lo ^= 1 << flag;
}

uint16_t read_16bit() {
    // this function normally kills all big endian cpu compatibility, but can be fixed by checking ednianness using the preprocessor
    uint16_t val = memoryRead(gbe.cpu.PC + 1); // our memory read function is limited to 8 bits at a time.
    val = val << 8; // after loading the 8 bit value (byte) into our 16 bit value (word) we shit our byte one byte to the left.
    val |= memoryRead(gbe.cpu.PC);
    return val;
}

/*
 * start of 8bit functions
 */


// 8 bit load

void cpu_8bit_load(uint8_t* regPtr) {
    gbe.cpu.cycles += 8; // increase the cycle count with the amount of cpu cycles specific opcodes require

    *regPtr = memoryRead(gbe.cpu.PC); // the program counter already moved to the data we need to load in regPtr
    gbe.cpu.PC++; // skip the value after the opcode
}

// 8bit add

void cpu_8bit_add(uint8_t* regPtr, uint8_t val, int cycles, int direct, int carry) {

    gbe.cpu.cycles += cycles;
    uint8_t old = (*regPtr); // store the old value of the regPtrister so we can see if an add overflows it
    uint8_t add = 0; // value to add to old

    // when direct > 0 we load the value from ram[PC], if not its opcode specific which regPtrister we pull the value from.
    if (direct) {
        add = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++; // increment opcode to we skip our "opcode arguments"
    } else {
        add = val;
    }

    // is this an add with carry?
    if (carry) {
        add += testFlag(FLAG_C);

    }
    *regPtr += add;

    gbe.cpu.AF.lo = 0x00; // most instructions require the flag regPtrister to be wiped


    // do we need to set any flags or change the accumulator?

    // did our operation result in the regPtrister becoming 0? if so, set the Z flag!
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }

    // do we carry from the lower nibble to the higher nibble (00001111 to 00010000 for example), if we did, lets set the Half carry flag!
    uint16_t hcarry = (old & 0xF);
    hcarry += (add & 0xF);
    if (hcarry > 0xF) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    }

    // Did our add overflow our 8 bit regPtrister, if so, set the carry flag.
    if ((old + add) > 0xFF) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
}

// adc

void cpu_8bit_adc(uint8_t val) {
    gbe.cpu.cycles += 8;
    uint8_t a = gbe.cpu.AF.hi;
    uint8_t carry = testFlag(FLAG_C);
    int result = a + val + carry;

    if (result & 0xFF == 0) {
        setFlag(FLAG_Z);
    } else {
        clearFlag(FLAG_Z);
    }
    clearFlag(FLAG_N);
    if ((((a & 0xF) + (val & 0xF) + carry) & 0x10) != 0) {
        setFlag(FLAG_H);
        
    }else{
        clearFlag(FLAG_H);
    }
    if((((a & 0xFF) + (val & 0xFF) + carry) & 0x10) != 0) {
        setFlag(FLAG_C);
    }else {
        clearFlag(FLAG_C);
    }
    gbe.cpu.AF.hi = result;
}
// 8bit sub

void cpu_8bit_sub(uint8_t*regPtr, uint8_t val, int cycles, int direct, int carry) {
    gbe.cpu.cycles += cycles;

    uint8_t old = *regPtr;
    uint8_t sub = 0; // value to substract from old

    if (direct) {
        sub = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++;
    } else {
        sub = val;
    }

    if (carry) {
        if (bit_test(gbe.cpu.AF.lo, FLAG_C)) {
            sub++;
        }
    }
    *regPtr -= sub;
    gbe.cpu.AF.lo = 0x00;

    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_N); // we need to set the N flag (used by BCD instructions)

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }

    // set when not borrowing
    if (old < sub) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }

    signed short hcarry = (old & 0xF); // hcarry is signed in this case because "underflows" can happen.
    hcarry -= (sub & 0xF);

    if (hcarry < 0) { // did we underflow the upper nibble? If so, set the half carry flag
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    }

}

// 8bit bitwise AND

void cpu_8bit_and(uint8_t *regPtr, uint8_t val, int cycles, int direct) {
    gbe.cpu.cycles += cycles;

    uint8_t and = 0;
    if (direct) {
        and = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++;
    } else {
        and = val;
    }
    *regPtr &= and; // perform bitwise AND on regPtr

    gbe.cpu.AF.lo = 0x00;

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    // the half carry is always set for this instruction
    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
}

// 8bit bitwise OR

void cpu_8bit_or(uint8_t *regPtr, uint8_t val, int cycles, int direct) {
    gbe.cpu.cycles += cycles;

    uint8_t or = 0;

    if (direct) {
        or = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++;
        gbe.cpu.cycles++;
    } else {
        or = val;
    }
    *regPtr |= or;

    gbe.cpu.AF.lo = 0x00;
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// 8bit bitwise XOR

void cpu_8bit_xor(uint8_t *regPtr, uint8_t val, int cycles, int direct) {
    gbe.cpu.cycles += cycles;

    uint8_t xor = 0;

    if (direct) {
        xor = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++;
    } else {
        xor = val;
    }
    *regPtr ^= xor;

    gbe.cpu.AF.lo = 0x00;
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// 8bit cmp

void cpu_8bit_cmp(uint8_t reg, uint8_t val, int cycles, int direct) {
    gbe.cpu.cycles += cycles;

    uint8_t old = reg;
    uint8_t sub = 0; // value to compare our register with
    if (direct) {
        sub = memoryRead(gbe.cpu.PC);
        gbe.cpu.PC++;
    } else {
        sub = val;
    }
    reg -= sub;

    gbe.cpu.AF.lo = 0x00;

    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_N);

    if (reg == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }

    if (old < sub) { // set when not borrowing
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }

    int16_t hcarry = (old & 0xF); // hcarry is signed in this case because "underflows" can happen.
    hcarry -= (sub & 0xF);

    if (hcarry < 0) { // did we underflow the upper nibble? If so, set the half carry flag
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    }

}

// increment register by 1

void cpu_8bit_inc(uint8_t *regPtr, int cycles) {
    gbe.cpu.cycles += cycles;

    uint8_t old = *regPtr;
    (*regPtr)++;

    // we should not clear the flag register for this operation

    gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N); // this instruction requires the N flag to be reset.

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    } else { // clear the zero flag when the result isn't zero
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    }
    if ((old & 0xF) == 0xF) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
    }
}

void cpu_8bit_dec(uint8_t *regPtr, int cycles) {
    gbe.cpu.cycles += cycles;

    uint8_t old = *regPtr;
    (*regPtr)--;

    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_N); // But this instruction requires the N flag to be set!

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    }
    if ((old & 0xF) == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
    }
}

// same as the cpu inc but this time directly in memory

void mem_8bit_inc(uint16_t addr, int cycles) {
    gbe.cpu.cycles += cycles;

    uint8_t old = memoryRead(addr);
    memoryWrite(addr, (old + 1));
    uint8_t updated = old + 1;

    gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);

    if (updated == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    }

    if ((old & 0x0F) == 0xF) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
    }
}

void mem_8bit_dec(uint16_t addr, int cycles) {
    gbe.cpu.cycles += cycles;

    uint8_t old = memoryRead(addr);
    memoryWrite(addr, (old - 1));
    uint8_t updated = old - 1;


    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_N);

    if (updated == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    }

    if ((old & 0x0F) == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
    }
}

/*
 * End of 8bit functions
 */

/*
 * start of 16bit functions
 */


void cpu_16bit_load(uint16_t *regPtr) {
    gbe.cpu.cycles += 12; // 16 bit read takes 12 cpu cycles
    *regPtr = read_16bit();
    gbe.cpu.PC += 2;
}

void cpu_16bit_add(uint16_t *regPtr, uint16_t val, int cycles) {
    gbe.cpu.cycles += cycles;

    *regPtr += val;

    gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);

    if ((((*regPtr & 0xFFFF) + (*regPtr & 0xFFFF)) & 0x10000) != 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_C);
    }

    if ((((*regPtr & 0xFFF) + (*regPtr & 0xFFF)) & 0x1000) != 0) { // mask the 2 most significant nibbles of old and add val, if old changes it means we half carried into the upper 2 nibbles
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_H);
    }
}

void cpu_16bit_inc(uint16_t *regPtr, int cycles) {
    gbe.cpu.cycles += cycles;
    (*regPtr)++;
}

void cpu_16bit_dec(uint16_t *regPtr, int cycles) {
    gbe.cpu.cycles += cycles;
    (*regPtr)--;
}

/*
 * end of 16bit functions
 */

/*
 * start of common cpu functions
 */

// load 8bit value directly into register

void cpu_reg_load(uint8_t *regPtr, uint8_t val, int cycles) {
    gbe.cpu.cycles += cycles;

    *regPtr = val;
}

// load 8bit value into register from RAM  

void cpu_reg_load_direct(uint8_t *regPtr, uint16_t addr) {
    gbe.cpu.cycles += 8;
    *regPtr = memoryRead(addr); /*
    printf("x%02x\n",memoryRead(addr));
    printf("x%02x\n", gbe.ram[addr]);
    usleep(1000000);*/
}

// jump directly to 16 bit address;

void cpu_jmp(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag) {
    gbe.cpu.cycles += 12;

    uint16_t addr = read_16bit(); // the memoryRead() function is limited to 8bit at a time.
    gbe.cpu.PC += 2;

    if (!jumpOnCondition) {
        gbe.cpu.PC = addr;
        return;
    }
    if (bit_test(gbe.cpu.AF.lo, flag) == jumpConditionFlag) { // is our flag register the same as the jump condition?  if so, jump!
        gbe.cpu.PC += addr;
    }
}

// jump directly to (PC + 8bit address) used for small jumps like "if else" "statements" 

void cpu_jmp_direct(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag) { //basically only used for nearby jumps/loops
    gbe.cpu.cycles += 8;
    //printf("flag %d, requested flag state %d\n",flag,jumpConditionFlag);
    signed char target = (signed char) memoryRead(gbe.cpu.PC);
    if (!jumpOnCondition) {
        gbe.cpu.PC += target;
    } else if (bit_test(gbe.cpu.AF.lo, flag) == jumpConditionFlag) {
        gbe.cpu.PC += target;
    }
    gbe.cpu.PC++;
}

// the same as a jump but less permanent, a function call stores the PC on the stack so we can return to its position when we RET

void cpu_call(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag) {
    gbe.cpu.cycles += 12;
    uint16_t addr = read_16bit();

    gbe.cpu.PC += 2;
    if (!jumpOnCondition) { // no conditions
        stackPush(gbe.cpu.PC); // save our program counter
        gbe.cpu.PC = addr; // change program counter to the new address
    } else if (bit_test(gbe.cpu.AF.lo, flag) == jumpConditionFlag) {
        stackPush(gbe.cpu.PC);
        gbe.cpu.PC = addr;
    }
}

// return from a cpu call;

void cpu_ret(uint8_t returnOnCondition, uint8_t flag, uint8_t returnConditionFlag) {
    gbe.cpu.cycles += 8;
    if (!returnOnCondition) {
        gbe.cpu.PC = stackPop();
    } else if (bit_test(gbe.cpu.AF.lo, flag) == returnConditionFlag) {
        gbe.cpu.PC = stackPop();
    }
}

// CPU restart

void cpu_rst(uint8_t val) {
    stackPush(gbe.cpu.PC);
    gbe.cpu.cycles += 32;
    gbe.cpu.PC = val;
}
/*
 * End of common cpu functions 
 */

/*
 * bit operation related cpu functions
 */

// swap 2 nibbles of an 8bit register

void cpu_nibble_swap(uint8_t *regPtr) {
    //printf("%u\n",*regPtr);
    gbe.cpu.cycles += 8;
    *regPtr = (((*regPtr & 0xF0) >> 4) | ((*regPtr & 0x0F) << 4));
    gbe.cpu.AF.lo = 0x00; // we also need to check if the result is 0
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// swap 2 nibbles of a byte

void mem_nibble_swap(uint16_t addr) {
    gbe.cpu.cycles += 16;

    uint8_t val = memoryRead(addr);
    val = (((val & 0xF0) >> 4) | ((val & 0x0F) << 4));
    memoryWrite(addr, val);

    gbe.cpu.AF.lo = 0x00;

    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// set register bit to 0

void cpu_reset_bit(uint8_t *regPtr, int bit) {
    gbe.cpu.cycles += 8;
    *regPtr = bit_clear(*regPtr, bit); // not sure if works
}
// set a register bit to 1

void cpu_set_bit(uint8_t *regPtr, int bit) {
    gbe.cpu.cycles += 8;
    *regPtr = bit_set(*regPtr, bit);
}
// set memory bit to 0

void mem_reset_bit(uint16_t addr, int bit) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    val = bit_clear(val, bit);
    memoryWrite(addr, val);
}


// set a bit to

void mem_set_bit(uint16_t addr, int bit) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    val = bit_set(val, bit);
    memoryWrite(addr, val);

}

// check if a bit is 1

void cpu_test_bit(uint8_t *regPtr, int bit, int cycles) {
    gbe.cpu.cycles += cycles;
    if (bit_test(*regPtr, bit)) {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    } else {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);
    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
}

void mem_test_bit(uint16_t addr, int bit, int cycles) {
    gbe.cpu.cycles += cycles;
    uint8_t val = memoryRead(addr);
    if (bit_test(val, bit)) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    } else {
        gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_Z);
    }
    gbe.cpu.AF.lo = bit_clear(gbe.cpu.AF.lo, FLAG_N);
    gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_H);
}

// DAA decimal adjust accumulator. Supposed to convert binary coded decimals, No idea how it works ¯\_(ツ)_/¯, shamelessly stolen and adapted from the internet, https://www.youtube.com/watch?v=L397TWLwrUU 

void cpu_daa() {
    gbe.cpu.cycles += 4;
    if (bit_test(gbe.cpu.AF.lo, FLAG_N)) {
        if ((gbe.cpu.AF.hi & 0x0F) > 0x09 || gbe.cpu.AF.hi & 0x20) {
            gbe.cpu.AF.hi -= 0x06;
            if ((gbe.cpu.AF.hi & 0xF0) == 0xF0) {
                gbe.cpu.AF.lo |= 0x10;
            } else {
                gbe.cpu.AF.lo &= ~0x10;
            }
        }
        if ((gbe.cpu.AF.hi & 0xF0) > 0x09 || gbe.cpu.AF.lo & 0x10) {
            gbe.cpu.AF.hi -= 0x60;
        }
    } else {
        if ((gbe.cpu.AF.hi & 0x0F) > 0x09 || gbe.cpu.AF.lo & 0x20) {
            gbe.cpu.AF.hi += 0x06;
            if ((gbe.cpu.AF.hi & 0xF0) == 0) {
                gbe.cpu.AF.lo |= 0x10;
            } else {
                gbe.cpu.AF.lo &= ~0x10;
            }
        }
        if ((gbe.cpu.AF.hi & 0xF0) > 0x90 || gbe.cpu.AF.lo & 0x10) {
            gbe.cpu.AF.lo += 0x60;
        }
    }
    if (gbe.cpu.AF.hi == 0) {
        gbe.cpu.AF.lo |= 0x80;
    } else {
        gbe.cpu.AF.lo &= ~0x80;
    }
}

// rotate right (through carry)
// rrc is rr from copied code

void cpu_rr(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;

    uint8_t carry = bit_test(gbe.cpu.AF.lo, FLAG_C); // we need to load the carry flag because we need to "scroll" the byte, and the byte "pushed" out of the byte is stored in the carry flag 
    uint8_t lsb = bit_test(*regPtr, 0); // least significant bit, if its 0 we don't even need to use the carry to temporarily store 

    gbe.cpu.AF.lo = 0x00;

    *regPtr >>= 1;
    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (carry) {
        *regPtr = bit_set(*regPtr, 7);
    }

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// memory rotate right (through carry)

void mem_rr(uint16_t addr) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    uint8_t carry = bit_test(gbe.cpu.AF.lo, FLAG_C);
    uint8_t lsb = bit_test(val, 0);

    gbe.cpu.AF.lo = 0x00;

    val >>= 1;
    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (carry) {
        val = bit_set(val, 7);
    }

    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

// rotate left through carry

void cpu_rlc(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;

    uint8_t msb = bit_test(*regPtr, 7);

    gbe.cpu.AF.lo = 0x00;

    *regPtr <<= 1;
    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
        *regPtr = bit_set(*regPtr, 0);
    }
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

void mem_rlc(uint16_t addr) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    uint8_t msb = bit_test(val, 7);

    gbe.cpu.AF.lo = 0x00;

    val <<= 1;
    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
        val = bit_set(val, 7);
    }

    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

// rotate left

void cpu_rl(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;

    uint8_t msb = bit_test(*regPtr, 7); // grab the most significant bit to append it after rotation
    uint8_t carry = bit_test(gbe.cpu.AF.lo, FLAG_C);
    gbe.cpu.AF.lo = 0x00;

    *regPtr <<= 1;

    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (carry) {
        *regPtr = bit_set(*regPtr, 0);
    }

    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// rotate left (memory)

void mem_rl(uint16_t addr) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    uint8_t carry = bit_test(gbe.cpu.AF.lo, FLAG_C);
    uint8_t msb = bit_test(val, 7);

    gbe.cpu.AF.lo = 0x00;

    val <<= 1;
    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (carry) {
        val = bit_set(val, 0);
    }


    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

void cpu_rrc(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;

    uint8_t lsb = bit_test(*regPtr, 0); // grab the most significant bit to append it after rotation

    gbe.cpu.AF.lo = 0x00;

    *regPtr >>= 1;

    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
        *regPtr = bit_set(*regPtr, 7);
    }


    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

// rotate right, memory

void mem_rrc(uint16_t addr) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);

    uint8_t lsb = bit_test(val, 0);

    gbe.cpu.AF.lo = 0x00;

    val >>= 1;
    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
        val = bit_set(val, 7);
    }

    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

// shift left arithmatically (left shit makes bit 0 0 and bit 7 gets stored in the carry)

void cpu_sla(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;
    uint8_t msb = bit_test(*regPtr, 7);
    *regPtr <<= 1;
    gbe.cpu.AF.lo = 0x00;

    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

void mem_sla(uint16_t addr) {
    gbe.cpu.cycles += 16;
    uint8_t val = memoryRead(addr);
    uint8_t msb = bit_test(val, 7);
    val <<= 1;

    gbe.cpu.AF.lo = 0x00;

    if (msb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

// shift right arithmatically bit 0 into carry bit 7 is set;

void cpu_sra(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;
    uint8_t lsb = bit_test(*regPtr, 0);
    uint8_t msb = bit_test(*regPtr, 7);

    gbe.cpu.AF.lo = 0x00;
    *regPtr >>= 1;

    if (msb) {
        *regPtr = bit_set(*regPtr, 7); // force set bit 7 to 1;
    }
    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

void mem_sra(uint16_t addr) {
    gbe.cpu.cycles += 16;

    uint8_t val = memoryRead(addr);
    uint8_t lsb = bit_test(val, 0);
    uint8_t msb = bit_test(val, 7);

    gbe.cpu.AF.lo = 0x00;
    val >>= 1;

    if (msb) {
        val = bit_set(gbe.cpu.AF.lo, 7); // force set bit 7 to 1;
    }
    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}

// shift right logical register (put bit 0 in carry)

void cpu_srl(uint8_t *regPtr) {
    gbe.cpu.cycles += 8;

    uint8_t lsb = bit_test(*regPtr, 0);

    gbe.cpu.AF.lo = 0x00;

    *regPtr >>= 1;

    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (*regPtr == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
}

void mem_srl(uint16_t addr) {
    gbe.cpu.cycles += 8;
    uint8_t val = memoryRead(addr);
    uint8_t lsb = bit_test(val, 0);

    gbe.cpu.AF.lo = 0x00;

    val >>= 1;

    if (lsb) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_C);
    }
    if (val == 0) {
        gbe.cpu.AF.lo = bit_set(gbe.cpu.AF.lo, FLAG_Z);
    }
    memoryWrite(addr, val);
}
