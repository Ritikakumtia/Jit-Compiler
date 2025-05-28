#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_BIN_OP,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_SEQUENCE
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number;
        char *var_name;
        struct {
            char *name;
            struct ASTNode *value;
        } assignment;
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            char op;
        } bin_op;
        struct ASTNode *print_expr;
        struct {
            struct ASTNode *cond;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;
        struct {
            struct ASTNode *cond;
            struct ASTNode *body;
        } while_stmt;
        struct {
            struct ASTNode **stmts;
            int count;
        } sequence;
    };
} ASTNode;

ASTNode *make_number(int val);
ASTNode *make_variable(const char *name);
ASTNode *make_assignment(const char *name, ASTNode *val);
ASTNode *make_bin_op(ASTNode *left, char op, ASTNode *right);
ASTNode *make_print(ASTNode *expr);
ASTNode *make_if(ASTNode *cond, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *make_while(ASTNode *cond, ASTNode *body);
ASTNode *make_sequence(ASTNode **stmts, int count);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);


#endif
