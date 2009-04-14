EXEEXT :=
OBJS := math.o

default: all

all: math$(EXEEXT) libmathparse.a

%.o: %.c
	$(CC) -c $< -o $@ -O3 -Wall

libmathparse.a: $(OBJS)
	ar rsc $@ $(OBJS)

math$(EXEEXT): $(OBJS) main.c mathparse.h
	$(CC) $(OBJS) main.c -o $@ -lm -O3 -Wall

clean:
	rm -f math$(EXEEXT)

