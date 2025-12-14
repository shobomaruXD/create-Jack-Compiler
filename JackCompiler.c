#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Structs.h"
#include "SyntaxAnalyser.h"
#include "CodeGenerator.h"
#include "SymbolTable.h"

#define LINE 256
#define GIGA 1024
#define MAX_TOKENS 10000

void trim(char *str){
    char *start=str;
    while(isspace((unsigned char)*start)) start++;

    char *end=start+strlen(start)-1;
    while(end>start && isspace((unsigned char)*end)){
        *end='\0';
        end--;
    }

    if(start!=str)
        memmove(str,start,strlen(start)+1);
}

void remove_comments(char *line,int *in_block_comment){
    char *p=line;
    char *write=line;
    while(*p!='\0'){
        if(*in_block_comment){
            if(p[0]=='*' && p[1]=='/'){
                *in_block_comment=0;
                p+=2;
            }else{
                p++;
            }
        }else if(p[0]=='/' && p[1]=='*'){
            *in_block_comment=1;
            p+=2;
        }else if(p[0]=='/' && p[1]=='/'){
            break; // 行コメント：ここで終わり
        }else{
            *write++ = *p++;
        }
    }
    *write='\0'; // 終端文字を付ける
}

char* replace_extension(char* str,char* target,char* replacement){
    char* pos=strstr(str,target); 
    int target_len=strlen(target);
    int replacement_len=strlen(replacement);
    int new_len=strlen(str)-target_len+replacement_len;

    char* result=malloc(new_len+1); // mallocはメモリの確保、+1は'\0'(終端文字)のため freeでメモリ解放

    int prefix_len=pos-str;
    strncpy(result,str,prefix_len); // 前半部分コピー
    strcpy(result+prefix_len,replacement); // 置換文字列コピー

    return result;
}

void addToken(Token tokens[],int *count,char *type,char *value){
    strcpy(tokens[*count].type,type);
    strcpy(tokens[*count].value,value);
    (*count)++;
}

void separateToken(char *token,char *symbols,Token tokens[],int *tokenCount){
    char *keywords[]={
        "class","constructor","function","method","field",
        "static","var","int","char","boolean","void","true",
        "false","null","this","let","do","if","else","while",
        "return"};

    if(strchr(symbols,token[0]) && token[1]=='\0'){ // symbols処理
        addToken(tokens,tokenCount,"symbol",&token[0]);
        // printf("%s\n",token);
    }else{ // keyword処理
        int is_keyword=0;
        for(int i=0;i<sizeof(keywords)/sizeof(keywords[0]);i++){
            if(strcmp(token,keywords[i])==0){
                addToken(tokens,tokenCount,"keyword",token);
                // printf("%s\n",token);
                is_keyword=1;
                break;
            }
        }
        if(is_keyword==0){ // 数値定数か識別子かの判定
            int is_number=1;
            for(int i=0;token[i];i++){
                if(!isdigit((unsigned char)token[i])){
                    is_number=0;
                    break;
                }
            }
            if(is_number!=0){
                addToken(tokens,tokenCount,"integerConstant",token); // integerConstant処理
                // printf("%s\n",token);
            }else{
                addToken(tokens,tokenCount,"identifier",token); // identifier処理
                // printf("%s\n",token);
            }
        }
    }
}

void parseLine(Token tokens[],int *tokenCount,char *line) {
    char *symbols="{}()[].,;+-*/&|<>=~";
    char token[LINE]=""; // token(部品が入る)
    int i=0,j=0;
    int IsString=0;

    while(line[i]!='\0'){ // １文字ずつ検索
        // printf("i=%d,j=%d\n",i,j);
        if(IsString!=0){
            if (line[i]=='"'){
                token[j]='\0';
                addToken(tokens,tokenCount,"stringConstant",token); // stringConstant処理
                IsString=0;
                j=0;
                i++; // 終わりの " をスキップ
            }else{
                token[j++]=line[i++];
            }
        }else if(line[i]=='"'){ // string判定
            IsString=1;
            i++;  // 開始の " をスキップ
            j=0;
        }else if(isspace((unsigned char)line[i])){
            if(j>0){
                token[j]='\0';
                separateToken(token,symbols,tokens,tokenCount);
                j=0;
            }
            i++;
        }else if(strchr(symbols,line[i])){
            if(j>0){
                token[j]='\0';
                separateToken(token,symbols,tokens,tokenCount);
                j=0;
            }
            char symbol[2]={line[i],'\0'};
            separateToken(symbol,symbols,tokens,tokenCount);
            i++;
        }else{
            token[j++]=line[i++];
        }
    }
    if(j>0){
        token[j]='\0';
        separateToken(token,symbols,tokens,tokenCount);
    }
}

void Tokenizer(FILE *input,Token tokens[],int *tokenCount){
    char line[LINE];
    int in_block_comment=0;
    while(fgets(line,sizeof(line),input)!=NULL){
        remove_comments(line,&in_block_comment);  // コメント削除（状態付き）
        trim(line);                                // 空白削除
        if (strlen(line)==0) continue;           // 空行スキップ

        parseLine(tokens,tokenCount,line); // パース(部品ごとに分ける)
    }
}

void Parser(char *filepath,Token tokens[],int *tokenCount,int count){
    char* target=".jack";
    char* replacement="Out.vm";
    char* output_filename=replace_extension(filepath,target,replacement); // 拡張子変更
    VMWriter *writer=createVMWriter(output_filename);
    
    while(count<(*tokenCount)){
        if(strcmp(tokens[count].type,"keyword")==0){
            if(strcmp(tokens[count].value,"class")==0){
                compileClass(writer,tokens,&count);
            }
        }
        count++;
    }

    closeVMWriter(writer);
}

int hasJackExtension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".jack") == 0;
}

int main(int argc,char *argv[]){
    Token tokens[MAX_TOKENS]; //tokens宣言
    int tokenCount=0;
    if (argc!=2) {
        perror("ディレクトリ名を入力してください。($ ./a.out [directory name])");
        return 1;
    }

    DIR *dir = opendir(argv[1]);
    if (!dir) {
        perror("ディレクトリが見つかりませんでした。");
        return 1;
    }

    struct dirent *ent;
    while ((ent = readdir(dir))) {
        if(ent->d_type == DT_REG && hasJackExtension(ent->d_name)){ //ディレクトリ内のファイルを一つずつ参照
            char filepath[GIGA];
            int startpoint=tokenCount;
            snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], ent->d_name);

            FILE *input=fopen(filepath, "r"); //filepath=参照するファイル(翻訳元)

            if(!input){
                perror("ファイルが開けませんでした。");
                return 1;
            }else{
                Tokenizer(input,tokens,&tokenCount);
                fclose(input);

                // for (int i = 0; i < tokenCount; i++) {
                // printf("Token(%s, %s)\n", tokens[i].type, tokens[i].value);
                // }// this is for debug

                Parser(filepath,tokens,&tokenCount,startpoint);
            }
        }
    }
    closedir(dir);

    return 0;
}
