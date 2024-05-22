#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "../headers/chip8.h"
#include "../headers/periph.h"

extern int quit;

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        printf("Usage: ./chip8 <rom_name>.ch8\n");
        return 1;
    }

    printf("Initializing CHIP-8 architecture...\n");
    init_cpu();

    char* rom_filename = argv[1];
    printf("Loading rom %s...\n", rom_filename);

    int error = load_rom(rom_filename);
    if (error) 
    {
        if (error == -1)
            printf("Return value of fread() was not equal to rom file size\n");
        else
            printf("Error while loading rom\n");
        return 1;
    }

	printf("Initializing display...\n");
    init_display();

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
