/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lr35902.h
 * Author: emiel
 *
 * Created on February 25, 2017, 11:40 PM
 */

#ifndef LR35902_H
#define LR35902_H

#include <stdint.h>
extern uint16_t read_16bit(void);
extern void cpu_8bit_load(uint8_t *regPtr);
extern void cpu_8bit_add(uint8_t *regPtr, uint8_t val, int cycles, int direct, int carry);
extern void cpu_8bit_adc(uint8_t val);
extern void cpu_8bit_sub(uint8_t *regPtr, uint8_t val, int cycles, int direct, int carry);
extern void cpu_8bit_and(uint8_t *regPtr, uint8_t val, int cycles, int direct);
extern void cpu_8bit_or(uint8_t *regPtr, uint8_t val, int cycles, int direct);
extern void cpu_8bit_xor(uint8_t *regPtr, uint8_t val, int cycles, int direct);
extern void cpu_8bit_inc(uint8_t *regPtr, int cycles);
extern void cpu_8bit_dec(uint8_t *regPtr, int cycles);

extern void mem_8bit_inc(uint16_t addr, int cycles);
extern void mem_8bit_dec(uint16_t addr, int cycles);

extern void cpu_16bit_load(uint16_t *regPtr);
extern void cpu_16bit_add(uint16_t *regPtr, uint16_t val, int cycles);
extern void cpu_16bit_inc(uint16_t *regPtr, int cycles);
extern void cpu_16bit_dec(uint16_t *regPtr, int cycles);

extern void cpu_reg_load(uint8_t *regPtr, uint8_t val, int cycles);
extern void cpu_reg_load_direct(uint8_t *regPtr, uint16_t addr);

extern void cpu_jmp(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);
extern void cpu_jmp_direct(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);

extern void cpu_call(uint8_t jumpOnCondition, uint8_t flag, uint8_t jumpConditionFlag);
extern void cpu_ret(uint8_t returnOnCondition, uint8_t flag, uint8_t returnConditionFlag);
extern void cpu_rst(uint8_t val);

extern void cpu_nibble_swap(uint8_t *regPtr);
extern void mem_nibble_swap(uint16_t addr);
extern void cpu_reset_bit(uint8_t *regPtr, int bit);
extern void mem_reset_bit(uint16_t addr, int bit);
extern void cpu_set_bit(uint8_t *regPtr, int bit);
extern void mem_set_bit(uint16_t addr, int bit);
extern void cpu_test_bit(uint8_t *regPtr, int bit, int cycles);
extern void mem_test_bit(uint16_t addr, int bit, int cycles);
extern void cpu_daa(void);

void cpu_8bit_cmp(uint8_t reg, uint8_t val, int cycles, int direct);

extern void cpu_rrc(uint8_t *regPtr);
extern void mem_rrc(uint16_t addr);
extern void cpu_rlc(uint8_t *regPtr);
extern void mem_rlc(uint16_t addr);
extern void cpu_rl(uint8_t *regPtr);
extern void mem_rl(uint16_t addr);
extern void cpu_rr(uint8_t *regPtr);
extern void mem_rr(uint16_t addr);
extern void cpu_sla(uint8_t *regPtr);
extern void mem_sla(uint16_t addr);
extern void cpu_sra(uint8_t *regPtr);
extern void mem_sra(uint16_t addr);
extern void cpu_srl(uint8_t *regPtr);
extern void mem_srl(uint16_t addr);
// flag bits
#define FLAG_Z 7
#define FLAG_N 6
#define FLAG_H 5
#define FLAG_C 4
// for if we need toclear the flags except a specific flag
#define FLAG_MASK_Z 128
#define FLAG_MASK_N 64
#define FLAG_MASK_H 32
#define FLAG_MASK_C 16


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* LR35902_H */

