EXES=CgLeches CgTorpes leches torpes
ROMS=leches.rom antleches.rom sin_leches.rom


all: $(EXES) $(ROMS)

clean:
	rm -rf $(EXES) $(ROMS) *.o

%:%.c
	gcc -m32 -O2 $< -o $@

%.rom: %.asm
	z88dk-z80asm -mz80 -b -o$@ $<
