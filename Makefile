FLAGS = -I include -Wall -Wextra -pedantic -std=c99 
LIBS = -lSDL2 

sources = src/main.c src/chip8.c src/periph.c
objects = obj/main.o obj/chip8.o obj/periph.o

all: emulator

# link object files together
emulator: $(objects)
	$(CC) $(FLAGS) $(LIBS) -o $@ $(objects)

# produce object files
# $< is the first prereq file
# $@ is the target name
obj/%.o: %.c
	$(CC) -c $< $(FLAGS) $(LIBS) -o $@
