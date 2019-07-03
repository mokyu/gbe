# GBE - Gameboy Emulator written in C with SDL2 and ncurses

![picture](https://github.com/mokyu/gbe/blob/master/img/shot.png?raw=true)

Original Gameboy emulator written in C for Linux systems. Originally started as a fun way to familiarize myself with CPU's, assembly, SDL and ncurses in general years ago, unfortunately i've never gotten around to finishing it.



## Features
• Most interrupts implemented (vsync/hsync etc)

• All opcodes implemented 

• Uses SDL2 for rendering

• Runs the gb bios rom and some of Blarggs GB CPU tests flawlessy

• Supports basic memory management 32KB roms (no memory banking)

• Comes with a debugger using `ncurses`

## usage

build with `sdl2` and `ncurses` dev packages on your run of the mill GNU/Linux installation on a __little endian__ machine

place `bios.gb` and `rom.gb` in the same directory. before running `gbe`.
