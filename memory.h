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
 * File:   memory.h
 * Author: emiel
 *
 * Created on January 26, 2017, 2:27 PM
 */

#ifndef MEMORY_H
#define MEMORY_H

uint8_t memoryRead(uint16_t addr);
void memoryWrite(uint16_t addr, uint8_t val);

void enableRAMBanking(uint8_t val);
void memoryBankingHandler(uint16_t addr, uint8_t val);

void setClockspeed(void);
uint8_t getClockspeed(void);

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* MEMORY_H */

