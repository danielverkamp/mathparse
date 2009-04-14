#include <stdio.h>

#include "mathparse.h"

int main(int argc, char **argv)
{
	double ret;
	ret = parse(argv[1]);
	printf("%.5f\n", ret);
	return 0;
}

