#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *make_number(int val) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->number = val;
    return node;
}

ASTNode *make_variable(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE;
    node->var_name = strdup(name);
    return node;
}

ASTNode *make_assignment(const char *name, ASTNode *val) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->assignment.name = strdup(name);
    node->assignment.value = val;
    return node;
}

ASTNode *make_bin_op(ASTNode *left, char op, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BIN_OP;
    node->bin_op.left = left;
    node->bin_op.op = op;
    node->bin_op.right = right;
    return node;
}

ASTNode *make_print(ASTNode *expr) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_PRINT;
    node->print_expr = expr;
    return node;
}

ASTNode *make_if(ASTNode *cond, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->if_stmt.cond = cond;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

ASTNode *make_while(ASTNode *cond, ASTNode *body) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->while_stmt.cond = cond;
    node->while_stmt.body = body;
    return node;
}

ASTNode *make_sequence(ASTNode **stmts, int count) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_SEQUENCE;
    node->sequence.stmts = stmts;
    node->sequence.count = count;
    return node;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    if (node->type == AST_ASSIGNMENT) {
        free(node->assignment.name);
        free_ast(node->assignment.value);
    } else if (node->type == AST_VARIABLE) {
        free(node->var_name);
    } else if (node->type == AST_BIN_OP) {
        free_ast(node->bin_op.left);
        free_ast(node->bin_op.right);
    } else if (node->type == AST_PRINT) {
        free_ast(node->print_expr);
    } else if (node->type == AST_IF) {
        free_ast(node->if_stmt.cond);
        free_ast(node->if_stmt.then_branch);
        free_ast(node->if_stmt.else_branch);
    } else if (node->type == AST_WHILE) {
        free_ast(node->while_stmt.cond);
        free_ast(node->while_stmt.body);
    } else if (node->type == AST_SEQUENCE) {
        for (int i = 0; i < node->sequence.count; i++) {
            free_ast(node->sequence.stmts[i]);
        }
        free(node->sequence.stmts);
    }
    free(node);
}
