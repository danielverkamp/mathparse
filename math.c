#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <string.h>

#include "mathparse.h"

/*
syntax:

expr -> term + term
expr -> term - term
expr -> term

term -> factor * factor
term -> factor / factor
term -> factor

factor -> - unary
factor -> unary !
factor -> unary

unary -> power ^ power
unary -> power

power -> num
power -> const
power -> ( expr )
power -> funct ( expr )

*/

enum { T_NONE, T_EOF, T_PLUS, T_MINUS, T_MUL, T_DIV, T_LPAREN, T_RPAREN, T_NUM, T_POW, T_WORD, T_BANG };

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
	const char *name;
	double (*funct)(double);
} funct;

static const funct functs[] = {
	{ "acos", acos },
	{ "asin", asin },
	{ "atan", atan },
	{ "cos", cos },
	{ "sin", sin },
	{ "tan", tan },
	{ "cosh", cosh },
	{ "sinh", sinh },
	{ "tanh", tanh },
	{ "acosh", acosh },
	{ "asinh", asinh },
	{ "atanh", atanh },
	{ "exp", exp },
	{ "ln", log },
	{ "sqrt", sqrt },
	{ "cbrt", cbrt },
	{ "ceil", ceil },
	{ "abs", fabs },
	{ "floor", floor },
	{ "significand", significand },
	{ "erf", erf },
	{ "erfc", erfc },
	{ "lgamma", lgamma },
	{ "gamma", gamma },
	{ "rint", rint },
	{ NULL }
};

typedef struct {
	tok t;
	const char **s;
} parse_ctx;

static const constant *find_const(const char *name)
{
	const constant *c;

	for (c = consts; c->name; c++)
		if (!strcasecmp(c->name, name))
			return c;

	return NULL;
}

static const funct *find_funct(const char *name)
{
	const funct *f;

	for (f = functs; f->name; f++)
		if (!strcasecmp(f->name, name))
			return f;

	return NULL;
}

static void err()
{
	fprintf(stderr, "parse error\n");
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
			case '!': ctx->t.type = T_BANG; break;
			}
		}
	} while (ctx->t.type == T_NONE && c);
}

static double factorial(double x)
{
	if (x > 170.0) return 0.0;
	if (x <= 0.0) return 1.0;
	return x * factorial(x - 1.0);
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
		const funct *f = NULL;
		if (!c) {
			f = find_funct(ctx->t.str);
			if (!f) {
				err();
				return 0.0;
			}
			get_token(ctx); // (
			get_token(ctx);
			ret = f->funct(expr(ctx));
		} else {
			ret = c->val;
		}
		get_token(ctx);
		return ret;
	}
	case T_LPAREN:
		get_token(ctx);
		ret = expr(ctx);
		get_token(ctx);
		return ret;
	default:
		err();
		return 0.0;
	}
}

static double unary(parse_ctx *ctx)
{
	double ret;

	ret = power(ctx);

	while (ctx->t.type == T_POW) {
		get_token(ctx);
		ret = pow(ret, power(ctx));
	}

	return ret;
}

static double factor(parse_ctx *ctx)
{
	double ret;
	int sign = 1;

	while (ctx->t.type == T_MINUS) {
		get_token(ctx);
		sign = -sign;
	}

	ret = sign * unary(ctx);

	while (ctx->t.type == T_BANG) {
		get_token(ctx);
		ret = factorial(floor(ret));
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
	int op;

	ret = term(ctx);

	while (ctx->t.type == T_PLUS || ctx->t.type == T_MINUS) {
		op = ctx->t.type;
		get_token(ctx);
		if (op == T_PLUS)
			ret += term(ctx);
		else if (op == T_MINUS)
			ret -= term(ctx);
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
