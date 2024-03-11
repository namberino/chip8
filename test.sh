if [ -f "obj/chip8.o" ]; then exit 0; else exit 1; fi
if [ -f "obj/periph.o" ]; then exit 0; else exit 1; fi
if [ -f "obj/main.o" ]; then exit 0; else exit 1; fi

if [ ./bin/chip8 -eq 1 ]; then exit 0; fi

./bin/chip8 roms/opcode-test.ch8
./bin/chip8 roms/maze.ch8
./bin/chip8 roms/particle.ch8
