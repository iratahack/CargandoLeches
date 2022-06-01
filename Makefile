EXES= CgLeches CgTorpes leches torpes


all: $(EXES)

clean:
	rm -rf $(EXES)

%:%.c
	gcc -O2 $< -o $@

