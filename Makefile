EXES=CgLeches CgTorpes leches torpes
ROMS=leches.rom antleches.rom sin_leches.rom Spectrum128_ROM0.rom


all: $(EXES) $(ROMS)

clean:
	rm -rf $(EXES) $(ROMS) roms/*.o *.map

%:src/%.c
	gcc -m32 -O2 $< -o $@

%.rom: roms/%.asm
	z88dk-z80asm -mz80 -m -b -o$@ $<
