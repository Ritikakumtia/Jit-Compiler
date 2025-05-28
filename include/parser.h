#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

void parser_init(const char *src);
ASTNode *parse_program();

#endif
