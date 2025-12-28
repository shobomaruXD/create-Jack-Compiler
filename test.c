#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SymbolTable.h"

#define N 128

typedef struct{
    char type[N]; // keyword,symbol,integerConstant,stringConstant,identifier 
    char value[N];
}Token;

char* test(const char* str, const char* target, const char* replacement) {
    const char* pos = strstr(str, target); 
    int target_len = strlen(target);
    int replacement_len = strlen(replacement);
    int new_len = strlen(str) - target_len + replacement_len;

    char* result = malloc(new_len + 1);  // +1 for '\0'

    int prefix_len = pos - str;
    strncpy(result, str, prefix_len);                   // 前半部分コピー
    strcpy(result + prefix_len, replacement);           // 置換文字列コピー

    return result;
}

int main() {
    

    return 0;
}

void printSymbolTable(SymbolTable *table) {
    printf("\n--- SYMBOL TABLE DEBUG START ---\n");

    // 1. クラススコープ（static/field）の表示
    printf("[Class Scope]\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol *current = table->classScope[i];
        while (current != NULL) {
            printf("  Index %3d: [Name] %-10s [Type] %-8s [Kind] %-8s [VM Index] %d\n",
                   i, current->name, current->type, current->kind, current->index);
            current = current->next;
        }
    }

    printf("--------------------------------\n");

    // 2. サブルーチンスコープ（argument/var）の表示
    printf("[Subroutine Scope]\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol *current = table->subroutineScope[i];
        while (current != NULL) {
            printf("  Index %3d: [Name] %-10s [Type] %-8s [Kind] %-8s [VM Index] %d\n",
                   i, current->name, current->type, current->kind, current->index);
            current = current->next;
        }
    }

    printf("--- SYMBOL TABLE DEBUG END ---\n\n");
}