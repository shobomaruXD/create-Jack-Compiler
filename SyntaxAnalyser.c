#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Structs.h"
#include "SyntaxAnalyser.h"
#include "CodeGenerator.h"
#include "SymbolTable.h"

#define MAX_STORAGE 256

static int labelCount=0;
char *stop=")];=<>";



static char *newLabel(const char *base){
    char *label=malloc(64);
    sprintf(label,"%s%d",base,labelCount++);
    return label;
}

// int checkUnary(Token tokens[],int *count){
//     while(strchr(tokens[*count].value,stop)!=0){
//         if(strcmp(tokens[*count].value,"-")==0 || strcmp(tokens[*count].value,"~")==0){
//             return 1;
//         }
//     }
//     return 0;
// }

int checkMulDiv(Token tokens[],int *count){
    while(!strchr(stop,tokens[*count].value[0])){
        (*count)++;
        if(strcmp(tokens[*count].value,"*")==0 || strcmp(tokens[*count].value,"/")==0){
            return 1;
        }
    }
    return 0;
}

void compileTerm(VMWriter *writer,Token tokens[],int *count){
    char *val=tokens[*count].value;
    char *typ=tokens[*count].type;

    if(strcmp(typ,"integerConstant")==0){ //数値定数
        printf("int\n");
        int n=atoi(val);
        writePush(writer,"constant",n);
        (*count)++;
        return;
    }

    if(strcmp(typ,"stringConstant")==0){ //文字列定数
        // printf("文字列定数だよ！ type=%s value=%s\n",tokens[*count].type,tokens[*count].value);
        int len=strlen(val);
        writePush(writer,"constant",len);
        writeCall(writer,"String.new",1);
        for(int i=0;i<len;i++){
            writePush(writer,"constant",val[i]);
            writeCall(writer,"String.appendChar",2);
        }
        (*count)++;
        return;
    }

    if(strcmp(val,"true")==0){ //キーワード定数
        printf("a\n");
        writePush(writer,"constant",0);
        writeArithmetic(writer,"not");
        (*count)++;
        return;
    }else if(strcmp(val,"false")==0 || strcmp(val,"null")==0){
        printf("b\n");
        writePush(writer,"constant",0);
        (*count)++;
        return;
    }else if(strcmp(val,"this")==0){
        writePush(writer,"pointer",0);
        (*count)++;
        return;
    }

    if(strcmp(val,"(")==0){ //( "Expression" )
        printf("(x)\n");
        (*count)++; // skip '('
        compileExpression(writer,tokens,count);
        if(strcmp(tokens[*count].value,")")==0){
            (*count)++;
        }
        return;
    }

    if(strcmp(val,"-")==0 || strcmp(val,"~")==0){ //unaryop
        char op=val[0];
        (*count)++;
        compileTerm(writer,tokens,count);
        if(op=='-'){
            writeArithmetic(writer,"neg");
        }else if(op=='~'){
            writeArithmetic(writer,"not");
        }
        return;
    }

    // varName or subroutineCall
    char name[128];
    strcpy(name,val);
    (*count)++; // skip "varName|subroutineName"
    if(strcmp(val,"[")== 0){ //varName["Expression"]
        (*count)++; // skip '['
        compileExpression(writer,tokens,count);
        (*count)++; // skip "]"
        writePush(writer,"local",0); // 仮実装
        writeArithmetic(writer,"add");
        writePop(writer,"pointer", 1);
        writePush(writer,"that",0);
        return;
    }else if(strcmp(val,"(")==0 || strcmp(val,".")==0){ //subroutineCall
        if(strcmp(val,".")==0){
            strcat(name,".");
            (*count)++;
            strcat(name,val);
            (*count)++;
        }
        (*count)++; // skip "("

        int nArgs=0; // how many expressions
        if(strcmp(val,")")!=0){ // ExpressionList
            compileExpression(writer,tokens,count);
            nArgs++;
            while(strcmp(val,",")==0){
                (*count)++; // skip ","
                compileExpression(writer,tokens,count);
                nArgs++;
            }
        }
        if(strcmp(val,")")==0){
            (*count)++;
        }
        writeCall(writer,name,nArgs);
        return;
    }else{ //varNameのみ
        writePush(writer,"local",0); // シンボルテーブル実装時に拡張
        return;
    }
}

void compileOthers(VMWriter *writer,Token tokens[],int *count){
    char *op=tokens[*count].value;
    if(strcmp(op,"&")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"and");
    }else if(strcmp(op,"|")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"or");
    }else if(strcmp(op,"<")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"lt");
    }else if(strcmp(op,">")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"gt");
    }else if(strcmp(op,"=")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"eq");
    }
    // else{
    //     compileTerm(writer,tokens,count);
    // }
}

void compileMulDiv(VMWriter *writer,Token tokens[],int *count){
    while(1){
        compileOthers(writer,tokens,count);
        if(strcmp(tokens[*count].value,"*")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeCall(writer,"Math.multiply",2);
        }else if(strcmp(tokens[*count].value,"/")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeCall(writer,"Math.divide",2);
        }else{
            break;
        }
    }
}

void compileAddSub(VMWriter *writer,Token tokens[],int *count){
    while(1){
        compileMulDiv(writer,tokens,count);
        if(strcmp(tokens[*count].value,"+")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"add");
        }else if(strcmp(tokens[*count].value,"-")==0){
            (*count)++;
            compileTerm(writer,tokens,count);
            writeArithmetic(writer,"sub");
        }else{
            break;
        }
    }
}

void compileExpression(VMWriter *writer,Token tokens[],int *count){
    compileTerm(writer,tokens,count);
    compileAddSub(writer,tokens,count);
}

// void compileLet(FILE *output,Token tokens[],int *count,int *indents){
//     MakeIndents(output,indents);
//     fprintf(output,"<letStatement>\r\n");
//     (*indents)++;
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     while((strcmp(tokens[*count].value,"[")==0) && (strcmp(tokens[*count].value,"=")!=0)){
//         writeToken(output,tokens,count,indents);
//         compileExpression(output,tokens,count,indents);
//         writeToken(output,tokens,count,indents);
//     }
//     writeToken(output,tokens,count,indents);
//     compileExpression(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents); // ;
//     (*indents)--;
//     MakeIndents(output,indents);
//     fprintf(output,"</letStatement>\r\n");
// }

void compileLet(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"let")==0){
        (*count)++; // skip 'let'
    }

    char varName[64];
    strcpy(varName,tokens[*count].value);
    (*count)++;

    while((strcmp(tokens[*count].value,"[")==0) && (strcmp(tokens[*count].value,"=")!=0)){
        (*count)++; //skip "["
        compileExpression(writer,tokens,count);
        (*count)++; //skip "]"
    }

    if(strcmp(tokens[*count].value,"=")==0){
        (*count)++;
    }
    compileExpression(writer,tokens,count);

    int value=atoi(tokens[*count].value); // int型only
    writePush(writer,"constant",value);
    (*count)++;

    writePop(writer,"local",0); // 仮にxはlocal 0

    if(strcmp(tokens[*count].value,";")==0){
        (*count)++;
    }
}

// void compileDo(VMWriter *writer,Token tokens[],int *count){
//     MakeIndents(output,indents);
//     fprintf(output,"<doStatement>\r\n");
//     (*indents)++;
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     if(strcmp(tokens[*count].value,"(")==0){ // subroutineCall
//         writeToken(output,tokens,count,indents);
//         compileExpressionList(output,tokens,count,indents);
//         writeToken(output,tokens,count,indents);
//     }else if(strcmp(tokens[*count].value,".")==0){ // subroutineCall
//         for(int i=0;i<3;i++){
//             writeToken(output,tokens,count,indents);
//         }
//         compileExpressionList(output,tokens,count,indents);
//         writeToken(output,tokens,count,indents); // <symbol> ) </symbol>
//     }
//     writeToken(output,tokens,count,indents);
//     (*indents)--;
//     MakeIndents(output,indents);
//     fprintf(output,"</doStatement>\r\n");
// }

void compileDo(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"do")==0){
        (*count)++;
    }

    char subroutineName[64];
    strcpy(subroutineName,tokens[*count].value);
    (*count)++; // skip subroutineName

    if(strcmp(tokens[*count].value,".")==0){
        strcat(subroutineName,".");
        (*count)++;
        strcat(subroutineName,tokens[*count].value);
        (*count)++;
    }
    (*count)++; // skip "("

    int nArgs=0; // how many expressions
    if(strcmp(tokens[*count].value,")")!=0){ // ExpressionList
        printf("yes\n");
        compileExpression(writer,tokens,count);
        nArgs++;
        while(strcmp(tokens[*count].value,",")==0){
            (*count)++; // skip ","
            compileExpression(writer,tokens,count);
            nArgs++;
        }
    }
    if(strcmp(tokens[*count].value,")")==0){
        (*count)++;
    }
    writeCall(writer,subroutineName,nArgs);
    (*count)++; //skip ";"
}

// void compileReturn(VMWriter *writer,Token tokens[],int *count){
//     MakeIndents(output,indents);
//     fprintf(output,"<returnStatement>\r\n");
//     (*indents)++;
//     writeToken(output,tokens,count,indents);
//     if(strcmp(tokens[*count].value,";")!=0){
//         compileExpression(output,tokens,count,indents);
//     }
//     writeToken(output,tokens,count,indents);
//     (*indents)--;
//     MakeIndents(output,indents);
//     fprintf(output,"</returnStatement>\r\n");
// }

void compileReturn(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"return")==0){
        (*count)++;
    }

    if(strcmp(tokens[*count].value,";")!=0){
        int value=atoi(tokens[*count].value); // return 1; のような定数を返す
        compileExpression(writer,tokens,count);
    }else{
        writePush(writer,"constant",0); // void return
    }
    writeReturn(writer);
    if(strcmp(tokens[*count].value,";")==0){ //skip ";"
        (*count)++;
    }
}

// void compileWhile(VMWriter *writer,Token tokens[],int *count){
//     MakeIndents(output,indents);
//     fprintf(output,"<whileStatement>\r\n");
//     (*indents)++;
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     compileExpression(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     compileStatements(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     (*indents)--;
//     MakeIndents(output,indents);
//     fprintf(output,"</whileStatement>\r\n");
// }

void compileWhile(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"while")==0){
        (*count)++;
    }
    char *startLabel=newLabel("WHILE_EXP");
    char *endLabel=newLabel("WHILE_END");

    writeLabel(writer,startLabel);

    // 条件をtrueに固定する
    writePush(writer,"constant",0); // falseにするとループしない
    writeArithmetic(writer,"not"); // trueに変換

    writeIf(writer, endLabel);

    // 本体スキップ
    while(strcmp(tokens[*count].value,"{")!=0){
        (*count)++;
    }
    (*count)++;
    while(strcmp(tokens[*count].value,"}")!=0){
        (*count)++;
    }
    writeGoto(writer,startLabel);
    writeLabel(writer,endLabel);
    (*count)++;
    free(startLabel);
    free(endLabel);
}

// void compileIf(VMWriter *writer,Token tokens[],int *count){
//     MakeIndents(output,indents);
//     fprintf(output,"<ifStatement>\r\n");
//     (*indents)++;
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     compileExpression(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     compileStatements(output,tokens,count,indents);
//     writeToken(output,tokens,count,indents);
//     if(strcmp(tokens[*count].value,"else")==0){
//         writeToken(output,tokens,count,indents);
//         writeToken(output,tokens,count,indents);
//         compileStatements(output,tokens,count,indents);
//         writeToken(output,tokens,count,indents);
//     }
//     (*indents)--;
//     MakeIndents(output,indents);
//     fprintf(output,"</ifStatement>\r\n");
// }

void compileIf(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"if")==0){
        (*count)++;
    }
    char *trueLabel=newLabel("IF_TRUE");
    char *falseLabel=newLabel("IF_FALSE");
    char *endLabel=newLabel("IF_END");

    // 条件式:true固定
    writePush(writer,"constant",0);
    writeArithmetic(writer,"not");

    writeIf(writer,trueLabel);
    writeGoto(writer,falseLabel);

    writeLabel(writer,trueLabel);

    // ifブロックのスキップ
    while(strcmp(tokens[*count].value,"{")!=0){
        (*count)++;
    }
    (*count)++;
    while(strcmp(tokens[*count].value,"}")!=0){
        (*count)++;
    }
    (*count)++;
    writeGoto(writer, endLabel);
    writeLabel(writer, falseLabel);

    // elseブロックを検出
    if(strcmp(tokens[*count].value,"else")==0){
        (*count)++;
        while(strcmp(tokens[*count].value,"{")!=0){
            (*count)++;
        }
        (*count)++;
        while(strcmp(tokens[*count].value,"}")!=0){
            (*count)++;
        }
        (*count)++;
    }
    writeLabel(writer, endLabel);

    free(trueLabel);
    free(falseLabel);
    free(endLabel);
}

void compileStatements(VMWriter *writer,Token tokens[],int *count){
    while(count){
        if(strcmp(tokens[*count].value,"let")==0){
            compileLet(writer,tokens,count);
        }else if(strcmp(tokens[*count].value,"do")==0){
            compileDo(writer,tokens,count);
        }else if(strcmp(tokens[*count].value,"return")==0){
            compileReturn(writer,tokens,count);
        }else if(strcmp(tokens[*count].value,"while")==0){
            compileWhile(writer,tokens,count);
        }else if(strcmp(tokens[*count].value,"if")==0){
            compileIf(writer,tokens,count);
        }else{
            break;
        }
    }
}

void compileSubroutineBody(VMWriter *writer,Token tokens[],int *count){
    while (strcmp(tokens[*count].value,"{")!=0){
        (*count)++;
    }
    (*count)++; // skip '{'

    while(strcmp(tokens[*count].value,"var")==0){
        while(strcmp(tokens[*count].value,";")!=0){
            (*count)++;
        }
        (*count)++; // skip ';'
    }

    compileStatements(writer,tokens,count);

    while(strcmp(tokens[*count].value,"}")!=0 && tokens[*count].value[0]!='\0'){
        (*count)++;
    }
    (*count)++; // skip '}'
}

void compileSubroutine(VMWriter *writer,Token tokens[],int *count,char *className){
    LocalCounter *LC=startSubroutine();
    Vars *varStorage[MAX_STORAGE];

    if(strcmp(tokens[*count].value,"function")==0 || strcmp(tokens[*count].value,"method")==0 || strcmp(tokens[*count].value,"constructor")==0){
        (*count)++;
    }
    (*count)++;

    char funcName[128];
    strcpy(funcName,className); //make it easy
    strcat(funcName,".");
    strcat(funcName,tokens[*count].value);
    (*count)++;

    while((strcmp(tokens[*count].value,"{")!=0)&&(tokens[*count].value[0]!='\0')){
        if(strcmp(tokens[*count].type,"identifier")==0){
            char varName[128];
            strcpy(varName,tokens[*count].value);
            defineArgsVars(LC,varName,varStorage);
        }
        (*count)++;
    }
    writeFunction(writer,funcName,0); // CodeGeneratorへ
    compileSubroutineBody(writer,tokens,count);
}

void compileClass(VMWriter *writer,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,"class")==0){
        (*count)++;
    }
    char className[64];
    strcpy(className,tokens[*count].value);
    (*count)++;

    while(strcmp(tokens[*count].value,"{")!=0){
        (*count)++;
    }
    (*count)++;

    while(strcmp(tokens[*count].value,"}")!=0){
        if(strcmp(tokens[*count].value,"function")==0||strcmp(tokens[*count].value,"constructor")==0||strcmp(tokens[*count].value,"method")==0){
            compileSubroutine(writer,tokens,count,className);
        }else{
            (*count)++;
        }
    }
    (*count)++;
}