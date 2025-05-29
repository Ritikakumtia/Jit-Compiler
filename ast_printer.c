#include <stdio.h>
#include "ast.h"

void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case AST_NUMBER:
            printf("Number: %d\n", node->number);
            break;
        case AST_VARIABLE:
            printf("Variable: %s\n", node->var_name);
            break;
        case AST_ASSIGNMENT:
            printf("Assignment to: %s\n", node->assignment.name);
            print_ast(node->assignment.value, indent + 1);
            break;
        case AST_BIN_OP:
            printf("Binary Operation: %c\n", node->bin_op.op);
            print_ast(node->bin_op.left, indent + 1);
            print_ast(node->bin_op.right, indent + 1);
            break;
        case AST_PRINT:
            printf("Print Statement\n");
            print_ast(node->print_expr, indent + 1);
            break;
        case AST_SEQUENCE:
            printf("Sequence (%d statements):\n", node->sequence.count);
            for (int i = 0; i < node->sequence.count; i++) {
                print_ast(node->sequence.stmts[i], indent + 1);
            }
            break;
        case AST_IF:
            printf("If Statement:\n");
            print_ast(node->if_stmt.cond, indent + 1);
            printf("%*sThen:\n", indent * 2, "");
            print_ast(node->if_stmt.then_branch, indent + 2);
            if (node->if_stmt.else_branch) {
                printf("%*sElse:\n", indent * 2, "");
                print_ast(node->if_stmt.else_branch, indent + 2);
            }
            break;
        case AST_WHILE:
            printf("While Loop:\n");
            print_ast(node->while_stmt.cond, indent + 1);
            printf("%*sBody:\n", indent * 2, "");
            print_ast(node->while_stmt.body, indent + 2);
            break;
        default:
            printf("Unknown node type\n");
    }
}
