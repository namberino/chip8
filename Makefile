FLAGS = -I include -Wall -Wextra -pedantic -std=c99 -lSDL2

sources = src/main.c src/chip8.c src/periph.c
objects = obj/main.o obj/chip8.o obj/periph.o

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
