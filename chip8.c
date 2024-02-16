// Architecture and logic
#include "headers/chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>


extern int errno;


// Architecture

/*
Memory is 4K (4096 bytes), opcode is big-endian
0x000 - 0x1FF: Interpreter
0x200 - 0xfff: Program and data
*/

// Font set (from Cowgod's chip-8 technical reference)
unsigned char fontset[80] = {
    0xF0, 0X90, 0X90, 0X90, 0XF0, // 0
    0X20, 0X60, 0X20, 0X20, 0X70, // 1
    0XF0, 0X10, 0XF0, 0X80, 0XF0, // 2
    0XF0, 0X10, 0XF0, 0X10, 0XF0, // 3
    0X90, 0X90, 0XF0, 0X10, 0X10, // 4
    0XF0, 0X80, 0XF0, 0X10, 0XF0, // 5
    0XF0, 0X80, 0XF0, 0X90, 0XF0, // 6
    0XF0, 0X10, 0X20, 0X40, 0X40, // 7
    0XF0, 0X90, 0XF0, 0X90, 0XF0, // 8
    0XF0, 0X90, 0XF0, 0X10, 0XF0, // 9
    0XF0, 0X90, 0XF0, 0X90, 0X90, // A
    0XE0, 0X90, 0XE0, 0X90, 0XE0, // B
    0XF0, 0X80, 0X80, 0X80, 0XF0, // C
    0XE0, 0X90, 0X90, 0X90, 0XE0, // D
    0XF0, 0X80, 0XF0, 0X80, 0XF0, // E
    0XF0, 0X80, 0XF0, 0X80, 0X80  // F
};

unsigned char memory[4096] = {0};       // Memory (4K)
unsigned char V[16] = {0};              // Register (16 general purpose 8 bit registers)
unsigned short I = 0;                   // Memory address register (16 bits) 
unsigned short pc = 0x200;              // Program counter (16 bits pseudo register) stores currently executing address
unsigned char sp = 0;                   // Stack pointer points to top most level of stack (8 bits)
unsigned short stack[16] = {0};         // Stack (16 bits each)
unsigned char keypad[16] = {0};         // Keypad
unsigned char display[64 * 32] = {0};   // Display (64x32 monochrome)
unsigned char dt = 0;                   // Delay timer
unsigned char st = 0;                   // Sound timer
unsigned char draw_flag = 0;            // Update display flag
unsigned char sound_flag = 0;           // Play sound flag


// Logic

// Initialize cpu (loading fontset into mem)
void init_cpu(void)
{
    srand((unsigned int)time(NULL));

    // Load font into mem
    memcpy(memory, fontset, sizeof(fontset));
}

// Load rom into mem
int load_rom(char* filename)
{
    FILE* fp = fopen(filename, "rb");

    if (fp == NULL) 
        return errno;

    // Retrieving file size
    struct stat st;
    stat(filename, &st);
    size_t fsize = st.st_size;

    size_t bytes_read = fread(memory + 0x200, 1, sizeof(memory) - 0x200, fp);

    fclose(fp);

    if (bytes_read != fsize)
        return -1;

    return 0;
}

// Running chip8 instructions
/*
1. Fetch opcode
  - Fetch opcode from mem at location specified by pc
  - Each array address contains 1 byte, opcode is 2 bytes long
  - We need to fetch 2 consecutive bytes and merge them

  - Example:
    - 0xAAFF means pc = 0xAA, pc + 1 = 0xFF
    - opcode = pc << 8 | pc + 1
    - Shift AA by 8 bits (0xAA00)
    - Bitwise OR to merge with pc + 1

2. Decode opcode
  - Look up the optable

  - Example: 0xAAFF means set I to address 0xAFF

3. Execute opcode
  - Execute parsed opcode
  - pc += 2 (every instruction is 2 bytes long)

4. Update timers
  - Count to 0 at 60Hz if they are set to a number > 0

Note: Vf is carry flag (no borrow flag if in subtraction)
*/
void emulate_cycle(void)
{
    draw_flag = 0;
    sound_flag = 0;
    
    unsigned short op = memory[pc] << 8 | memory[pc + 1];

    // The shifting puts X and Y in the opcode at the end (example: 0800 -> 0008, 0080 -> 0008)
    unsigned short x = (op & 0x0F00) >> 8; // Vx register (getting the X that is in some instructions such as CXNN)
    unsigned short y = (op & 0x00F0) >> 4; // Vy register (getting the Y that is in some instructions such as DXYN)

    switch (op & 0xF000)
    {
        case 0x0000:
            switch (op & 0x00FF)
            {
                case 0x00E0: // Clear screen
                    printf("[SUCCESS] 0x%x: 0x00E0\n", op);

                    for (int i = 0; i < 64 * 32; i++)
                    {
                        display[i] = 0;
                    }
                    
                    pc += 2;

                    break;
                
                case 0x00EE: // Return from subroutine
                    printf("[SUCCESS] 0x%x: 0x00EE\n", op);

                    pc = stack[sp];
                    sp--;
                    pc += 2;

                    break;
            
                default:
                    printf("[ERROR] Unknown opcode: 0x%x\n", op);
                    break;
            }
            break;

        case 0x1000: // Jumps to address NNN
            printf("[SUCCESS] 0x%x: 0x1NNN\n", op);
            
            pc = op & 0x0FFF;
            
            break;

        case 0x2000: // Calls subroutine at NNN
            printf("[SUCCESS] 0x%x: 0x2NNN\n", op);

            // Storing current address to stack 
            sp += 1;
            stack[sp] = pc;
            pc = op & 0x0FFF; // Getting NNN

            break;

        case 0x3000: // Skips next instruction if Vx == NN
            printf("[SUCCESS] 0x%x: 0x3XNN\n", op);

            if (V[x] == (op & 0x00FF))
                pc += 2;
            pc += 2;

            break;

        case 0x4000: // Skips next instruction if Vx != NN
            printf("[SUCCESS] 0x%x: 0x4XNN\n", op);
            
            if (V[x] != (op & 0x00FF))
                pc += 2;
            pc += 2;

            break;

        case 0x5000: // Skips next instruction if Vx == Vy
            printf("[SUCCESS] 0x%x: 0x5XY0\n", op);

            if (V[x] == V[y])
                pc += 2;
            pc += 2;

            break;

        case 0x6000: // Sets Vx to NN
            printf("[SUCCESS] 0x%x: 0x6XNN\n", op);

            V[x] = op & 0x00FF;
            pc += 2;

            break;

        case 0x7000: // Adds NN to Vx (carry flag remains unchanged)
            printf("[SUCCESS] 0x%x: 0x7XNN\n", op);

            V[x] += op & 0x00FF;
            pc += 2;

            break;

        case 0x8000: // Multiple instructions
            switch (op & 0x000F)
            {
                case 0x0000: // 8XY0: Sets Vx to value of Vy
                    printf("[SUCCESS] 0x%x: 0x8XY0\n", op);

                    V[x] = V[y];
                    pc += 2;

                    break;

                case 0x0001: // 8XY1: Sets Vx to Vx | Vy
                    printf("[SUCCESS] 0x%x: 0x8XY1\n", op);

                    V[x] = V[x] | V[y];
                    pc += 2;

                    break;

                case 0x0002: // 8XY2: Sets Vx to Vx & Vy
                    printf("[SUCCESS] 0x%x: 0x8XY2\n", op);

                    V[x] = V[x] & V[y];
                    pc += 2;
                    
                    break;

                case 0x0003: // 8XY3: Sets Vx to Vx ^ Vy
                    printf("[SUCCESS] 0x%x: 0x8XY3\n", op);

                    V[x] = V[x] ^ V[y];
                    pc += 2;
                    
                    break;

                case 0x0004: // 8XY4: Adds Vy to Vx (if overflow, Vf = 1)
                    printf("[SUCCESS] 0x%x: 0x8XY4\n", op);

                    V[0xF] = (V[x] + V[y] > 0xFF) ? 1 : 0;
                    V[x] += V[y];

                    pc += 2;

                    break;

                case 0x0005: // 8XY5: Subtracts Vy from Vx (if Vx > Vy, Vf = 1)
                    printf("[SUCCESS] 0x%x: 0x8XY5\n", op);

                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];

                    pc += 2;

                    break;

                case 0x0006: // 0XY6: Shift right Vx by 1 (Vf = LSB of Vx)
                    printf("[SUCCESS] 0x%x: 0x8XY6\n", op);

                    V[0xF] = V[x] & 0x1;
                    V[x] = V[x] >> 1;

                    pc += 2;

                    break;

                case 0x0007: // 8XY7: Vx = Vy - Vx (if Vy > Vx, Vf = 1)
                    printf("[SUCCESS] 0x%x: 0x8XY7\n", op);

                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];

                    pc += 2;
                    
                    break;

                case 0x000E: // 8XYE: Shift left Vx by 1 (Vf = MSB of Vx)
                    printf("[SUCCESS] 0x%x: 0x8XYE\n", op);

                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] = V[x] << 1;

                    pc += 2;

                    break;
                
                default:
                    break;
            }
            break;
        
        case 0x9000: // 9XY0: Skips next intruction if Vx != Vy
            printf("[SUCCESS] 0x%x: 0x9XY0\n", op);

            if (V[x] != V[y])
                pc += 2;
            pc += 2;
            
            break;

        case 0xA000: // ANNN: Sets I to address NNN
            printf("[SUCCESS] 0x%x: 0xANNN\n", op);

            I = op & 0x0FFF;
            pc += 2;
            
            break;

        case 0xB000: // BNNN: Jumps to address NNN + V0
            printf("[SUCCESS] 0x%x: 0xBNNN\n", op);

            pc = (op & 0x0FFF) + V[0];
            
            break;

        case 0xC000: // CXNN: Sets Vx to the result of a bitwise AND op on a random number (0 - 255) and NN
            printf("[SUCCESS] 0x%x: 0xCXNN\n", op);

            V[x] = (rand() % 256) & (op & 0x00FF);

            pc += 2;

            break;

        /*
        DXYN: Draws a sprite at coordinate (Vx, Vy)
        with a width of 8px and height of Npx (8x(N+1)).
        Each row of 8px is read as bit-coded starting from mem location I
        I value doesn't change after the execution of this instruction
        Vf = 1 if any screen pixels are flipped from set to unset when sprite is drawn
        */
        case 0xD000:
            printf("[SUCCESS] 0x%x: 0xDXYN\n", op);

            draw_flag = 1;

            unsigned short height = op & 0x000F;
            unsigned short px;

            V[0xF] = 0; // Set collision flag to 0

            // Loop each rows
            for (int yline = 0; yline < height; yline++)
            {
                px = memory[I + yline]; // fetch the pixel value from the memory starting at location I

                // Loop 8 bits of 1 row
                for (int xline = 0; xline < 8; xline++)
                {
                    // Check if current evaluated pixel is set to 1 (0x80 >> xline scans through the byte, one bit at the time)
                    if ((px & (0x80 >> xline)) != 0)
                    {
                        // If drawing causes any pixel to be erased set the collision flag to 1
                        if (display[(V[x] + xline + ((V[y] + yline) * 64))] == 1)
                            V[0xF] = 1;

                        // Set pixel value using XOR
                        display[V[x] + xline + ((V[y] + yline) * 64)] ^= 1;
                    }
                }
            }
            pc += 2;

            break;

        case 0xE000:
            switch (op & 0x00FF)
            {
                case 0x009E: // Skips next instruction if key stored in Vx is pressed
                    printf("[SUCCESS] 0x%x: 0xEX9E\n", op);

                    if (keypad[V[x]])
                        pc += 2;
                    pc += 2;

                    break;

                case 0x00A1: // Skips next instruction if key stored in Vx is not pressed
                    printf("[SUCCESS] 0x%x: 0xEXA1\n", op);

                    if (!keypad[V[x]])
                        pc += 2;
                    pc += 2;
                    
                    break;

                default:
                    printf("[ERROR] Unknown opcode: 0x%x\n", op);
                    break;
            }
            break;

        case 0xF000:
            switch (op & 0x00FF)
            {
                case 0x0007: // Sets Vx to value of delay timer
                    printf("[SUCCESS] 0x%x: 0xFX07\n", op);

                    V[x] = dt;
                    pc += 2;
                    
                    break;

                case 0x000A: // A key press is awaited and then stored in Vx (block operation)
                    printf("[SUCCESS] 0x%x: 0xFX0A\n", op);

                    for (int i = 0; i < 16; i++)
                    {
                        if (keypad[i])
                        {
                            V[x] = i;
                            pc += 2;
                            break;
                        }
                    }

                    break;

                case 0x0015: // Sets delay timer to Vx
                    printf("[SUCCESS] 0x%x: 0xFX15\n", op);

                    dt = V[x];
                    pc += 2;
                    
                    break;

                case 0x0018: // Sets sound timer to Vx
                    printf("[SUCCESS] 0x%x: 0xFX18\n", op);
                    
                    st = V[x];
                    pc += 2;
                    
                    break;

                case 0x001E: // Adds Vx to I (Vf unaffected)
                    printf("[SUCCESS] 0x%x: 0xFX1E\n", op);
                    
                    I += V[x];
                    pc += 2;
                    
                    break;

                case 0x0029: // Sets I to location of the sprite for the character in Vx (Character 0-F are represented by a 4x5 font)
                    printf("[SUCCESS] 0x%x: 0xFX29\n", op);
                    
                    // Each digit is 5 bytes long
                    I = V[x] * 5;
                    pc += 2;
                    
                    break;

                // Stores BCD of Vx
                // The hundreds digit in mem at location I 
                // The tens digit in mem at location I + 1
                // The ones digit in mem at location I + 2
                case 0x0033: 
                    printf("[SUCCESS] 0x%x: 0xFX33\n", op);

                    memory[I] = (V[x] % 1000) / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = (V[x] % 10);

                    pc += 2;

                    break;

                case 0x0055: // Stores V0 to Vx in mem, starting at address I (offset from I is incremented but I is unmodified)
                    printf("[SUCCESS] 0x%x: 0xFX55\n", op);

                    for (int i = 0; i <= x; i++)
                        memory[I + i] = V[i];
                    pc += 2;

                    break;

                case 0x0065: // Fills V0 to Vx with values from mem, starting at address I (offset from I is incremented but I is unmodified)
                    printf("[SUCCESS] 0X%x: 0xFX65\n", op);

                    for (int i = 0; i <= x; i++)
                        V[i] = memory[I + i];
                    pc += 2;

                    break;

                default:
                    printf("[ERROR] Unknown opcode: 0x%x\n", op);
                    break;
            }
            break;

        default:
            printf("[ERROR] Unknown opcode: 0x%x\n", op);
            break;
    }

    // Decrement timers if they are > 0
    if (dt > 0)
        dt -= 1;

    if (st > 0)
    {
        sound_flag = 1;
        puts("Beep");
        st -= 1;
    }
}
