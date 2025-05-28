#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKEN_LEN 64

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_ASSIGN,
    TOKEN_PRINT,
    TOKEN_END
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;

int tokenize(const char* filename, Token* tokens, int max_tokens);

#endif
