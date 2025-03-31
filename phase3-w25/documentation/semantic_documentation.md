Symbol Table Implementation
Semantic Checking rules
Error Handling Approach

# SEMANTIC ANALYZER IMPLEMENTATION
## Symbol Table Implementation:
The symbol table is used to manage variable declarations, types, scopes, and initialization status. It enables semantic checking by tracking identifiers and enforcing scope rules.

#### Adding Symbols to table:
```c
 void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type; // int/char
        symbol->scope_level = table->current_scope; // Current Scope of variable based on table
        symbol->line_declared = line; // Line declared
        symbol->is_initialized = 0;  // Not initialized 

        symbol->next = table->head;
        table->head = symbol;
    }
}
```
Where the symbol entry contained information
- Name
- Type
- Scope 
- Line
- Initialization status

#### Removing symbols from table:
```c
void remove_symbol(SymbolTable* table, const char* name) {
    if (!table || !table->head) return; // If table is NULL or empty, do nothing.

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
```

#### Removing symbols from table within current scope:
```c
void remove_symbols_in_current_scope(SymbolTable* table) {
    if (!table || !table->head) return;// If table is NULL or empty, do nothing.

    // Remove symbols from the head of the list if they belong to current scope.
    while (table->head && table->head->scope_level > table->current_scope) {
        Symbol* temp = table->head;
        table->head = table->head->next;
        free(temp);
    }

    //Traverse List and remove symbols belonging to current scope.
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
```
#### Looking up symbols globally:
```c
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
```

#### Looking up symbols within current scope:
```c
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
```

#### Managing Scope:
```c
void enter_scope(SymbolTable* table) {
    if (!table) return; // If table is NULL or emtpy, do nothing
    table->current_scope++; // Increment table scope attribute
}

void exit_scope(SymbolTable* table) {
    if (!table) return; // If table is NULL or empty, do nothing

    // Check if it is possible to exit current scope
    if (table->current_scope > 0) {
        table->current_scope--; // Decrement table scope attribute
    }
}
```
#### Tracking Initalization Status:
```c
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
```

## Semantic Checking Rules:

### 1. Variable Declaration and Usage
#### 1.1 Variables Must be Declared Before Use
- Before variable can be assigned or referenced, it must be declared.
  - e.g. `x = 5; `❌ `int = x; x = 5;`✅ 
- If an undeclared variable is used, `SEM_ERROR_UNDECLARED_VARIABLE` will be reported.

#### 1.2 No Redeclaration in the Same Scope
- If a variable is redeclared in the same scope, `SEM_ERROR_REDECLARED_VARIABLE` will be reported.

#### 1.3 Initialization Check
- If a variable is used before being initialized, `SEM_ERROR_UNINITIALIZED_VARIABLE` will be reported.

### 2. Type Checking Rules
#### 2.1 Type Compatibility in Assignment 
-  Type of RHS of expression must be compatibile with the type on the LHS.
-  `char` can be implicitly set to `int`. But `int` cannot be assigned to `char`.
-  Incompatible Types are reported as `SEM_ERROR_TYPE_MISMATCH`.

#### 2.2 Expression Type Determination
- Expressions have defined types based on:
  - `AST_NUMBER` = `int`
  - `AST_IDENTIFIER` = Type from symbol table
  - `AST_BINOP` = Type from operands
    - e.g. `int + int`: Type is `int`
  - `AST_CONDITION` & `AST_COMPARISON` = `int`(0 or 1)
  - `AST_FACTORIAL` = `int`
    - `SEM_ERROR_INVALID_OPERATION` will be reported if operation is applied to `char`

### 3. Control Flow Statements
#### 3.1 If Statement
- Condition expression must be evaluated as `int`
- `SEM_ERROR_INVALID_OPERATION` will be reported otherwise.
- The body will be checked recursively.

#### 3.2 While Loop
- Condition expression must be evaluated as `int`
- `SEM_ERROR_INVALID_OPERATION` will be reported otherwise.
- The body will be checked recursively.

#### 3.3 Repeat-Until
- Loop body is checked first.
- Condition expression must be evaluated as `int`
- `SEM_ERROR_INVALID_OPERATION` will be reported otherwise.

### 4. Scope Rules
#### 4.1 Entering and Exiting Scopes
- When a block is encountered a new scope is created and entered.
- When block exits variables are kept in symbol table with scope recorded.

## Error Handling
When an error is encountered, an error message is generated containing the line, error code and variable name.
#### Semantic Errors
## 7.1 Semantic Error Codes

| Error Code                         | Description                                      | Error Message Format                              |
|-------------------------------------|--------------------------------------------------|--------------------------------------------------|
| `SEM_ERROR_UNDECLARED_VARIABLE`    | Undeclared variable usage.                      | `Semantic Error at line X: Undeclared variable 'name'` |
| `SEM_ERROR_REDECLARED_VARIABLE`    | Redeclaration of a variable in the same scope.  | `Semantic Error at line X: Variable 'name' already declared in this scope` |
| `SEM_ERROR_TYPE_MISMATCH`          | Type mismatch in an operation or assignment.    | `Semantic Error at line X: Type mismatch involving 'name'` |
| `SEM_ERROR_UNINITIALIZED_VARIABLE` | Use of an uninitialized variable.               | `Semantic Error at line X: Variable 'name' may be used uninitialized` |
| `SEM_ERROR_INVALID_OPERATION`      | Invalid operation involving a variable.         | `Semantic Error at line X: Invalid operation involving 'name'` |
| `SEM_ERROR_SEMANTIC_ERROR`         | General semantic error.                         | `Semantic Error at line X: Semantic error involving 'name'` |