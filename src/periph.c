// Handles the peripherals (outputs, inputs, etc)
#include "../headers/periph.h"
#include <SDL2/SDL.h>

SDL_Window* screen;
SDL_Renderer* renderer;
int quit = 0;

/*
Mapping keys

Layout (not original layout):
1 2 3 4
q w e r
a s d f
z x c v
*/
SDL_Scancode keymaps[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V
};

void init_display()
{
    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 8, 32 * 8, 0);
    renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
}

// draw rectangle to screen
void draw(unsigned char* display)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black

    SDL_RenderClear(renderer); // clear current rendering target with drawing color (settting all to black)

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

    // going through the 64x32 display
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            // checking if pixel at x and y position is on
            if (display[x + (y * 64)]) // display is 1D so this is accessing it as if it was 2D
            {
                SDL_Rect rect;

                // 8x8 rectangle
                rect.x = x * 8;
                rect.y = y * 8;
                rect.w = 8;
                rect.h = 8;

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer); // update the screen
}

// SDL event handler for keypress
void sdl_handler(unsigned char* keypad)
{
    SDL_Event event;

    // Check for event
    if (SDL_PollEvent(&event))
    {
        // Get snapshot of current state of the keyboard
        const Uint8* state = SDL_GetKeyboardState(NULL);

        switch (event.type)
        {
            case SDL_QUIT:
                quit = 1;
                break;

            default:
                if (state[SDL_SCANCODE_ESCAPE]) 
                    quit = 1;

                // Updating the keypad with the current state
                for (int keycode = 0; keycode < 16; keycode++)
                    keypad[keycode] = state[keymaps[keycode]];

                break;
        }
    }
}

void stop_display()
{
    SDL_DestroyWindow(screen);
    SDL_Quit();
}
