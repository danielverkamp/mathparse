EXEEXT :=

default: all

all: math$(EXEEXT)

math$(EXEEXT): math.c
	$(CC) $< -o $@ -lm -O3

clean:
	rm -f math$(EXEEXT)

