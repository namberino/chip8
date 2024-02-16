#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "chip8.h"
#include "periph.h"

extern int quit;

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        perror("Usage: ./emulator <rom_name>.ch8");
        return 1;
    }

    puts("Initializing CHIP-8 architecture...");
    init_cpu();
    puts("Done");

    char* rom_filename = argv[1];
    printf("Loading rom %s...\n", rom_filename);

    int error = load_rom(rom_filename);
    if(error) 
    {
        if (error == -1)
            perror("Return value of fread() was not equal to rom file size");
        else
            perror("Error while loading rom");
        return 1;
    }

    puts("Rom loading successful");

    init_display();
    puts("Display initialization successful");

    while (!quit)
    {
        emulate_cycle();
        sdl_handler(keypad);

        if (draw_flag)
            draw(display);

        // Delay to emulate chip-8's clock speed
        usleep(1500);
    }

    stop_display();
    
    return 0;
}
