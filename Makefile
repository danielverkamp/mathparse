EXEEXT :=

default: all

all: math$(EXEEXT)

math$(EXEEXT): math.c mathparse.h
	$(CC) $< -o $@ -lm -O3 -Wall

clean:
	rm -f math$(EXEEXT)

