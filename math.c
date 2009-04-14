#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
syntax:

expr -> term + term
expr -> term - term
expr -> term

term -> factor * factor
term -> factor / factor
term -> factor

factor -> power ^ power
factor -> power

*/

enum { T_NONE, T_EOF, T_PLUS, T_MINUS, T_MUL, T_DIV, T_LPAREN, T_RPAREN, T_NUM, T_POW };

typedef struct {
	int type;
	double val;
} tok;

tok t;

void err()
{
	fprintf(stderr, "parse error\n");
	exit(1);
}

tok get_number(char **s)
{
	char *end;
	tok ret;

	ret.type = T_NONE;
	ret.val = strtod(*s, &end);

	if (end != *s) {
		ret.type = T_NUM;
		*s = end;
	}

	return ret;
}

tok get_token(char **s)
{
	char c;
	tok t;

	t.type = T_NONE;

	do {
		t = get_number(s);

		if (t.type == T_NONE) {
			c = **s;
			(*s)++;

			switch (c) {
			case 0: t.type = T_EOF; break;
			case '+': t.type = T_PLUS; break;
			case '-': t.type = T_MINUS; break;
			case '*': t.type = T_MUL; break;
			case '/': t.type = T_DIV; break;
			case '(': t.type = T_LPAREN; break;
			case ')': t.type = T_RPAREN; break;
			case '^': t.type = T_POW; break;
			}
		}

		if (t.type != T_NONE) break;
	} while (c);
	return t;
}

double expr(char **s);

double power(char **s)
{
	double ret;

	switch (t.type) {
	case T_NUM:
		ret = t.val;
		t = get_token(s);
		return ret;
	case T_LPAREN:
		t = get_token(s);
		ret = expr(s);
		t = get_token(s); // )
		return ret;
	default:
		err();
		return 0.0;
	}
}

double factor(char **s)
{
	double ret;

	ret = power(s);

	while (t.type == T_POW) {
		t = get_token(s);
		ret = pow(ret, power(s));
	}

	return ret;
}

double term(char **s)
{
	double ret;
	int op;

	ret = power(s);

	while (t.type == T_MUL || t.type == T_DIV) {
		op = t.type;
		t = get_token(s);
		if (op == T_MUL)
			ret *= factor(s);
		else if (op == T_DIV)
			ret /= factor(s);
	}

	return ret;
}

double expr(char **s)
{
	double ret;

	ret = term(s);

	while (t.type == T_PLUS || t.type == T_MINUS) {
		switch (t.type) {
		case T_PLUS:
			t = get_token(s);
			ret += term(s);
		}
	}

	return ret;
}

double parse(char *s)
{
	t = get_token(&s);
	return expr(&s);
}

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