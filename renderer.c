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
 * File:   renderer.c
 * Author: emiel
 *
 * Created on February 19, 2017, 2:23 PM
 */
#include "renderer.h"
#include "graphics.h"
#include "memory.h"
#include "machine.h"
void renderTiles(void);
void renderSprites(void);
COLOR getColor(uint8_t colornum, uint16_t addr);

/* Tiles are 8x8 bit sprites that are fixed in the background each line is 2 bytes for in total 16 bytes/tile 
 * a line of a tile contains 4 colors for 8 pixels represented in 2 bytes
 */
void renderTiles() {
    uint8_t status = memoryRead(0xFF40);
    if (bit_test(status, 0)) { // don't have to draw a background when its disabled
        uint16_t tileMem = 0;
        uint16_t bgmem = 0;
        uint8_t isUnsigned = 1;

        uint8_t sy = memoryRead(0xFF42); // scroll
        uint8_t sx = memoryRead(0xFF43); // scroll
        uint8_t wy = memoryRead(0xFF4a); // window y
        uint8_t wx = memoryRead(0xFF4b) - 7; // window x

        uint8_t window = 0;

        if (bit_test(status, 5)) {
            if (wy <= memoryRead(CURRENT_SCANLINE)) {
                window = 1;
            }
        } else {
            window = 0;
        }
        // where is our tile data stored?
        if (bit_test(status, 4)) {
            tileMem = 0x8000;
        } else {
            tileMem = 0x8800; // this area has tiles with negative numbering, requiring signed integers
            isUnsigned = 0;
        }

        // where is our bgmem stored
        if (window == 0) {
            if (bit_test(status, 3)) {
                bgmem = 0x9C00;
            } else {
                bgmem = 0x9800;
            }
        } else {
            if (bit_test(status, 6)) {
                bgmem = 0x9C00;
            } else {
                bgmem = 0x9800;
            }
        }
        uint8_t ypos = 0;
        if (!window) {
            ypos = sy + memoryRead(CURRENT_SCANLINE);
        } else {
            ypos = memoryRead(CURRENT_SCANLINE) - wy;
        }
        uint16_t tilerow = ((uint8_t) (ypos / 8)*32);
        for (int px = 0; px < 160; px++) {
            uint8_t xpos = px + sx;
            if (window) {
                if (px >= wx) {
                    xpos = px - wx;
                }
            }

            uint16_t tilecol = (xpos / 8);
            int16_t tilenum;
            if (isUnsigned) {
                tilenum = (uint8_t) memoryRead(bgmem + tilerow + tilecol);
            } else {
                tilenum = (int8_t) memoryRead(bgmem + tilerow + tilecol);
            }
            uint16_t tileloc = tileMem;
            if (isUnsigned) {
                tileloc += (tilenum * 16);
            } else {
                tileloc += ((tilenum + 128) * 16); // signed tiles have an 128 offset for the negative tiles;
            }
            uint8_t line = ypos % 8;
            line *= 2;
            uint8_t data1 = memoryRead(tileloc + line); // tiles consist of 2 bytes, with each pixel requiring 2 bits to represent one of 4 colors
            uint8_t data2 = memoryRead(tileloc + line + 1);
            int bitColor = xpos % 8;
            bitColor -= 7; // grab msb
            bitColor *= -1; // go to the next row

            // extract 2 required bits to see which color we need;
            int colorNum = bit_val(data2, bitColor);
            colorNum <<= 1;
            colorNum |= bit_val(data1, bitColor);

            COLOR col = getColor(colorNum, 0xFF47);
            uint8_t r, g, b;

            switch (col) { // use colors that make the screen almost look like the real lcd panel
                case WHITE:
                    r = 202;
                    g = 210;
                    b = 169;
                    break;
                case LGREY:
                    r = 143;
                    g = 150;
                    b = 117;
                    break;
                case DGREY:
                    r = 90;
                    g = 94;
                    b = 77;
                    break;
                case BLACK:
                    r = 39;
                    g = 40;
                    b = 32;
                    break;
                default:
                    printf("Invalid colors!\n");
                    exit(1);
                    break;
            }
            int scanline = memoryRead(CURRENT_SCANLINE);
            if ((scanline < 0) || (scanline > 143) || (px < 0) || (px > 159)) { // check if we are within bounds
                continue;
            }
            gbe.gpu.FrameBuffer[px][scanline][0] = r;
            gbe.gpu.FrameBuffer[px][scanline][1] = g;
            gbe.gpu.FrameBuffer[px][scanline][2] = b;
        }
    }
}

/**/
COLOR getColor(uint8_t colornum, uint16_t addr) {
    COLOR res = WHITE;
    uint8_t palette = memoryRead(addr);
    int hi = 0;
    int lo = 0;

    switch (colornum) {
        case 0:
            hi = 1;
            lo = 0;
            break;
        case 1:
            hi = 3;
            lo = 2;
            break;
        case 2:
            hi = 5;
            lo = 4;
            break;
        case 3:
            hi = 7;
            lo = 6;
            break;
        default:
            printf("failed to get color\n");
            exit(1);
    }
    int color = 0;
    color = bit_val(palette, hi) << 1;
    color |= bit_val(palette, lo);
    switch (color) {
        case 0: res = WHITE;
            break;
        case 1: res = LGREY;
            break;
        case 2: res = DGREY;
            break;
        case 3: res = BLACK;
            break;
        default:
            printf("failed to get color\n");
            exit(1);
    }
    return res;
}
// sprites are drawn over tiles and can have "empty" parts

void renderSprites() {
    uint8_t status = memoryRead(0xFF40);
    if (bit_test(status, 1)) {
        uint8_t doubleWidth = 0; // sprites can be double their width
        if (bit_test(status, 2)) {
            doubleWidth = 1;
        }
        for (int sprite = 0; sprite < 40; sprite++) {
            uint8_t index = sprite * 4;
            uint8_t ypos = memoryRead(0xFE00 + index) - 16;
            uint8_t xpos = memoryRead(0xFE00 + index + 1) - 8;
            uint8_t tileloc = memoryRead(0xFE00 + index + 2);
            uint8_t attrib = memoryRead(0xFE00 + index + 3);

            // is the sprite flipped horizontally/vetically?
            uint8_t yflip = bit_test(attrib, 6);
            uint8_t xflip = bit_test(attrib, 5);

            int scanline = memoryRead(CURRENT_SCANLINE);
            int ysize = 8;
            if (doubleWidth) {
                ysize = 16;
            }
            if ((scanline >= ypos) && (scanline < (ypos + ysize))) {
                int line = scanline - ypos;
                if (yflip) {
                    line -= ysize;
                    line *= -1;
                }
                line *= 2;
                uint8_t data1 = memoryRead((0x8000 + (tileloc * 16)) + line);
                uint8_t data2 = memoryRead((0x8000 + (tileloc * 16)) + line + 1);

                for (int tilepx = 7; tilepx >= 0; tilepx--) {
                    int colorbit = tilepx;
                    if (xflip) {
                        colorbit -= 7;
                        colorbit *= -1;
                    }
                    int colorNum = bit_val(data2, colorbit);
                    colorNum <<= 1;
                    colorNum |= bit_val(data1, colorbit);
                    COLOR col = getColor(colorNum, bit_test(attrib, 4) ? 0xFF49 : 0xFF48);
                    if (col == WHITE) {
                        continue;
                    }
                    uint8_t r, g, b;
                    switch (col) { // use colors that make the screen almost look like the real lcd panel
                        case WHITE:
                            r = 202;
                            g = 210;
                            b = 169;
                            break;
                        case LGREY:
                            r = 143;
                            g = 150;
                            b = 117;
                            break;
                        case DGREY:
                            r = 90;
                            g = 94;
                            b = 77;
                            break;
                        case BLACK:
                            r = 39;
                            g = 40;
                            b = 32;
                            break;
                        default:
                            printf("Invalid colors!\n");
                            exit(1);
                            break;
                    }
                    int xpix = 0 - tilepx;
                    xpix += 7;
                    int pixel = xpos + xpix;
                    // don't overwrite the pixel if it must be hidden
                    if (bit_test(attrib, 7)) {
                        if ((gbe.gpu.FrameBuffer[pixel][scanline][0] != 202 || (gbe.gpu.FrameBuffer[pixel][scanline][1] != 210) || (gbe.gpu.FrameBuffer[pixel][scanline][2] != 169))) {
                            continue;
                        }
                    }
                    gbe.gpu.FrameBuffer[pixel][scanline][0] = r;
                    gbe.gpu.FrameBuffer[pixel][scanline][1] = g;
                    gbe.gpu.FrameBuffer[pixel][scanline][2] = b;
                }
            }
        }
    }
}