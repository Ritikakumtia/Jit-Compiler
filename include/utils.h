#ifndef UTILS_H
#define UTILS_H

#include "ast.h"  

void panic(const char *msg);
void print_ast(ASTNode *node, int indent);

#endif
