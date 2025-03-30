#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/semantic.h"
// Initialize symbol table
SymbolTable* init_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Add symbol to table
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;  // Not initialized yet

        symbol->next = table->head;
        table->head = symbol;
    }
}

// Remove a specific symbol by name (only removes the first match)
void remove_symbol(SymbolTable* table, const char* name) {
    if (!table || !table->head) return;

    Symbol* current = table->head;
    Symbol* prev = NULL;

    while (current) {
        if (strcmp(current->name, name) == 0) {
            // Remove current from the linked list
            if (!prev) {
                // Removing the head
                table->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Look up symbol in current scope only
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 &&
            current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void enter_scope(SymbolTable* table) {
    if (!table) return;
    table->current_scope++;
}

void exit_scope(SymbolTable* table) {
    if (!table) return;
    if (table->current_scope > 0) {
        table->current_scope--;
        // remove_symbols_in_current_scope(table);
    }
}

void remove_symbols_in_current_scope(SymbolTable* table) {
    if (!table || !table->head) return;

    while (table->head && table->head->scope_level > table->current_scope) {
        Symbol* temp = table->head;
        table->head = table->head->next;
        free(temp);
    }

    Symbol* current = table->head;
    while (current && current->next) {
        if (current->next->scope_level > table->current_scope) {
            Symbol* temp = current->next;
            current->next = current->next->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

void initialize_symbol(SymbolTable* table, const char* name) {
    Symbol* current = lookup_symbol(table, name);
    if (current) {
        current->is_initialized = 1;
    }
}

int is_initialized(SymbolTable* table, const char* name) {
    Symbol* current = lookup_symbol(table, name);
    return (current && current->is_initialized);
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    Symbol* current = table->head;
    while (current) {
        Symbol* temp = current;
        current = current->next;
        free(temp);
    }
    free(table);
}

// Optional helper to print the symbol table for debugging
static void print_symbol_table(SymbolTable* table) {
    printf("\n== SYMBOL TABLE DUMP ==\n");
    Symbol* current = table->head;
    int count = 0;
    while (current) {
        count++;
        current = current->next;
    }
    printf("Total symbols: %d\n\n", count);

    current = table->head;
    int index = 0;
    while (current) {
        printf("Symbol[%d]:\n", index);
        printf("  Name: %s\n", current->name);
        if (current->type == TOKEN_INT)
            printf("  Type: int\n");
        else if (current->type == TOKEN_CHAR)
            printf("  Type: char\n");
        else
            printf("  Type: Unknown (%d)\n", current->type);

        printf("  Scope Level: %d\n", current->scope_level);
        printf("  Line Declared: %d\n", current->line_declared);
        printf("  Initialized: %s\n\n", current->is_initialized ? "Yes" : "No");
        current = current->next;
        index++;
    }
    printf("=======================\n");
}

// Forward declarations for expression type-checking
static int get_expression_type(ASTNode* node, SymbolTable* table);

// We'll define these check_XXX helpers used by check_statement
static int check_if(ASTNode* node, SymbolTable* table);
static int check_while(ASTNode* node, SymbolTable* table);
static int check_repeat(ASTNode* node, SymbolTable* table);
static int check_print(ASTNode* node, SymbolTable* table);

// Check the top-level program (basically a list of statements)
int check_program(ASTNode* node, SymbolTable* table) {
    int result = 1;

    while (node) {
        if (node->type == AST_PROGRAM) {
            node = node->next;
            continue;
        }
        result = check_statement(node, table) && result;
        node = node->next;
    }

    return result;
}

int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_program(ast, table);

    // Uncomment to see the final symbol table after analysis:
    print_symbol_table(table);

    free_symbol_table(table);
    return result;
}


int check_statement(ASTNode* node, SymbolTable* table) {
    if (!node) return 1;

    switch (node->type) {
        case AST_VARDECL:
            return check_declaration(node, table);

        case AST_ASSIGN:
            return check_assignment(node, table);

        case AST_BLOCK:
            return check_block(node, table);

        case AST_IF:
            return check_if(node, table);

        case AST_WHILE:
            return check_while(node, table);

        case AST_REPEAT:
            return check_repeat(node, table);

        case AST_PRINT:
            return check_print(node, table);

        default:
            semantic_error(SEM_ERROR_SEMANTIC_ERROR,
                           "Unknown statement type", node->token.line);
            return 0;
    }
}

int check_declaration(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_VARDECL) return 0;

    // The parser sets node->token as the identifier token
    const char* name = node->token.lexeme;

    // Check if variable is redeclared in the same scope
    Symbol* existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // The token type might be TOKEN_INT or TOKEN_CHAR, etc.
    // If your parser put `int`, `char`, etc. in `node->token.type`:
    int declared_type = node->token.type; // e.g., TOKEN_INT or TOKEN_CHAR

    // Add to symbol table
    add_symbol(table, name, declared_type, node->token.line);
    return 1;
}

int check_assignment(ASTNode* node, SymbolTable* table) {
    // AST_ASSIGN with node->left = identifier node, node->right = expression
    if (!node->left || !node->right) return 0;

    const char* var_name = node->left->token.lexeme;

    // Ensure variable is declared
    Symbol* symbol = lookup_symbol(table, var_name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, var_name, node->token.line);
        return 0;
    }

    // Get expression type
    int expr_type = get_expression_type(node->right, table);
    if (expr_type == -1) {
        // -1 means an error was already reported during expression check
        return 0;
    }

    // Check type compatibility. 
    // Simple logic: if symbol->type == TOKEN_INT but expr_type == TOKEN_CHAR,
    //   we allow it (char -> int). If symbol->type == TOKEN_CHAR but expr_type == TOKEN_INT,
    //   we flag a mismatch. Adjust to your language rules.
    if (symbol->type == TOKEN_INT && expr_type == TOKEN_CHAR) {
        // Allowed: char -> int
    }
    else if (symbol->type == TOKEN_CHAR && expr_type == TOKEN_INT) {
        // Not allowed: int -> char
        semantic_error(SEM_ERROR_TYPE_MISMATCH, var_name, node->token.line);
        return 0;
    }
    else if (symbol->type != expr_type) {
        // If both are the same or are something else?
        // We only handle int/char. If they differ, mismatch.
        if (!(symbol->type == TOKEN_INT && expr_type == TOKEN_INT) &&
            !(symbol->type == TOKEN_CHAR && expr_type == TOKEN_CHAR))
        {
            semantic_error(SEM_ERROR_TYPE_MISMATCH, var_name, node->token.line);
            return 0;
        }
    }

    // Mark variable as initialized
    symbol->is_initialized = 1;
    return 1;
}

static int check_if(ASTNode* node, SymbolTable* table) {
    // node->left = expression (the condition)
    // node->right = statement or block
    //  e.g. if(...) { ... } 
    int cond_type = get_expression_type(node->left, table);
    if (cond_type == -1) return 0; // error in the condition


    // Now check the body
    return check_statement(node->right, table);
}

static int check_while(ASTNode* node, SymbolTable* table) {
    // node->left = expression (the condition)
    // node->right = statement or block
    int cond_type = get_expression_type(node->left, table);
    if (cond_type == -1) return 0; // error

    // Check body
    return check_statement(node->right, table);
}

/**
 * parse_repeat_statement does:
 *   node->type = AST_REPEAT
 *   node->left = AST_BLOCK  (the block)
 *   node->right = AST_CONDITION
 *   node->right->left = parse_expression()  (the actual expr)
 */
static int check_repeat(ASTNode* node, SymbolTable* table) {
    // Check the block (node->left). That is an AST_BLOCK itself.
    // So we can just call check_statement on node->left; that'll do scope handling.
    int result = check_statement(node->left, table);

    // Now node->right is AST_CONDITION, whose left is the expression
    if (node->right && node->right->left) {
        int cond_type = get_expression_type(node->right->left, table);
        if (cond_type == -1) result = 0;
    } else {
        semantic_error(SEM_ERROR_SEMANTIC_ERROR, "repeat-until condition", node->token.line);
        result = 0;
    }

    return result;
}


int check_block(ASTNode* node, SymbolTable* table) {
    // Enter a new scope
    enter_scope(table);

    // The parser’s parse_block() returns an AST_BLOCK whose `node->next`
    // is the first statement in that block. We can walk through them:
    ASTNode* stmt = node->next;
    int result = 1;
    while (stmt) {
        result = check_statement(stmt, table) && result;
        stmt = stmt->next;
    }

    // Exit scope (removes block’s declarations)
    exit_scope(table);
    return result;
}


static int check_print(ASTNode* node, SymbolTable* table) {
    // For print, node->left should be the expression to print.
    int expr_type = get_expression_type(node->left, table);
    if (expr_type == -1) {
        return 0;
    }
    return 1;
}


/**
 * Return the type of the expression:
 *   TOKEN_INT, TOKEN_CHAR, or -1 if error
 * We do basic checks for:
 *   - AST_NUMBER => int
 *   - AST_IDENTIFIER => type from symbol table (error if not declared)
 *   - AST_BINOP => unify left & right type, or report mismatch
 *   - AST_CONDITION => typically yields int or bool. For simplicity, treat as int
 *   - AST_FACTORIAL => typically int
 *   etc.
 */
static int get_expression_type(ASTNode* node, SymbolTable* table) {
    if (!node) return -1; // error if no expression

    switch (node->type) {
        case AST_NUMBER:
            return TOKEN_INT;

        case AST_IDENTIFIER: {
            Symbol* sym = lookup_symbol(table, node->token.lexeme);
            if (!sym) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE,
                               node->token.lexeme, node->token.line);
                return -1;
            }
            // Warn if uninitialized
            if (!sym->is_initialized) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE,
                               node->token.lexeme, node->token.line);
            }
            return sym->type;
        }

        case AST_BINOP: {
            // node->left, node->right are sub-expressions
            int left_type = get_expression_type(node->left, table);
            int right_type = get_expression_type(node->right, table);
            if (left_type == -1 || right_type == -1) return -1; // error


            if (left_type == TOKEN_INT || right_type == TOKEN_INT) {
                return TOKEN_INT;  
            }
            // else if both char, maybe return char. 
            return TOKEN_CHAR;
        }

        case AST_CONDITION:
        case AST_COMPARISON: {
            if (node->left) get_expression_type(node->left, table);
            if (node->right) get_expression_type(node->right, table);
            return TOKEN_INT;
        }

        case AST_FACTORIAL: {
            // Usually factorial returns an int
            // node->left is the argument
            int arg_type = get_expression_type(node->left, table);
            if (arg_type == -1) return -1; 
            // If you want to enforce that factorial’s argument must be an int:
            if (arg_type == TOKEN_CHAR) {
                semantic_error(SEM_ERROR_TYPE_MISMATCH, "factorial()", node->token.line);
                return -1;
            }
            return TOKEN_INT;
        }

        default:
            // If it's something else (like AST_BLOCK?), that's not a valid expression
            semantic_error(SEM_ERROR_INVALID_OPERATION, "expression", node->token.line);
            return -1;
    }
}

// ---------------------------------------------------------------------------
// ERROR REPORTING
// ---------------------------------------------------------------------------

void semantic_error(SemanticErrorType error, const char* name, int line) {
    printf("Semantic Error at line %d: ", line);

    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s'\n", name);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope\n", name);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s'\n", name);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized\n", name);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s'\n", name);
            break;
        case SEM_ERROR_SEMANTIC_ERROR:
            printf("Semantic error involving '%s'\n", name);
            break;
        default:
            printf("Unknown semantic error with '%s'\n", name);
    }
}


int main() {
    const char* input = "int x;\n"
                        "x = 42;\n"
                        "if (x > 0) {\n"
                        "    int y;\n"
                        "    y = x + 10;\n"
                        "    print y;\n"
                        "}\n"
                        "z = 10;\n"
                        "char test;\n"
                        "test = 123;\n"
                        "x = test + 1;";
    
    printf("Analyzing input:\n%s\n\n", input);
    
    // Lexical analysis and parsing
    parser_init(input);
    ASTNode* ast = parse();
    
    printf("AST created. Performing semantic analysis...\n\n");
    
    // Semantic analysis
    int result = analyze_semantics(ast);
    
    if (result) {
        printf("Semantic analysis successful. No errors found.\n");
    } else {
        printf("Semantic analysis failed. Errors detected.\n");
    }
    
    // Clean up
    free_ast(ast);
    
    return 0;
}
