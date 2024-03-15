FLAGS = -I include -L lib -Wall -Wextra -pedantic -std=c99 -lSDL2

sources = src/chip8.c src/periph.c src/main.c
objects = obj/chip8.o obj/periph.o obj/main.o

compile: bin/chip8

# link object files together
bin/chip8: $(objects) | bin
	$(CC) $(FLAGS) -o $@ $(objects)

bin:
	mkdir -p bin

# produce object files
# $< is the first prereq file
# $@ is the target name
obj/%.o: src/%.c
	$(CC) -c $< $(FLAGS) -o $@
