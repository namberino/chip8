FLAGS  = -I include -Wall -Wextra -pedantic -std=c99 
LIBS  = -lm -lSDL2 

sources = main.c chip8.c periph.c
objects = main.o chip8.o periph.o
headers = chip8.h periph.h

all: emulator

# link object files together
emulator: $(objects) $(headers)
	$(CC) $(FLAGS) $(LIBS) -o $@ $(objects)

# produce object files
# $< is the first prereq file
# $@ is the target name
%.o: %.c
	$(CC) -c $< $(FLAGS) $(LIBS) -o $@
