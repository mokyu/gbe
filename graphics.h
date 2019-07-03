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
 * File:   graphics.h
 * Author: emiel
 *
 * Created on February 16, 2017, 11:55 AM
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCROLL_Y 0xFF42 // background y position where drawing starts
#define SCROLL_X 0xFF43 // background x position where drawing starts
#define CURRENT_SCANLINE 0xFF44   // the current scanline to draw 
#define VIEWPORT_Y 0xFF4A // y position of the viewport start
#define VIEWPORT_X 0xFF4B // x position of the viewport start
#define LCD_CONTROL 0xFF40 // LCD control register

#define VBLANK_SCANLINE_MAX 0x99 // the last scanline before vblank grace time is over (before vblank is completed)
#define VBLANK_SCANLINE_MIN 0x90 // the scanline where vblank has been started

extern SDL_Window* SDLWindow;
extern SDL_Renderer* SDLRenderer;
extern SDL_Event* SDLEvent;
extern void initSDL(void);
extern void updateFramebuffer(int cycles);
extern void drawFramebuffer(void);
extern int windowActive(void);


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif

#endif /* GRAPHICS_H */

