%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 100
#define INITIAL_ADDRESS 1000

typedef struct {
    char name[50];
    char type[20];
    char value[50];
    int address;
    int size;
} Symbol;

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
int currentAddress = INITIAL_ADDRESS;
char lastIdentifier[50] = "";
char currentType[20] = "";

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "int",
    "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
    "while"
};
const int num_keywords = sizeof(keywords) / sizeof(keywords[0]);
int is_keyword(const char *identifier) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(identifier, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int findSymbolIndex(const char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void addSymbol(const char *name, const char *type, const char *value, int address, int size) {
    int index = findSymbolIndex(name);
    if (index == -1) {
        if (symbolCount < MAX_SYMBOLS) {
            strcpy(symbolTable[symbolCount].name, name);
            strcpy(symbolTable[symbolCount].type, type);
            strcpy(symbolTable[symbolCount].value, value);
            symbolTable[symbolCount].address = address;
            symbolTable[symbolCount].size = size;
            symbolCount++;
        } else {
            printf("Symbol table is full.\n");
        }
    }
}

void printSymbolTable() {
    printf("\nSymbol Table:\n\n");
    printf("---------------------------------------------------------------\n");
    printf("| %-15s | %-10s | %-10s | %-8s | %-4s |\n", "Name", "Type", "Value", "Address", "Size");
    printf("|-----------------|------------|------------|----------|------|\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("| %-15s | %-10s | %-10s | %-8d | %-4d |\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].value,
               symbolTable[i].address,
               symbolTable[i].size);
    }
    printf("---------------------------------------------------------------\n");
}

int is_keyword(const char *identifier) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(identifier, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int findSymbolIndex(const char *name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void addSymbol(const char *name, const char *type, const char *value, int address, int size) {
    int index = findSymbolIndex(name);
    if (index == -1) {
        if (symbolCount < MAX_SYMBOLS) {
            strcpy(symbolTable[symbolCount].name, name);
            strcpy(symbolTable[symbolCount].type, type);
            strcpy(symbolTable[symbolCount].value, value);
            symbolTable[symbolCount].address = address;
            symbolTable[symbolCount].size = size;
            symbolCount++;
        } else {
            printf("Symbol table is full.\n");
        }
    }
}

void printSymbolTable() {
    printf("\nSymbol Table:\n\n");
    printf("---------------------------------------------------------------\n");
    printf("| %-15s | %-10s | %-10s | %-8s | %-4s |\n", "Name", "Type", "Value", "Address", "Size");
    printf("|-----------------|------------|------------|----------|------|\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("| %-15s | %-10s | %-10s | %-8d | %-4d |\n",
               symbolTable[i].name,
               symbolTable[i].type,
               symbolTable[i].value,
               symbolTable[i].address,
               symbolTable[i].size);
    }
    printf("---------------------------------------------------------------\n");
}

int hasUnclosedComment = 0;
%}

%%

^\s*#\s*(include|define|undef|if|ifdef|ifndef|else|elif|endif|error|pragma)\s*(<[a-z]+>|\"[a-z]+\"|.*) {
    printf("Preprocessor Directive\t: %s\n", yytext);
}

\/\/[^\n]* {
    printf("Single Line Comment\t\t: %s\n", yytext);
}

\/\*([^*]|\*+[^*/])*\*+\/ {
    printf("Multi-Line Comment\t\t: %s\n", yytext);
    hasUnclosedComment = 0;
}

\/\*([^*]|\*+[^*/])* {
    hasUnclosedComment = 1;
}

(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while) {
    strcpy(currentType, yytext);
    printf("Keyword\t\t\t: %s\n", yytext);
}

[a-zA-Z_][a-zA-Z0-9_]*\s*\([^{}></=]*\)\s*; {
    printf("Function Call\t\t: %s\n", yytext);
}

[0-9]+\.[0-9]+ {
    printf("Double Constant\t\t: %s\n", yytext);
}

[0-9]+ {
    printf("Integer Constant\t\t: %s\n", yytext);
}

\'(\\.|[^\\\'])\' {
    printf("Character Constant\t: %s\n", yytext);
}

\"([^\"\\]|\\.)*\" {
    printf("String Constant\t\t: %s\n", yytext);
}

[a-zA-Z_][a-zA-Z0-9_]*\s*\[\s*[0-9]+\s*\]\s*;? {
    // Finds the size of the identifier
    char identifier[20];
    int arraySize = 1;

    sscanf(yytext, "%[^[;[]", identifier);
    sscanf(yytext, "%*[^[][%d]", &arraySize);

    if (strcmp(currentType, "") != 0) {
        int size;
        if (strcmp(currentType, "int") == 0) {
            size = arraySize * sizeof(int);
        } else if (strcmp(currentType, "float") == 0) {
            size = arraySize * sizeof(float);
        } else if (strcmp(currentType, "double") == 0) {
            size = arraySize * sizeof(double);
        } else if (strcmp(currentType, "char") == 0) {
            size = arraySize * sizeof(char);
        } else {
            size = arraySize * sizeof(void);
        }
        
        addSymbol(identifier, currentType, "-", currentAddress, size);
        currentAddress += size; 
        strcpy(currentType, ""); 
        strcpy(lastIdentifier, identifier);
    }
}

[a-zA-Z_][a-zA-Z0-9_]*\s*[\[\];(]? {
    // Excludes function names
    char identifier[20];
    sscanf(yytext, "%[^[;(]", identifier);
    if (!is_keyword(identifier)) {
        printf("Valid Identifier\t\t: %s\n", identifier);
        sscanf(yytext, "%[^[;]", lastIdentifier);

        if (strcmp(currentType, "") != 0 && lastIdentifier[strlen(lastIdentifier)-1] != '(') {
            int size;
            if (strcmp(currentType, "int") == 0) {
                size = sizeof(int);
            } else if (strcmp(currentType, "float") == 0) {
                size = sizeof(float);
            } else if (strcmp(currentType, "double") == 0) {
                size = sizeof(double);
            } else if (strcmp(currentType, "char") == 0) {
                size = sizeof(char);
            } else {
                size = sizeof(void);
            }

            addSymbol(identifier, currentType, "-", currentAddress, size);
            currentAddress += size; 
            strcpy(currentType, ""); 
            strcpy(lastIdentifier, identifier);
        }
    }
}

(=)\s*([^;,]+)[;,] {
    char assignedValue[50];
    sscanf(yytext, "=%s", assignedValue);
    int len = strlen(assignedValue);
    if (assignedValue[len-1] == ';' || assignedValue[len-1] == ',') {
        assignedValue[len-1] = '\0';
    }
    
    int index = findSymbolIndex(lastIdentifier);
    if (index != -1 && strcmp(symbolTable[index].value, "-") == 0) {
        strcpy(symbolTable[index].value, assignedValue);
    }
}

("+"|"*"|"-"|"/"|"%")(=) {
    printf("Arithmetic Assignment Operator : %s\n", yytext);
}

("+"|"*"|"-"|"/"|"%") {
    printf("Arithmetic Operator\t\t: %s\n", yytext);
}

(>|<|<=|>=|!=|==) {
    printf("Relational Operator\t\t: %s\n", yytext);
}

("&&"|"||"|"!") {
    printf("Logical Operator\t\t: %s\n", yytext);
}

(\^|&|\||<<|>>) {
    printf("Bitwise Operator\t\t: %s\n", yytext);
}

("++"|"--"|"-"[a-z]+) {
    printf("Unary Operator\t\t\t: %s\n", yytext);
}

(;|,|\.|\[|\]|\(|\)|\{|\}) {
    printf("Special Character\t\t: %s\n", yytext);
}

%%

int main(int argc, char *argv[]) {
    yyin = fopen("program.c", "r");
    if (yyin == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    yylex();
    if (hasUnclosedComment) {
        printf("Error: Unclosed multi-line comment.\n");
    }
    printSymbolTable();
    fclose(yyin);
    return 0;
}
