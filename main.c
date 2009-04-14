#include <stdio.h>

#include "mathparse.h"

int main(int argc, char **argv)
{
	int i;
	double ret;
	for (i = 0; i < 100000; i++) {
		ret = parse(argv[1]);
	}
	printf("%.5f\n", ret);
	return 0;
}

