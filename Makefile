FLAGS  = -I include -Wall -Wextra -pedantic -std=c99 
LIBS  = -lm -lSDL2 

sources = main.c chip8.c periph.c
objects = obj/main.o obj/chip8.o obj/periph.o
headers = headers/chip8.h headers/periph.h

all: emulator

# link object files together
emulator: $(objects) $(headers)
	$(CC) $(FLAGS) $(LIBS) -o $@ $(objects)

# produce object files
# $< is the first prereq file
# $@ is the target name
obj/%.o: %.c
	$(CC) -c $< $(FLAGS) $(LIBS) -o $@
