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
 * File:   bitops.h
 * Author: emiel
 *
 * Created on March 1, 2017, 8:49 PM
 */

#ifndef BITOPS_H
#define BITOPS_H
#include <stdint.h>
extern uint8_t bit_set(uint8_t target, int bit);
extern uint8_t bit_clear(uint8_t target, int bit);
extern uint8_t bit_test(uint8_t target, int bit);
extern uint8_t bit_val (uint8_t target, int bit);
#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* BITOPS_H */

