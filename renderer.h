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
 * File:   renderer.h
 * Author: emiel
 *
 * Created on February 19, 2017, 2:23 PM
 */

#ifndef RENDERER_H
#define RENDERER_H

typedef enum COLOR {
    WHITE,
    LGREY,
    DGREY,
    BLACK
} COLOR;
extern void renderTiles(void);
extern void renderSprites(void);

#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* RENDERER_H */

