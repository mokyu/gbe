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

#include "graphics.h"
#include "renderer.h"
#include "logger.h"
#include "machine.h"
#include "interrupt.h"
#include "memory.h"

SDL_Window* SDLWindow;
SDL_Renderer* SDLRenderer;

void initSDL(void);
void updateFramebuffer(int cycles);
void drawFramebuffer(void);
void drawFrame(void);

void drawScanLine(void);

int windowActive(void);

void setLCDStatus(void);
int LCDEnabled(void);

void initSDL() {
    logger(LOG_INFO, "Initializing SDL");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        logger(LOG_CRIT, "Failed to initialize SDL2");
        logger(LOG_CRIT, SDL_GetError());
        exit(1);
    }
    int height = 288;
    int width = 320;
    logger(LOG_INFO, "Building window.");
    SDLWindow = SDL_CreateWindow("GBE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 0, 255);

    SDL_RenderPresent(SDLRenderer);
    logger(LOG_INFO, "Succesfully initialized SDL2.");
}

void updateFramebuffer(int cycles) {
    setLCDStatus();
    if (LCDEnabled()) {
        gbe.gpu.scanlineCounter -= cycles;
    }
    // next scanline

    uint8_t current = memoryRead(CURRENT_SCANLINE);

    // v-blank period reached.
    if (current > VBLANK_SCANLINE_MAX) {
        gbe.ram[CURRENT_SCANLINE] = 0;
    }
    if (gbe.gpu.scanlineCounter <= 0) {
        drawScanLine();
    }
    //gbe.ram[CURRENT_SCANLINE]++;
}

// return bit 7 of the control register

int LCDEnabled() {
    return bit_test(memoryRead(LCD_CONTROL), 7);
}

/*
 * LCD control register (LCD_CONTROL)
 * 7 LCD enabled
 * 6 Window Tile Map Display Select (region where target tile is located for drawing on the viewport)
 * 5 Window Display Enable (set to 0 when not enabled to disable drawing of the viewport region)
 * 4 BG & Window Tile Data Select ( set when tile position is signed or unsigned )
 * 3 BG Tile Map Display Select ( sane as 6 but for the background "canvas" )
 * 2 OBJ (Sprite) Size (sprite says, can be 8x16 instead of 8x8 like the background tiles)
 * 1 OBJ (Sprite) Display Enable (Does the same as bit 5 but for sprites instead of the whole viewport)
 * 0 BG Display (Same as bit 1 but for tiles instead)
 * 
 * LCD status register (0xFF41)
 * 6 LYC=LY coincidence interrupt (set when CURRENT_SCANLINE equals 0xFF45)
 * 5 Mode 2 OAM Interrupt
 * 4 Mode 1 V-blank interrupt
 * 3 Mode 0 H-blank interrupt
 * 
 * -- mode flags --
 * 
 * 00: H-Blank (mode = 0)
 * 01: V-Blank (mode = 1)
 * 10: Searching Sprites Atts  (mode=2)
 * 11: Transfering Data to LCD Driver (mode=3)
 */
void setLCDStatus() {
    uint8_t status = memoryRead(0xFF41);
    if (!LCDEnabled()) {
        // set LCD mode to 1 and reset scanlines
        gbe.gpu.scanlineCounter = 456;
        gbe.ram[CURRENT_SCANLINE] = 0;
        status &= 252; // mask 2 lsb
        status = bit_set(status, 0); // invert (~) & AND, set VBLANK (clear bit 0)
        memoryWrite(0xFF41, status);
        return;
    }
    uint8_t ly = memoryRead(CURRENT_SCANLINE);
    uint8_t currentMode = status & 0x3; // last two bits
    uint8_t mode = 0;
    uint8_t intReq = 0;

    //are we vblanking (line 144+)? (set vblank mode)
    if (ly >= VBLANK_SCANLINE_MIN) {
        mode = 1; // set vblank enable
        status = bit_set(status, 0); // set bit 0 to 1
        status = bit_clear(status, 1); // set bit 1 to 0
        intReq = bit_test(status, 4); // interrupt requested?
    } else {
        int mode2boundary = (456 - 80); // mode 2 requires 80 CPU cycles
        int mode3boundary = (mode2boundary - 172); // Mode 3 takes another 172 CPU cycles

        if (gbe.gpu.scanlineCounter >= mode2boundary) {
            mode = 2;
            status = bit_set(status, 1);
            status = bit_clear(status, 0);
            intReq = bit_test(status, 5);
        } else if (gbe.gpu.scanlineCounter >= mode3boundary) {
            mode = 3;
            // set mode to 11 (LCD transfer)
            status = bit_set(status, 1);
            status = bit_set(status, 0);
        } else {
            mode = 0;
            // set to 00
            status = bit_clear(status, 1);
            status = bit_clear(status, 0);
            intReq = bit_test(status, 3);
        }
    }
    if (intReq && (mode != currentMode)) {
        requestInterrupt(LCD_INTERRUPT);
    }
    // special coincidence register
    if (ly == memoryRead(0xFF45)) {
        status = bit_set(status, 2);
        if (bit_test(status, 6)) {
            requestInterrupt(LCD_INTERRUPT);
        }
    } else {
        status = bit_clear(status, 2);
    }
    memoryWrite(0xFF41, status);
}

//see LCD control register above setLCDStatus()

void drawScanLine(void) {
    uint8_t status = memoryRead(0xFF40);
    if (bit_test(status, 7) == 0) {
        return; // don't draw when display is disabled
    }
    gbe.ram[CURRENT_SCANLINE]++;
    gbe.gpu.scanlineCounter = 456; // reset for retracing
    if (gbe.ram[CURRENT_SCANLINE] == VBLANK_SCANLINE_MIN) {
        requestInterrupt(VBLANK_INTERRUPT);
    }
    if (gbe.ram[CURRENT_SCANLINE] > VBLANK_SCANLINE_MAX) {
        gbe.ram[CURRENT_SCANLINE] = 0;
    }
    if (gbe.ram[CURRENT_SCANLINE] < VBLANK_SCANLINE_MIN) { // only render outside vblank!
        if (bit_test(status, 7)) { // we are good to go!
            renderTiles();
            renderSprites();
        }
    }

}

void drawFramebuffer() {
    drawFrame(); /*
    

    
    SDL_RenderPresent(SDLRenderer);*/
}

void drawFrame() { // draws the game boy thingy in the background
    //SDL_Rect rect = {163, 85, 320, 288};
    //SDL_SetRenderDrawColor(SDLRenderer, 255, 255, 255, 255);

    // starting coordinates
    int coordy = 0;
    int coordx = 0;
    for (int ypix = 0; ypix < 144; ypix++) { // scanline
        for (int xpix = 0; xpix < 160; xpix++) { // draw scanline
            SDL_SetRenderDrawColor(SDLRenderer, gbe.gpu.FrameBuffer[xpix][ypix][0], gbe.gpu.FrameBuffer[xpix][ypix][1], gbe.gpu.FrameBuffer[xpix][ypix][2], 255); // set the color of the target pixel
            //SDL_SetRenderDrawColor(SDLRenderer,ypix ,xpix ,123 , 255);
            const SDL_Point pix[4] = {// create 4 pixels
                {coordx, coordy}, // top left
                {coordx, coordy + 1}, // top right
                {coordx + 1, coordy + 1}, // bottom right
                {coordx + 1, coordy}
            }; // bottom left
            SDL_RenderDrawPoints(SDLRenderer, pix, 4);

            coordx += 2; // increment by 2
        }
        coordx = 0;
        coordy += 2;
    }

    //SDL_RenderFillRect(SDLRenderer, &rect);
    SDL_RenderPresent(SDLRenderer);
}

int windowActive() {
    SDL_Event event;
    SDL_PollEvent(&event); // have to poll for events before being able to determine screen status.
    uint32_t active = SDL_GetWindowFlags(SDLWindow);
    if (active & SDL_WINDOW_INPUT_FOCUS) {
        return 1;
    } else {
        return 0;

    }

}