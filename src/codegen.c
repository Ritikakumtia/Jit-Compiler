#include <libgccjit.h>
#include <stdio.h>
#include <string.h>
#include "codegen.h"
#include "utils.h"

#define MAX_VARS 100

typedef struct {
    char *name;
    gcc_jit_lvalue *lval;
} Var;

static Var vars[MAX_VARS];
static int var_count = 0;

static gcc_jit_lvalue *lookup_var(gcc_jit_context *ctx, gcc_jit_function *func, gcc_jit_type *int_type, const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0)
            return vars[i].lval;
    }
    gcc_jit_lvalue *v = gcc_jit_function_new_local(func, NULL, int_type, name);
    vars[var_count++] = (Var){strdup(name), v};
    return v;
}

static gcc_jit_rvalue *emit_expr(gcc_jit_context *ctx, gcc_jit_function *func, gcc_jit_block *block, ASTNode *node) {
    switch (node->type) {
        case AST_NUMBER:
            return gcc_jit_context_new_rvalue_from_int(ctx, gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT), node->number);
        case AST_VARIABLE:
            return gcc_jit_lvalue_as_rvalue(lookup_var(ctx, func, gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT), node->var_name));
        case AST_BIN_OP: {
            gcc_jit_rvalue *lhs = emit_expr(ctx, func, block, node->bin_op.left);
            gcc_jit_rvalue *rhs = emit_expr(ctx, func, block, node->bin_op.right);
            enum gcc_jit_binary_op op;
            switch (node->bin_op.op) {
                case '+': op = GCC_JIT_BINARY_OP_PLUS; break;
                case '-': op = GCC_JIT_BINARY_OP_MINUS; break;
                case '*': op = GCC_JIT_BINARY_OP_MULT; break;
                case '/': op = GCC_JIT_BINARY_OP_DIVIDE; break;
                default: panic("Unknown binary op");
            }
            return gcc_jit_context_new_binary_op(ctx, NULL, op, gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT), lhs, rhs);
        }
        default:
            panic("Invalid expression");
            return NULL;
    }
}

static void emit_stmt(gcc_jit_context *ctx, gcc_jit_function *func, gcc_jit_block *block, ASTNode *node) {
    switch (node->type) {
        case AST_ASSIGNMENT: {
            gcc_jit_lvalue *lval = lookup_var(ctx, func, gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT), node->assignment.name);
            gcc_jit_rvalue *rval = emit_expr(ctx, func, block, node->assignment.value);
            gcc_jit_block_add_assignment(block, NULL, lval, rval);
            break;
        }
        case AST_PRINT: {
            gcc_jit_type *int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);
            gcc_jit_type *char_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_CHAR);
            gcc_jit_type *char_ptr_type = gcc_jit_type_get_pointer(char_type);

            // Correct parameter type for printf: char*
            gcc_jit_param *param = gcc_jit_context_new_param(ctx, NULL, char_ptr_type, "format");

            // Imported printf function declaration
            gcc_jit_function *printf_func = gcc_jit_context_new_function(
                ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, int_type, "printf", 1, &param, 1);

            // Format string literal
            gcc_jit_rvalue *fmt = gcc_jit_context_new_string_literal(ctx, "Value: %d\n");
            gcc_jit_rvalue *val = emit_expr(ctx, func, block, node->print_expr);

            // Call printf(format, val)
            gcc_jit_block_add_eval(block, NULL, gcc_jit_context_new_call(ctx, NULL, printf_func, 2, (gcc_jit_rvalue *[]){fmt, val}));
            break;
        }
        case AST_SEQUENCE: {
            for (int i = 0; i < node->sequence.count; i++) {
                emit_stmt(ctx, func, block, node->sequence.stmts[i]);
            }
            break;
        }
        default:
            panic("Invalid statement");
    }
}

void generate_and_execute(ASTNode *program) {
    gcc_jit_context *ctx = gcc_jit_context_acquire();
    gcc_jit_type *int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);
    gcc_jit_function *main_func = gcc_jit_context_new_function(
        ctx, NULL, GCC_JIT_FUNCTION_EXPORTED, int_type, "main", 0, NULL, 0);
    gcc_jit_block *block = gcc_jit_function_new_block(main_func, "entry");

    emit_stmt(ctx, main_func, block, program);

    gcc_jit_block_end_with_return(block, NULL, gcc_jit_context_new_rvalue_from_int(ctx, int_type, 0));

    gcc_jit_result *res = gcc_jit_context_compile(ctx);
    if (!res) {
        panic("Failed to compile JIT code");
    }
    int (*fn)() = (int (*)())gcc_jit_result_get_code(res, "main");
    if (!fn) {
        panic("Failed to get compiled function pointer");
    }

    fn();

    gcc_jit_result_release(res);
    gcc_jit_context_release(ctx);
}
