## Error Code Documentation

### Overview
This document provides a reference for new error codes encountered during parsing. 

### Error Codes

| Error Code | Description | Cause | Example | Solution |
|------------|-------------|-------|---------|------------|
| **PARSE_ERROR_INVALID_STATEMENT** | Invalid statement encountered. | The parser detected an incorrect sequence of tokens. | ``` if (x == 5) ! ``` Missing block/statement. | Use proper syntax for statements. |
| **PARSE_ERROR_MISSING_LPAREN** | Expected `(` after a specific token. | A function or condition is missing an opening parenthesis. | ``` if x > 5) ``` Missing `(` before `x`. | Ensure conditions and function calls start with `(`. |
| **PARSE_ERROR_MISSING_RPAREN** | Expected `)` after a specific token. | A function or condition is missing a closing parenthesis. | ``` if (x > 5 ``` Missing `)` after `5`. | Ensure all opened parentheses are properly closed. |
| **PARSE_ERROR_MISSING_LBRACE** | Expected `{` after a specific token. | A block of code is missing an opening brace. | ``` if (x > 5) return x; ``` Missing `{}` for multi-line blocks. | Use `{}` to define code blocks properly. |
| **PARSE_ERROR_MISSING_RBRACE** | Expected `}` after a specific token. | A block of code is missing a closing brace. | ``` { int x = 5; ``` Missing `}` at the end. | Ensure all opened braces `{` have a matching closing brace `}`. |
| **PARSE_ERROR_MISSING_LBRACK** | Expected '[' after a specific token. | An array declaration or index operation is missing `[` . | ``` int arr 5]; ``` Missing `[` before `5]`. | Use `[` correctly in array declarations and indexing. |
| **PARSE_ERROR_MISSING_RBRACK** | Expected ']' after a specific token. | An array declaration or index operation is missing `]` . | ``` int arr[5; ``` Missing `]` after `5`. | Ensure array brackets are properly closed. |
| **PARSE_ERROR_MISSING_UNTIL** | Expected 'until' after a specific token. | A loop or conditional statement requires 'until' but it is missing. | ``` repeat { x = x + 5; } (x == 5)``` Missing `until` keyword. | Ensure that `until` is properly included in loop constructs. |
| **PARSE_ERROR_INVALID_COMPARISON** | Invalid comparison operation found. | A relational operation was incorrectly formatted. | ``` if (x =+ 5) ``` `=+` is invalid. | Use valid comparison operators such as `==`, `!=`, `<=`, `>=`. |


