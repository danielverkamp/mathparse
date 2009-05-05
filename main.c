#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mathparse.h"

int main(int argc, char **argv)
{
	double ret;
	srand(time(0));
	ret = parse(argv[1]);
	printf("%f\n", ret);
	return 0;
}

