EXES=CgLeches leches
ROMS=leches.rom Spectrum128_ROM0.rom


all: $(EXES) $(ROMS)

clean:
	rm -rf $(EXES) $(ROMS) roms/*.o *.map

%:src/%.c
	gcc -O2 $< -o $@

%.rom: roms/%.asm
	z88dk-z80asm -mz80 -m -b -o$@ $<
	./offsetCheck $(@:.rom=.map)
