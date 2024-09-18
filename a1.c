#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct symbol_table {
    char identifier[50];
    char type[10];
    int bytes;
    int address;
    char value[10];
} symbols[100];

int symbol_count = 0;
int memory_address = 1000;

// Function to check if a string is a Keyword : 
int isKeyword(const char *str) {
    const char *keywords[] = {
        "auto", "break", "case", "char", "const", "continue", "default", "do",
        "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
        "long", "register", "return", "short", "signed", "sizeof", "static",
        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
        "while"
    };
    size_t n = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < n; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a string is a Inbuilt Function Call : 
int isFunction(const char *str) {
    const char *functions[] = {"printf", "scanf", "getch", "main", "clrscr"};
    size_t n = sizeof(functions) / sizeof(functions[0]);
    for (size_t i = 0; i < n; i++) {
        if (strcmp(str, functions[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a character is an Arithmetic Operator :
int isArithmeticOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%';
}

// Function to check if a string is an Unary Operator :
int isUnaryOperator(const char *str) {
    const char *unary_operators[] = {"-", "++", "--"};
    size_t n = sizeof(unary_operators) / sizeof(unary_operators[0]);
    for (size_t i = 0; i < n; i++) {
        if (strcmp(str, unary_operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to check if a character is a Special Character :
int isSpecialCharacter(char ch) {
    return ch == ';' || ch == ',' || ch == '.' || ch == '[' || ch == ']' ||
           ch == '(' || ch == ')' || ch == '{' || ch == '}';
}

// Function to check if a string is a Preprocessor Directive :
int isPreprocessorDirective(const char *str) {
    const char *directives[] = {
        "include", "define", "ifdef", "ifndef", "endif", "if", "else", "elif", "undef", "pragma"
    };
    size_t n = sizeof(directives) / sizeof(directives[0]);
    for (size_t i = 0; i < n; i++) {
        if (strcmp(str, directives[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to add an Identifier to the symbol table :
void addSymbol(const char *name, const char *type, char *value) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbols[i].identifier, name) == 0) {
            return;
        }
    }

    strcpy(symbols[symbol_count].identifier, name);
    strcpy(symbols[symbol_count].type, type);
    symbols[symbol_count].bytes = (strcmp(type, "int") == 0) ? sizeof(int) :
                                  (strcmp(type, "char") == 0) ? sizeof(char) :
                                  (strcmp(type, "float") == 0) ? sizeof(float) :
                                  (strcmp(type, "double") == 0) ? sizeof(double) :
                                  0;
    symbols[symbol_count].address = memory_address;
    strcpy(symbols[symbol_count].value, value);

    memory_address += symbols[symbol_count].bytes; 
    symbol_count++;
}

void reverse(char *str) {
    int n = strlen(str);
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

// Function to parse the program character by character :
void lexicalParserAnalyser(char *contents) {
    size_t length = strlen(contents);
    size_t i = 0;

    int quote_open = 0;
    int comment_open = 0;
    int bracket_count = 0;

    while (i < length) {
        if (isspace(contents[i])) {
            i++;
            continue;
        }

        // Preprocessor directive :
        if (contents[i] == '#') {
            char buffer[100];
            size_t j = 0;
            i++;
            if (strncmp(&contents[i], "include", 7) == 0) {
                i += 7;
                while (i < length && contents[i] != '\n') {
                    buffer[j++] = contents[i++];
                }
                buffer[j] = '\0';
                printf("Preprocessor Directive: \t - #include%s\n", buffer);
            }
        } 
        
        // Skip string literals :
        else if (contents[i] == '"') {
            quote_open = !quote_open; 
            i++;
            while (i < length && contents[i] != '"') {
                i++;
            }
            if (i < length) {
                i++; 
                quote_open = 0;
            }
        } else if (contents[i] == '/') {

            // Single-line comment :
            if (i + 1 < length && contents[i + 1] == '/') {
                i += 2;
                printf("Single line Comment : \t\t - ");
                while (i < length && contents[i] != '\n') {
                    printf("%c", contents[i]);
                    i++;
                }
                printf("\n");
            } 
            
            // Multi-line comment :
            else if (i + 1 < length && contents[i + 1] == '*') {
                comment_open = 1;
                i += 2;
                printf("Multi-line Comment : \t\t - \n");
                while (i + 1 < length && !(contents[i] == '*' && contents[i + 1] == '/')) {
                    printf("%c", contents[i]);
                    i++;
                }
                printf("\n");
                if (i + 1 < length) {
                    i += 2;
                    comment_open = 0;
                }
            } else {
                i++;
            }
        } 
        
        // Integer Constant (hex) : 
        else if (contents[i] == '0' && contents[i + 1] == 'x'){
            char buffer[10];
            size_t j = 0;

            buffer[j++] = contents[i++];
            buffer[j++] = contents[i++]; 
            while (i < length && (isxdigit(contents[i]))) {
                buffer[j++] = contents[i++];
            }
            buffer[j] = '\0';
            printf("Hexadecimal Integer Constant : \t\t - %s\n", buffer);
        }

        // Double Constant : 
        else if (isdigit(contents[i])) {
            char buffer[10];
            size_t j = 0;

            while (i < length && isdigit(contents[i])) {
                buffer[j++] = contents[i++];
            }

            if (contents[i] == '.' && isdigit(contents[i+1])){
                buffer[j++] = contents[i++];
                while (i < length && isdigit(contents[i])) {
                    buffer[j++] = contents[i++];
                }
                buffer[j] = '\0';
                printf("Double Constant : \t\t - %s\n", buffer);
            }

            // Integer Constant (decimal) :
            else{
                buffer[j] = '\0';
                printf("Integer Constant : \t\t - %s\n", buffer);
            }
        } 

        // Identifier or keyword or function call : 
        else if (isalpha(contents[i]) || contents[i] == '_') {
            char buffer[100];
            size_t j = 0;
            while (i < length && (isalnum(contents[i]) || contents[i] == '_')) {
                buffer[j++] = contents[i++];
            }
            buffer[j] = '\0';

            // Keyword : 
            if (isKeyword(buffer)) {
                printf("Keyword: \t\t\t - %s\n", buffer);
            } 
            
            // Function Call : 
            else if (isFunction(buffer)) {
                if (i < length && contents[i] == '(') {
                    printf("Function Call: \t\t\t - %s(", buffer);
                    i++; 
                    bracket_count++;
                    while (i < length && contents[i] != ')') {
                        putchar(contents[i++]);
                    }
                    if (i < length) {
                        i++;
                        bracket_count--;
                    }
                    printf(")\n");
                }
            } 
            
            // Identifier :
            else {
                char identifier[50];
                strcpy(identifier, buffer);
                
                // Type of the identifier :
                char type[10] = "int";
                int k = i - strlen(identifier) - 1;
                while (k >= 0 && isspace(contents[k])) k--; 
                if (k >= 0 && isalpha(contents[k])) {
                    size_t t = 0;
                    char type_buffer[100];
                    while (k >= 0 && isalpha(contents[k])) {
                        type_buffer[t++] = contents[k--];
                    }
                    type_buffer[t] = '\0';
                    reverse(type_buffer);
                    strcpy(type, type_buffer);
                }

                // Value of the identifier : 
                char value_buffer[10] = "0";
                int index = i;
                while (index<length && isspace(contents[index])){
                        index++;
                }
                if (index < length && contents[index] == '=') {
                    index++;
                    size_t v = 0;
                    while (index<length && isspace(contents[index])){
                        index++;
                    }

                    while (index < length && contents[index]!=';' && contents[index]!=',' && !isspace(contents[index])) {
                        value_buffer[v++] = contents[index++];
                    }
                    value_buffer[v] = '\0';
                }

                printf("Identifier: \t\t\t - %s\n", identifier);

                addSymbol(identifier, type, value_buffer);  // Add to symbol table
            }
        } 
        
        // Arithmetic operator :
        else if (isArithmeticOperator(contents[i]) && !isArithmeticOperator(contents[i+1])) {
            printf("Arithmetic Operator: \t\t - %c\n", contents[i]);
            i++;
        } 
        
        // Relational operator :
        else if (contents[i] == '<' || contents[i] == '>') {
            printf("Relational Operator: \t\t - %c\n", contents[i]);
            if ((contents[i] == '<' && contents[i+1] == '=') || (contents[i] == '>' && contents[i+1] == '=') || (contents[i] == '=' && contents[i+1] == '=') || (contents[i] == '!' && contents[i+1] == '=')){
                printf("Relational Operator: \t\t - %c%c\n", contents[i], contents[i+1]);
                i++;
            } 
            i++;
        } 
        
        // Logical operator :
        else if (contents[i] == '!') {
            printf("Logical Operator: \t\t - %c\n", contents[i]);
            if ((contents[i] == '&' && contents[i+1] == '&') || (contents[i] == '|' && contents[i+1] == '|')){
                printf("Logical Operator: \t\t - %c%c\n", contents[i], contents[i+1]);
                i++;
            }
            i++;
        } 
        
        // Bitwise operator :
        else if (contents[i] == '^' || contents[i] == '&' || contents[i] == '|'){
            printf("Bitwise Operator: \t\t - %c\n", contents[i]);
            if (contents[i] == '<' && contents[i+1] == '<'){
                printf("Bitwise Operator: \t\t - %c%c\n", contents[i], contents[i+1]);
                i++;
            }
            else if (contents[i] == '>' && contents[i+1] == '>'){
                printf("Bitwise Operator: \t\t - %c%c\n", contents[i], contents[i+1]);
                i++;
            }
            i++;
        } 
        
        // Arithmetic Assignment operator :
        else if (isArithmeticOperator(contents[i]) && contents[i+1] == '=') {
            printf("Arithmetic Assignment : \t\t - %c%c\n", contents[i], contents[i+1]);
            i += 2;
        } else if (isUnaryOperator((char[]){contents[i], contents[i + 1], '\0'})) {
            printf("Unary Operator: \t\t - %c%c\n", contents[i], contents[i + 1]);
            i += 2;
        } 

        // Assignment operator :
        else if (contents[i] == '=') {
            printf("Assignment operator : \t\t - %c\n", contents[i]);
            i++;
        } 
        
        // Special character :
        else if (isSpecialCharacter(contents[i])) {
            printf("Special Character: \t\t - %c\n", contents[i]);
            i++;
        } else {
            i++;
        }
    }

    // Error handling for unclosed quotes, comments, or brackets : 
    if (quote_open) {
        fprintf(stderr, "SyntaxError: Unclosed string quote.\n");
    }
    if (comment_open) {
        fprintf(stderr, "SyntaxError: Unclosed multi-line comment.\n");
    }
    if (bracket_count != 0) {
        fprintf(stderr, "SyntaxError: Unmatched brackets.\n");
    }

    // Display the symbol table :
    printf("\n----------------------------------SYMBOL TABLE---------------------------------\n\n");
    
    printf("\nIdentifier\tType\tBytes\tAddress\tValue\n");
    printf("------------------------------------------------\n");
    for (int k = 0; k < symbol_count; k++) {
        printf("|%s\t\t%s\t%d\t%d\t%s\t|\n", symbols[k].identifier, symbols[k].type,
               symbols[k].bytes, symbols[k].address, symbols[k].value);
        printf("------------------------------------------------\n");
    }
}

int main() {
    char file_content[1000] = {0};
    char line[100];
    FILE *fptr;

    fptr = fopen("program.c", "r");
    if (fptr == NULL) {
        fprintf(stderr, "\nError in opening the file\n");
        exit(1);
    }
    while (fgets(line, sizeof(line), fptr) != NULL) {
        strcat(file_content, line);
    }
    fclose(fptr);

    printf("-----------------------------------TOKENS------------------------------------\n\n");
    lexicalParserAnalyser(file_content);
    return 0;
}
