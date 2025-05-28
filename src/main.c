#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"
#include "utils.h"

#define MAX_SRC_LEN 8192

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char *source = malloc(MAX_SRC_LEN);
    fread(source, 1, MAX_SRC_LEN, file);
    fclose(file);

    parser_init(source);
    ASTNode *program = parse_program();

    // 🔍 Print AST
    printf("=== AST Tree ===\n");
    print_ast(program, 0);

    // ▶️ Execute JIT compiled code
    printf("\n=== Executing JIT compiled code ===\n");
    generate_and_execute(program);

    free(source);

    printf("\nPress Enter to exit...");
    getchar();

    return 0;
}
