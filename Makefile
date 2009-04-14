EXEEXT :=
OBJS := math.o
CFLAGS := -g -Wall
LDFLAGS := -g

default: all

all: math$(EXEEXT) libmathparse.a

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) -Wall

libmathparse.a: $(OBJS)
	ar rsc $@ $(OBJS)

math$(EXEEXT): $(OBJS) main.c mathparse.h
	$(CC) $(OBJS) main.c -o $@ -lm $(CFLAGS) $(LDFLAGS)

clean:
	rm -f math$(EXEEXT)

