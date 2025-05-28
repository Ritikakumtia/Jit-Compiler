#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"

static const char *input;
static int pos;

// Forward declarations
static ASTNode *parse_expression();
static ASTNode *parse_statement();

static void skip_whitespace() {
    while (input[pos] && isspace(input[pos])) pos++;
}

static int is_alpha(char c) {
    return isalpha(c) || c == '_';
}

static int is_alnum(char c) {
    return isalnum(c) || c == '_';
}

static char *parse_identifier() {
    skip_whitespace();
    int start = pos;
    while (is_alnum(input[pos])) pos++;
    int len = pos - start;
    char *id = malloc(len + 1);
    if (!id) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    strncpy(id, input + start, len);
    id[len] = '\0';
    return id;
}

static int parse_number() {
    skip_whitespace();
    int val = 0;
    while (isdigit(input[pos])) {
        val = val * 10 + (input[pos++] - '0');
    }
    return val;
}

static ASTNode *parse_factor() {
    skip_whitespace();

    if (isdigit(input[pos])) {
        return make_number(parse_number());
    } else if (is_alpha(input[pos])) {
        char *id = parse_identifier();
        return make_variable(id);
    } else if (input[pos] == '(') {
        pos++;  // consume '('
        ASTNode *expr = parse_expression();
        skip_whitespace();

        if (input[pos] != ')') {
            fprintf(stderr, "parse_factor error at pos %d: expected ')', found '%c'\n", pos, input[pos]);
            panic("Expected ')'");
        }
        pos++;  // consume ')'
        return expr;
    }

    panic("Invalid factor");
    return NULL;  // unreachable, but avoids compiler warning
}

static ASTNode *parse_term() {
    ASTNode *left = parse_factor();
    skip_whitespace();
    while (input[pos] == '*' || input[pos] == '/') {
        char op = input[pos++];
        ASTNode *right = parse_factor();
        left = make_bin_op(left, op, right);
    }
    return left;
}

static ASTNode *parse_expression() {
    ASTNode *left = parse_term();
    skip_whitespace();
    while (input[pos] == '+' || input[pos] == '-') {
        char op = input[pos++];
        ASTNode *right = parse_term();
        left = make_bin_op(left, op, right);
    }
    return left;
}

static ASTNode *parse_print() {
    skip_whitespace();
    if (strncmp(input + pos, "print", 5) != 0) panic("Expected 'print'");
    pos += 5;
    ASTNode *expr = parse_expression();
    if (input[pos++] != ';') panic("Expected ';'");
    return make_print(expr);
}

static ASTNode *parse_assignment() {
    char *id = parse_identifier();
    skip_whitespace();
    if (input[pos++] != '=') panic("Expected '='");
    ASTNode *expr = parse_expression();
    if (input[pos++] != ';') panic("Expected ';'");
    return make_assignment(id, expr);
}

static ASTNode *parse_statement() {
    skip_whitespace();
    if (strncmp(input + pos, "print", 5) == 0) {
        return parse_print();
    } else if (is_alpha(input[pos])) {
        return parse_assignment();
    } else {
        panic("Unknown statement");
    }
    return NULL;
}

ASTNode *parse_program() {
    parser_init(input); // optional re-init
    ASTNode **stmts = malloc(sizeof(ASTNode*) * 100);
    if (!stmts) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    int count = 0;
    while (input[pos] != '\0') {
        ASTNode *stmt = parse_statement();
        stmts[count++] = stmt;
        skip_whitespace();
    }
    return make_sequence(stmts, count);
}

void parser_init(const char *src) {
    input = src;
    pos = 0;
}
