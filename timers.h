/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timers.h
 * Author: emiel
 *
 * Created on February 18, 2017, 4:04 PM
 */

#ifndef TIMERS_H
#define TIMERS_H


/* Emulation speed */
#define MAX_CYCLES 69905        // maximum amount of cpu cycles before updatig the screen.
#define CORE_CLOCK 4194394      // standard operating mode: 4mhz

extern void updateRegisterDivider(int cycles);
extern void updateTimers(int cycles);
extern void setClockspeed(void);
extern uint8_t getClockspeed(void);
extern int clockEnabled(void);

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* TIMERS_H */

