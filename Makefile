EXES=CgLeches leches
ROMS=leches.rom Spectrum128_ROM0.rom gw03.rom

ifeq ($(OS),Windows_NT)
	EXT=.exe
endif

all: $(EXES) $(ROMS)

clean:
	rm -rf $(EXES) $(ROMS) roms/*.o *.map

%:src/%.c
	gcc -Wall -O2 $< -o $@
	strip $@$(EXT)

%.rom: roms/%.asm
	z88dk-z80asm -mz80 -m -b -o$@ $<
	./offsetCheck $(@:.rom=.map)
