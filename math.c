#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <string.h>

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

power -> num
power -> const
power -> ( expr )

*/

enum { T_NONE, T_EOF, T_PLUS, T_MINUS, T_MUL, T_DIV, T_LPAREN, T_RPAREN, T_NUM, T_POW, T_WORD };

typedef struct {
	int type;
	double val;
	char *str;
} tok;

typedef struct {
	const char *name;
	double val;
} constant;

static const constant consts[] = {
	{ "e",  2.71828182845904523536 },
	{ "pi", 3.14159265358979323846 },
	{ NULL }
};

typedef struct {
	tok t;
	const char **s;
} parse_ctx;

static const constant *find_const(char *name)
{
	const constant *c;

	for (c = consts; c->name; c++)
		if (!strcasecmp(c->name, name))
			return c;

	return NULL;
}

static void err()
{
	fprintf(stderr, "parse error\n");
	exit(1);
}

static tok get_number(const char **s)
{
	const char *end;
	tok ret;

	ret.type = T_NONE;

	for (end = *s; (*end >= '0' && *end <= '9') || (*end == '.'); end++)
		;

	if (end != *s) {
		ret.type = T_NUM;
		ret.val = atof(*s);
		*s = end;
	}

	return ret;
}

static tok get_word(const char **s)
{
	const char *end;
	tok ret;

	ret.type = T_NONE;
	for (end = *s; (*end >= 'A' && *end <= 'Z') || (*end >= 'a' && *end <= 'z'); end++)
		;

	if (end != *s) {
		int len = end - *s;
		ret.type = T_WORD;
		ret.str = malloc(len + 1);
		memcpy(ret.str, *s, len);
		ret.str[len] = '\0';
		*s = end;
	}

	return ret;
}

static void get_token(parse_ctx *ctx)
{
	char c;
	tok t;

	ctx->t.type = T_NONE;

	do {
		ctx->t = get_number(ctx->s);

		if (ctx->t.type == T_NONE) {
			ctx->t = get_word(ctx->s);
		}

		if (ctx->t.type == T_NONE) {
			c = **ctx->s;
			(*ctx->s)++;

			switch (c) {
			case 0:   ctx->t.type = T_EOF; break;
			case '+': ctx->t.type = T_PLUS; break;
			case '-': ctx->t.type = T_MINUS; break;
			case '*': ctx->t.type = T_MUL; break;
			case '/': ctx->t.type = T_DIV; break;
			case '(': ctx->t.type = T_LPAREN; break;
			case ')': ctx->t.type = T_RPAREN; break;
			case '^': ctx->t.type = T_POW; break;
			}
		}
	} while (c && ctx->t.type == T_NONE);
}

static double expr(parse_ctx *ctx);

static double power(parse_ctx *ctx)
{
	double ret;

	switch (ctx->t.type) {
	case T_NUM:
		ret = ctx->t.val;
		get_token(ctx);
		return ret;
	case T_WORD:
	{
		const constant *c = find_const(ctx->t.str);
		if (!c) err();
		ret = c->val;
		get_token(ctx);
		return ret;
	}
	case T_LPAREN:
		get_token(ctx);
		ret = expr(ctx);
		get_token(ctx); // )
		return ret;
	default:
		err();
		return 0.0;
	}
}

static double factor(parse_ctx *ctx)
{
	double ret;

	ret = power(ctx);

	while (ctx->t.type == T_POW) {
		get_token(ctx);
		ret = pow(ret, power(ctx));
	}

	return ret;
}

static double term(parse_ctx *ctx)
{
	double ret;
	int op;

	ret = factor(ctx);

	while (ctx->t.type == T_MUL || ctx->t.type == T_DIV) {
		op = ctx->t.type;
		get_token(ctx);
		if (op == T_MUL)
			ret *= factor(ctx);
		else if (op == T_DIV)
			ret /= factor(ctx);
	}

	return ret;
}

static double expr(parse_ctx *ctx)
{
	double ret;

	ret = term(ctx);

	while (ctx->t.type == T_PLUS || ctx->t.type == T_MINUS) {
		switch (ctx->t.type) {
		case T_PLUS:
			get_token(ctx);
			ret += term(ctx);
		}
	}

	return ret;
}

double parse(const char *s)
{
	parse_ctx ctx;
	ctx.s = &s;
	get_token(&ctx);
	return expr(&ctx);
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
