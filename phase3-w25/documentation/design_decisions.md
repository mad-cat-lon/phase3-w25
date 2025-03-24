# Parser Design Decisions

## Changes and Updates in Parser Design

This document highlights the modifications made to the original `parser.c` file, detailing changes in parsing logic, error handling, and AST structure.

### 1. **New Parsing Functions**

Added parsing functions to handle additional types:
- **Primary**: Base case handling of expressions`(expression1, x)`
- **If statements**: `if (condition) { ... }`
- **While loops**: `while (condition) { ... }`
- **Repeat-until loops**: `repeat { ... } until (condition)`
- **Print statements**: `print x;`
- **Block statements**: `{ statement1; statement2; }`
- **Factorial function**: `factorial(x)`

### 2. **Enhanced Error Handling**

Extended `parse_error()` function to include additional error types:
- `PARSE_ERROR_INVALID_STATEMENT`: When any statement fails to meet the syntax requirements (e.g., `if (x == 5) !`)
- `PARSE_ERROR_MISSING_LPAREN`: Missing left parenthesis `(` (e.g., `while x != 1) {...}`)
- `PARSE_ERROR_MISSING_RPAREN`: Missing right parenthesis `)` (e.g., `if (x < y` )
- `PARSE_ERROR_MISSING_LBRACE`: Missing left brace `{` (e.g., `repeat x = x + 5; } `)
- `PARSE_ERROR_MISSING_RBRACE`: Missing right brace `}` (e.g., `repeat {x = x + 5; `)
- `PARSE_ERROR_MISSING_LBRACK`: Missing left bracket `[`
- `PARSE_ERROR_MISSING_RBRACK`: Missing right bracket `]`
- `PARSE_ERROR_MISSING_UNTIL`: Missing `until` keyword (e.g., `repeat {x = x + 5;} (x == 50)`)
- `PARSE_ERROR_MISSING_COMPARISON`: Invalid comparison operator (e.g., `if (x = 5)`)

### 3. **Expression Parsing Improvements**

Extended expression parsing:
- **Binary operations** (`+`, `-`, `*`, `/`)
- **Comparison operators** (`<`, `>`, `==`, etc.)
- **Operator precedence handling**
- **Parentheses grouping**

### 4. **New AST Node Types**

The AST now includes additional node types to accommodate the new features:
- `AST_IF`: `if` statements
- `AST_WHILE`: `while` loops
- `AST_REPEAT`: `repeat-until` loops
- `AST_BLOCK`: Blocks of statements enclosed in braces 
- `AST_BINOP`: Binary operations
- `AST_FUNCTION_CALL`: Function calls
- `AST_PRINT`: `print` statements
- `AST_COMPARISON`: Comparison expressions
- `AST_FACTORIAL`: Factorials function calls

### 5. **Added AST Node Functionality**
Added `next` pointer to `ASTNode` structure to support sequences of statements or linked structures within the AST.
```cpp
typedef struct ASTNode {
    ASTNodeType type;           
    Token token;               
    struct ASTNode* left;     
    struct ASTNode* right;     
    struct ASTNode* next;  
} ASTNode;
```
**Usage:**
  - In a block of statements, each statement is linked to the next statement via the `next` pointer.
  - Allowing for tracking and efficient traversal of these elements in the AST.

**Example use case:**
```
{
    statement1;
    statement2;
    statement3;
}
```
- `AST_BLOCK` node points to the `statement1`
- `statement1` points to `statement2` via `next`
- `statement2` points to `statement3` via `next`
- `statement1` points to `NULL`, indicating end of the sequence
- 
### 6. **Refactored Statement Parsing**

The `parse_statement()` function now includes cases for handling newly introduced statements as well as more variable types for variable declaration:
```cpp
else if (match(TOKEN_LBRACE))   return parse_block();
else if (match(TOKEN_IF))   return parse_if_statement();
else if (match(TOKEN_WHILE))    return parse_while_statement();
else if (match(TOKEN_REPEAT))   return parse_repeat_statement();
else if (match(TOKEN_PRINT))    return parse_print_statement();
else if (match(TOKEN_FACTORIAL))    return parse_factorial();
else if (match(TOKEN_OPERATOR)) return parse_binop();
```
This ensures that the parser correctly identifies and processes different statement types.

### 7. **AST Printing**

Updated `print_ast()` to handle the newly added AST node types:
```cpp
case AST_IF: printf("If statement\n"); break;
case AST_WHILE: printf("While loop\n"); break;
case AST_REPEAT: printf("Repeat-Until loop\n"); break;
case AST_BLOCK: printf("Block\n"); break;
case AST_BINOP: printf("BinaryOp: %s\n", node->token.lexeme); break;
```
This makes debugging easier by providing a clearer representation of the AST.

### 8. **New Token Types**

Updated `tokens.h` to include more specific token types.
 -   **Terminals**
    - `TOKEN_STRING_LITERAL` 
    - `TOKEN_COMPARISON`
 -   **Delimiters**
    - `TOKEN_LBRACK` 
    - `TOKEN_RBRACK`
 -   **Keywords**
    - `TOKEN_ELSE`
    - `TOKEN_REPEAT`
    - `TOKEN_UNTIL`
    - `TOKEN_FOR`
    - `TOKEN_WHILE`
    - `TOKEN_BREAK`
    - `TOKEN_RETURN`
    - `TOKEN_FACTORIAL`
    - `TOKEN_VOID`
    - `TOKEN_CONST`
    - `TOKEN_FLOAT`
    - `TOKEN_CHAR`

### 9. **Expanded Test Cases**

Additional test cases added to validate new functionality:
- **Valid input tests** for control structures, expressions, and function calls.
- **Invalid input tests** for missing semicolons, incorrect parentheses, and malformed statements.
- **Reading input from a file** for larger and more complex test cases.


