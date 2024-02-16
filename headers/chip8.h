#ifndef CHIP8_H_
#define CHIP8_H_

extern unsigned char fontset[80];		// groups are from 0 - F (each are 5 bytes long)
extern unsigned char memory[4096];
extern unsigned char V[16];				// 8-bit registers, called Vx (x is a hex digit)
extern unsigned short I;				// 16-bit register (used for storing address)
extern unsigned short pc;				// program counter (16-bit)
extern unsigned char sp;				// stack pointer (8-bit)
extern unsigned short stack[16];		// stores address the intepreter should return to when finishing a subroutine (16-bit)
extern unsigned char keypad[16];
extern unsigned char display[64 * 32];	// 64x32 monochrome display
extern unsigned char dt;				// delay timer
extern unsigned char st;				// sound timer
extern unsigned char draw_flag;			// update display flag
extern unsigned char sound_flag;		// update sound flag

#endif
