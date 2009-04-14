EXEEXT :=
OBJS := math.o

default: all

all: math$(EXEEXT) libmathparse.a

%.o: %.c
	$(CC) -c $< -o $@ -O3 -Wall

libmathparse.a: $(OBJS)
	ar rsc $@ $(OBJS)

math$(EXEEXT): $(OBJS) mathparse.h
	$(CC) $(OBJS) -o $@ -lm -O3 -Wall

clean:
	rm -f math$(EXEEXT)

