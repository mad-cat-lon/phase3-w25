/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
static char last_token_type = 'x'; // For checking consecutive operators

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
    printf("Lexical Error at line %d: ", line);
    switch (error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_UNKNOWN_ESCAPE_SEQUENCE:
            printf("Unknown escape sequence\n");
            break;
        case ERROR_UNTERMINATED_STRING:
            printf("Unterminated string\n");
        default:
            printf("Unknown error\n");
    }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type) {
        case TOKEN_STRING_LITERAL: printf("STRING"); break;
        case TOKEN_NUMBER:      printf("NUMBER"); break;
        case TOKEN_OPERATOR:    printf("OPERATOR"); break;
        case TOKEN_COMPARISON:  printf("COMPARISON"); break;
        case TOKEN_IDENTIFIER:  printf("IDENTIFIER"); break;
        case TOKEN_EQUALS:      printf("EQUALS"); break;
        case TOKEN_SEMICOLON:   printf("SEMICOLON"); break;
        case TOKEN_LPAREN:      printf("LPAREN"); break;
        case TOKEN_RPAREN:      printf("RPAREN"); break;
        case TOKEN_LBRACE:      printf("LBRACE"); break;
        case TOKEN_RBRACE:      printf("RBRACE"); break;
        case TOKEN_LBRACK:      printf("LBRACK"); break;
        case TOKEN_RBRACK:      printf("RBRACK"); break;
        case TOKEN_IF:          printf("IF"); break;
        case TOKEN_ELSE:        printf("ELSE"); break;
        case TOKEN_REPEAT:      printf("REPEAT"); break;
        case TOKEN_UNTIL:       printf("UNTIL"); break;
        case TOKEN_FOR:         printf("FOR"); break;
        case TOKEN_BREAK:       printf("BREAK"); break;
        case TOKEN_VOID:        printf("VOID"); break;
        case TOKEN_CONST:       printf("CONST"); break;
        case TOKEN_INT:         printf("INT"); break;
        case TOKEN_FLOAT:       printf("FLOAT"); break;
        case TOKEN_CHAR:        printf("CHAR"); break;
        case TOKEN_PRINT:       printf("PRINT"); break;
        case TOKEN_FACTORIAL:   printf("FACTORIAL"); break;
        case TOKEN_EOF:         printf("EOF"); break;
        default:                printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n",
           token.lexeme, token.line);
}

/* check if string is a keyword */
int is_keyword(const char *str) {
    /* You can expand this list as needed. */
    // Control flow keywords
    if (strcmp(str, "if") == 0)         return TOKEN_IF;
    if (strcmp(str, "else") == 0)       return TOKEN_ELSE;
    if (strcmp(str, "repeat") == 0)     return TOKEN_REPEAT;
    if (strcmp(str, "until") == 0)      return TOKEN_UNTIL;
    if (strcmp(str, "for") == 0)        return TOKEN_FOR;
    if (strcmp(str, "while") == 0)      return TOKEN_WHILE;
    if (strcmp(str, "break") == 0)      return TOKEN_BREAK;
    // Function & scope Keywords
    if (strcmp(str, "print") == 0)      return TOKEN_PRINT;
    if (strcmp(str, "factorial") == 0)  return TOKEN_FACTORIAL;
    if (strcmp(str, "return") == 0)     return TOKEN_RETURN;
    if (strcmp(str, "void") == 0)       return TOKEN_VOID;
    // Variable & data type keywords
    if (strcmp(str, "int") == 0)        return TOKEN_INT;
    if (strcmp(str, "float") == 0)      return TOKEN_FLOAT;
    if (strcmp(str, "char") == 0)       return TOKEN_CHAR;
    if (strcmp(str, "const") == 0)      return TOKEN_CONST;
    return 0; 
}

/* check if character is valid delimiter */
int is_delimiter(char c) {
    switch (c) {
        case ';':   return TOKEN_SEMICOLON;
        case '(':   return TOKEN_LPAREN;
        case ')':   return TOKEN_RPAREN;
        case '{':   return TOKEN_LBRACE;
        case '}':   return TOKEN_RBRACE;
        case '[':   return TOKEN_LBRACK;
        case ']':   return TOKEN_RBRACK;
        default:    return 0;
            
    }
}

/* Check if character could be part of an operator */
int is_operator_char(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
            return TOKEN_OPERATOR;
        case '<':
        case '>':
        case '!':
            return TOKEN_COMPARISON;
        case '=':
            return TOKEN_EQUALS;
        default:
            return 0;
    }
}


/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    int idx = 0;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
            token.line = current_line;
        }
        (*pos)++;
    }

    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // TODO: Add comment handling here
    if (c == '/' && input[*pos + 1] == '/') {
        // single-line comment, skip until newline or end of input
        (*pos) += 2;
        c = input[*pos];

        while (c != '\0' && c != '\n') {
            (*pos)++;
            c = input[*pos];
        }
        // recurse here to get the next toke
        return get_next_token(input, pos);
    }
    if (c == '/' && input[*pos + 1] == '*') {
        // multi-line comment, skip until */ or end of input
        (*pos) += 2;
        c = input[*pos];

        while (c != '\0' && !(c == '*' && input[*pos + 1] == '/')) {
            (*pos)++;
            c = input[*pos];
        }
        if (c != '\0') {
            // skip closing */
            (*pos) += 2;
        }
        // recurse to get next token
        return get_next_token(input, pos);
    }

    // Handle numbers
    if (isdigit(c)) {
        while (isdigit(c)) {
            token.lexeme[idx++] = c;
            (*pos)++;
            c = input[*pos];
            if (idx >= (int)sizeof(token.lexeme) - 1) break;
        }
        token.lexeme[idx] = '\0';
        // Optional: check next char to ensure valid number format
        // e.g., 123abc => ERROR_INVALID_NUMBER
        if (isalpha(c)) {
            token.type = TOKEN_ERROR;
            token.error = ERROR_INVALID_NUMBER;
        } else {
            token.type = TOKEN_NUMBER;
        }
        return token;
    }

    // TODO: Add keyword and identifier handling here
    // Hint: You'll have to add support for keywords and identifiers, and then string literals
    if (isalpha(c) || c == '_') {
        while (isalnum(c) || c == '_') {
            token.lexeme[idx++] = c;
            (*pos)++;
            c = input[*pos];
            if (idx >= (int)sizeof(token.lexeme) - 1) break;
        }
        token.lexeme[idx] = '\0';

        // Check if it's a keyword
        TokenType keyword_type = is_keyword(token.lexeme);
        if (keyword_type) {
            token.type = keyword_type;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        return token;
    }
    // TODO: Add string literal handling here
    if (c == '"') {
        (*pos)++; // skip opening quote
        c = input[*pos];
        
        while (c != '\0' && c != '"') {
            // handle escape sequences like \n, \t, etc
            if (c == '\\') {
                (*pos)++;
                c = input[*pos];
                switch (c) {
                    case 'n':
                        token.lexeme[idx++] = '\n';
                        break;
                    case 't':
                        token.lexeme[idx++] = '\t';
                        break;
                    case '\\':
                        token.lexeme[idx++] = '\\';
                        break;
                    case '"':
                        token.lexeme[idx++] = '"';
                        break;
                    default:
                        token.error = ERROR_UNKNOWN_ESCAPE_SEQUENCE;
                        token.lexeme[idx++] = c;
                }
            } else {
                token.lexeme[idx++] = c;
            }
            (*pos)++;
            c = input[*pos];
            if (idx >= (int)sizeof(token.lexeme) - 1) break; // Avoid overflow
        }
        if (c == '"') {
            // Found closing quote
            (*pos)++; // skip closing quote
            token.lexeme[idx] = '\0';
            token.type = TOKEN_STRING_LITERAL;
        } else {
            // Unterminated string
            token.type = TOKEN_ERROR;
            token.error = ERROR_UNTERMINATED_STRING;
            token.lexeme[idx] = '\0';
        }
        return token;
    }
    // Handle operators
    TokenType operator_type = is_operator_char(c);
    if (operator_type) {
        token.type = operator_type;
        token.lexeme[idx++] = c;
        (*pos)++;
        char next_c = input[*pos];

        // raise error for consecutive operators
        if ((c == '+' || c == '-' || c == '*' || c == '/') && (next_c == '+' || next_c == '-' || next_c == '*' || next_c == '/')) {
            token.type = TOKEN_ERROR;
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            return token;
        }
        /* Check for double-character operators (==, !=, etc.) */
        if (
            (c == '=' && next_c == '=') ||
            (c == '!' && next_c == '=') ||
            (c == '|' && next_c == '|') ||
            (c == '<' && next_c == '=') ||
            (c == '>' && next_c == '=') ) {
            token.lexeme[idx++] = next_c;
            (*pos)++;
        }

        if (c == '=' && next_c == '=') {
            token.type = TOKEN_COMPARISON; // or else is token_equals by default
        }

        token.lexeme[idx] = '\0';
        return token;
    }
    // TODO: Add delimiter handling here
    TokenType delimeter_type = is_delimiter(c);
    if (delimeter_type) {
        token.type = delimeter_type;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }

    // Handle invalid characters
    token.type = TOKEN_ERROR;
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

// int main() {
//     const char *input = 
//     "123\n"
//     "if repeat until\n"
//     "variable_name xYz_123\n"
//     "+ - * / = == != <= >=\n"
//     "\"Hello, World!\"\n"
//     "{ } ( ) [ ] ;\n"
//     "// This is a comment\n"
//     "123\n"
//     "/* Multi-line\n"
//     "   comment */\n"
//     "456\n"
//     "$\n"
//     "123abc\n"
//     "1++2\n"
//     "\"Hello\\xWorld\"\n"
//     "\"This is unterminated\n";
//     int position = 0;
//     Token token;

//     printf("Analyzing input:\n%s\n\n", input);

//     do {
//         token = get_next_token(input, &position);
//         print_token(token);
//     } while (token.type != TOKEN_EOF);

//     return 0;
// }
