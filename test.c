#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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