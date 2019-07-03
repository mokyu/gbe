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
 * File:   interrupt.h
 * Author: emiel
 *
 * Created on February 17, 2017, 1:12 PM
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <SDL2/SDL.h>

#define VBLANK_INTERRUPT    0
#define LCD_INTERRUPT       1
#define TIMER_INTERRUPT     2
#define SERIAL_INTERRUPT    3
#define JOYPAD_INTERRUPT    4

extern void requestInterrupt(uint8_t id);
extern void handleInterrupts(void);
extern uint8_t getKeyState(void);
extern void handleKeyboard(void);
#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* INTERRUPT_H */

