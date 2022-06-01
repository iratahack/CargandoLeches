EXES= CgLeches CgTorpes leches torpes


all: $(EXES)

clean:
	rm -rf $(EXES)

%:%.c
	gcc -m32 -O2 $< -o $@

