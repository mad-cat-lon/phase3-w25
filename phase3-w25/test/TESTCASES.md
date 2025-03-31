# Semantic Analyzer Test Cases

### Variable Checking

#### 1. Basic variable declaration

Input:
```c
int x;
```
Expected Output:
```
Semantic analysis successful. No errors found.
```

#### 2. Undeclared variable usage (ERROR)

Input:
```c
y = 5;
```
Expected Output:
```
Semantic Error at line 1: Undeclared variable 'y'
Semantic analysis failed. Errors detected.
```

#### 3. Variable redeclaration (ERROR)

Input:
```c
int x;
int x;
```
Expected Output:
```
Semantic Error at line 2: Variable 'x' already declared in this scope
Semantic analysis failed. Errors detected.
```

---

### Type Checking

#### 1. Implicit type conversion

Input:
```c
int x;
char test;
test = "123";
x = test + 1;
```
Expected Output:
```
Semantic analysis successful. No errors found.
```

#### 2. Compatible variable type assignment 

Assignment from char -> int

Input:
```c
int x;
char expr;
expr = "123";
x = expr;
```
Expected Output:
```
Semantic analysis successful. No errors found.
```

#### 3. Incompatible variable type assignment (ERROR)

Assignment from int -> char

Input:
```c
char x;
int expr;
expr = 123;
x = expr;
```
Expected Output:
```
Semantic Error at line 4: Type mismatch involving 'x'
Semantic analysis failed. Errors detected.
```

#### 4. Incompatable operator for type (ERROR)

Input:
```c
char x;
int y;
x = "hello";
y = 40;
y = x < y;
```
Expected Output:
```
Semantic Error at line 5: Invalid operation involving '<'
Semantic analysis failed. Errors detected.
```

---

### Scope Analysis

#### 1. Basic variable tracking (ERROR)

Input:
```c
if (1) {
    int y;
    y = 10;
}
print y;
```
Expected Output:
```
Semantic Error at line 5: Undeclared variable 'y'
Semantic analysis failed. Errors detected.
```

---

### Flow Control Validation

#### 1. If/while condition validation

Input:
```c
if ("hello") {
    print "bye";
}
```
Expected Output:
```
Semantic Error at line 1: Invalid condition involving 'if statement'
Semantic analysis failed. Errors detected.
```
---

### Special Feature Validation

#### 1. Factorial function calls

Input:
```c
factorial(5 < 3);
```
Expected Output:
```
Semantic Error at line 1: Invalid parameter(s) involving 'factorial statement'
Semantic analysis failed. Errors detected.
```



