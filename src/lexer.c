#include "lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int tokenize(const char* filename, Token* tokens, int max_tokens) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open input file");
        return -1;
    }

    int index = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF && index < max_tokens) {
        if (isspace(ch)) continue;

        if (isalpha(ch)) {
            char buffer[MAX_TOKEN_LEN];
            int i = 0;
            buffer[i++] = ch;
            while (isalnum(ch = fgetc(file)) && i < MAX_TOKEN_LEN - 1)
                buffer[i++] = ch;
            buffer[i] = '\0';
            if (ch != EOF) ungetc(ch, file);

            Token tok;
            if (strcmp(buffer, "print") == 0)
                tok.type = TOKEN_PRINT;
            else
                tok.type = TOKEN_IDENTIFIER;
            strcpy(tok.text, buffer);
            tokens[index++] = tok;
        }
        else if (isdigit(ch)) {
            char buffer[MAX_TOKEN_LEN];
            int i = 0;
            buffer[i++] = ch;
            while (isdigit(ch = fgetc(file)) && i < MAX_TOKEN_LEN - 1)
                buffer[i++] = ch;
            buffer[i] = '\0';
            if (ch != EOF) ungetc(ch, file);

            Token tok = {TOKEN_NUMBER, ""};
            strcpy(tok.text, buffer);
            tokens[index++] = tok;
        }
        else if (ch == '=') {
            tokens[index++] = (Token){TOKEN_ASSIGN, "="};
        }
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
            char op[2] = {ch, '\0'};
            tokens[index++] = (Token){TOKEN_OPERATOR, ""};
            strcpy(tokens[index - 1].text, op);
        }
        else {
            // ignore unknown chars
        }
    }
    tokens[index++] = (Token){TOKEN_END, ""};
    fclose(file);
    return index;
}
