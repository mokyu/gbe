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
#include "interrupt.h"
#include "graphics.h"
#include "machine.h"
#include "memory.h"

#define GAMEBOY_DPAD_UP         SDLK_UP
#define GAMEBOY_DPAD_RIGHT      SDLK_RIGHT
#define GAMEBOY_DPAD_DOWN       SDLK_DOWN
#define GAMEBOY_DPAD_LEFT       SDLK_LEFT
#define GAMEBOY_START           SDLK_RETURN
#define GAMEBOY_SELECT          SDLK_RSHIFT
#define GAMEBOY_A               SDLK_z
#define GAMEBOY_B               SDLK_x

extern gb gbe;

void requestInterrupt(uint8_t id);
void serviceInterrupt(uint8_t id);
void handleInterrupts(void);
void keyPressed(int key);
void keyReleased(int key);
uint8_t getKeyState(void);
void handleKeyboard(void);

void requestInterrupt(uint8_t id) {
    uint8_t flagset = memoryRead(0xFF0F);
    flagset = bit_set(flagset, id); 
    memoryWrite(0xFF0F, flagset);
}

void serviceInterrupt(uint8_t id) {
    // disable cpu halt because of interrupt
    gbe.cpu.halted = 0;
    // save Program Counter to stack, move Program Counter to internal interrupt handler functions
    stackPush(gbe.cpu.PC);
    switch (id) {
        case VBLANK_INTERRUPT:
            gbe.cpu.PC = 0x40;
            break;
        case LCD_INTERRUPT:
            gbe.cpu.PC = 0x48;
            break;
        case TIMER_INTERRUPT:
            gbe.cpu.PC = 0x50;
            break;
        case SERIAL_INTERRUPT: // SERIAL
            gbe.cpu.PC = 0x58;
            printf("SERIAL not supported\n");
            exit(1);
            break;
        case 4: // JOYPAD
            gbe.cpu.PC = 0x60;
            break;
        default:
            break;

    }
    gbe.cpu.ime = 0; // disable IME after servicing interrupt;
    gbe.ram[0xFF0F] = bit_clear(gbe.ram[0xFF0F], id);
}

void keyReleased(int key) {
    gbe.buttonState = (gbe.buttonState >> key) & 1;
    printf("released key %d\n", key);
}

void keyPressed(int key) {
    printf("pressed key %d\n", key);
    int wasUnset = 0;

    if (bit_test(gbe.buttonState, key) == 0) {
        wasUnset = 1;
    }
    gbe.buttonState = bit_clear(gbe.buttonState, key);
    // clear pressed key

    uint8_t btn = 0;
    if (key > 3) {
        btn = 1;
    } else {
        btn = 0;
    }
    uint8_t request = gbe.ram[0xFF00]; // completely ignore button press if it wasn't requested
    uint8_t interruptRequest = 0;
    // is it a regular button?
    // bit 4 and 5 are used to determine if we need a dpad button or a regular button, they can't be set both at the same time.
    if (btn && !bit_test(request, 5)) { // does the button match the requested one
        interruptRequest = 1;
    } else if (!btn && !bit_test(request, 4)) {
        interruptRequest = 1;
    }

    // actually request the interrupt
    if (interruptRequest && !wasUnset) {
        requestInterrupt(JOYPAD_INTERRUPT);
    }

}

uint8_t getKeyState() {
    uint8_t state = gbe.ram[0xFF00];
    // gotta flip em all for cleaner comparisons
    state ^= 0xFF;
    // buttons share the same space so we need to check if its a regular button or the dpad by checking a control bit
    // normal buttons?
    if (!bit_test(state, 4)) {
        uint8_t hiJoypad = gbe.buttonState >> 4; // move to lower nibble so we can OR it into state
        hiJoypad |= 0xF0; // switch top 4 bits to on position
        state &= hiJoypad; // perform bitwise and to find out which buttons are pressed
    } else if (!bit_test(state, 5)) { // joypad buttons
        uint8_t loJoypad = gbe.buttonState;
        loJoypad |= 0xF0;
        state &= loJoypad;
    }
    return state;
}

void handleInterrupts() {
    /* Check if the interrupt master enable is set.
     * Follow up by checking if any bits are set in the request register.
     * When an interrupt is requested we check if the interrupt is enabled.
     * When all these conditions apply we service the interrupt */
    if (gbe.cpu.ime) { // check interrupt master enable
        uint8_t request = gbe.ram[IF];
        uint8_t enabled = gbe.ram[IE];
        if (request > 0) {
            for (uint8_t i = 0; i < 5; i++) {
                if (bit_test(request, i)) {
                    if (bit_test(enabled, i)) {
                        serviceInterrupt(i);
                    }
                }
            }
        }
    }
}

void handleKeyboard() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case GAMEBOY_DPAD_UP:
                        keyPressed(2);
                        break;
                    case GAMEBOY_DPAD_RIGHT:
                        keyPressed(0);
                        break;
                    case GAMEBOY_DPAD_DOWN:
                        keyPressed(3);
                        break;
                    case GAMEBOY_DPAD_LEFT:
                        keyPressed(1);
                        break;
                    case GAMEBOY_START:
                        keyPressed(7);
                        break;
                    case GAMEBOY_SELECT:
                        keyPressed(6);
                        break;
                    case GAMEBOY_A:
                        keyPressed(4);
                        break;
                    case GAMEBOY_B:
                        keyPressed(5);
                        break;
                    case SDLK_ESCAPE:
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case GAMEBOY_DPAD_UP:
                        keyReleased(2);
                        break;
                    case GAMEBOY_DPAD_RIGHT:
                        keyReleased(0);
                        break;
                    case GAMEBOY_DPAD_DOWN:
                        keyReleased(3);
                        break;
                    case GAMEBOY_DPAD_LEFT:
                        keyReleased(1);
                        break;
                    case GAMEBOY_START:
                        keyReleased(7);
                        break;
                    case GAMEBOY_SELECT:
                        keyReleased(6);
                        break;
                    case GAMEBOY_A:
                        keyReleased(4);
                        break;
                    case GAMEBOY_B:
                        keyReleased(5);
                        break;
                    case SDLK_ESCAPE:
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                exit(1);
                break;
            default:
                break;
        }
    }
}