CFLAGS = -Wall -Wextra -pedantic -std=c99 `sdl2-config --cflags`
LFLAGS = -Wall -Wextra -pedantic -std=c99 `sdl2-config --libs`

sources = src/chip8.c src/periph.c src/main.c
objects = obj/chip8.o obj/periph.o obj/main.o

compile: bin/chip8

# link object files together
bin/chip8: $(objects) | bin
	$(CC) $(LFLAGS) -o $@ $(objects)

bin:
	mkdir -p bin

# produce object files
# $< is the first prereq file
# $@ is the target name
obj/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@
