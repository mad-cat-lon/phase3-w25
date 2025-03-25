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
        symbol->is_initialized = 0;
        
        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;
    }
}

void remove_symbol(SymbolTable* table, const char* name) {
    if (!table || !table->head) return;

    Symbol* current = table -> head;

    if(strcmp(current->name, name) == 0) {
        table -> head = current ->next;
        free(current);
    }

    while(current && current->next) {
        if (strcmp(current->next->name, name) == 0) {
            Symbol* temp = current->next;
            current -> next = current->next->next;
            free(temp);
        }
        current = current->next;
    }
}

// Look up symbol by name
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
    }
    else {
        return;
    }
}

void remove_symbols_in_current_scope(SymbolTable* table) {
    if (!table || !table->head) return;
    
    while (table->head && table->head->scope_level == table->current_scope) {
        Symbol* temp = table->head;
        table->head = table->head->next;
        free(temp);
    }
    Symbol* current = table->head;
    while (current && current->next) {
        if (current->next->scope_level == table->current_scope) {
            Symbol* temp = current->next;
            current->next = current->next->next;
            free(temp);
        }
        current = current->next;
    }
}

void initialize_symbol(SymbolTable* table, const char* name) {
    Symbol* current = lookup_symbol(table, name);
    if (current) {
        current-> is_initialized = 1;
    }
}

int is_initialized(SymbolTable* table, const char* name) {
    Symbol* current = lookup_symbol(table, name);
    return current->is_initialized;
}

void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    Symbol* current = table->head;
    while(current) {
        Symbol* temp = current;
        current = current->next;
        free(temp);
    }
    free(table);
}

// Analyze AST semantically
int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_program(ast, table);
    free_symbol_table(table);
    return result;
}

// Check program node
int check_program(ASTNode* node, SymbolTable* table) {
    if (!node) return 1;
    
    int result = 1;
    
    if (node->type == AST_PROGRAM) {
        // Check left child (statement)
        if (node->left) {
            result = check_statement(node->left, table) && result;
        }
        
        // Check right child (rest of program)
        if (node->right) {
            result = check_program(node->right, table) && result;
        }
    }
    
    return result;
}

int check_statement(ASTNode* node, SymbolTable* table) {
    if (node->type == AST_VARDECL) {

    }
}

// Check declaration node
int check_declaration(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_VARDECL) {
        return 0;
    }
    
    const char* name = node->token.lexeme;
    
    // Check if variable already declared in current scope
    Symbol* existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }
    
    // Add to symbol table
    add_symbol(table, name, TOKEN_INT, node->token.line);
    return 1;
}

// Check assignment node
int check_assignment(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_ASSIGN || !node->left || !node->right) {
        return 0;
    }
    
    const char* name = node->left->token.lexeme;
    
    // Check if variable exists
    Symbol* symbol = lookup_symbol(table, name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }
    
    // Check expression
    int expr_valid = check_expression(node->right, table);
    
    // Mark as initialized
    if (expr_valid) {
        symbol->is_initialized = 1;
    }
    
    return expr_valid;
}

int check_block(ASTNode* node, SymbolTable* table){

}

int check_condition(ASTNode* node, SymbolTable* table) {

}

int check_expression(ASTNode* node, SymbolTable* table){

}


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
                        "}\n";
    
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
