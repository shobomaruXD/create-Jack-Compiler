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
char className[64];

const char* mapKindToSegment(const char* kind){
    if(strcmp(kind,"var")==0) return "local";
    if(strcmp(kind,"argument")==0) return "argument";
    if(strcmp(kind,"static")==0) return "static";
    if(strcmp(kind,"field")==0) return "this";
    return "unknown";
}

const char* mapOpToCommand(const char* Op){
    if(strcmp(Op,"+")==0) return "add";
    if(strcmp(Op,"-")==0) return "sub";
    if(strcmp(Op,"*")==0) return "Math.multiply";
    if(strcmp(Op,"/")==0) return "Math.divine";
    return "unknown";
}

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

void compileTerm(VMWriter *writer,SymbolTable* table,Token tokens[],int *count){
    char *val=tokens[*count].value;
    char *typ=tokens[*count].type;

    if(strcmp(typ,"integerConstant")==0){ //数値定数
        printf("int item\n");
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
        compileExpression(writer,table,tokens,count);
        if(strcmp(tokens[*count].value,")")==0){
            (*count)++;
        }else{
            fprintf(stderr,"expected ')' here");
            return;
        }
        return;
    }

    if(strcmp(val,"-")==0 || strcmp(val,"~")==0){ //unaryop
        char op=val[0];
        (*count)++;
        compileTerm(writer,table,tokens,count);
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
        compileExpression(writer,table,tokens,count);
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
            compileExpression(writer,table,tokens,count);
            nArgs++;
            while(strcmp(val,",")==0){
                (*count)++; // skip ","
                compileExpression(writer,table,tokens,count);
                nArgs++;
            }
        }
        if(strcmp(val,")")==0){
            (*count)++;
        }
        writeCall(writer,name,nArgs);
        return;
    }else{ //varNameのみ
        Symbol *s=search(table,val);
        if(s==NULL){
            fprintf(stderr,"Error: Variable '%s' is not defined\n",val);
            return;
        }
        const char *segment=mapKindToSegment(s->kind);
        writePush(writer,segment,s->index);
        return;
    }
}

// void compileOthers(VMWriter *writer,Token tokens[],int *count){
//     char *op=tokens[*count].value;
//     if(strcmp(op,"&")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"and");
//     }else if(strcmp(op,"|")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"or");
//     }else if(strcmp(op,"<")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"lt");
//     }else if(strcmp(op,">")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"gt");
//     }else if(strcmp(op,"=")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"eq");
//     }
    // else{
    //     compileTerm(writer,tokens,count);
    // }
// }

// void compileMulDiv(VMWriter *writer,Token tokens[],int *count){
//     while(1){
//         compileOthers(writer,tokens,count);
//         if(strcmp(tokens[*count].value,"*")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeCall(writer,"Math.multiply",2);
//         }else if(strcmp(tokens[*count].value,"/")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeCall(writer,"Math.divide",2);
//         }else{
//             break;
//         }
//     }
// }

// void compileAddSub(VMWriter *writer,Token tokens[],int *count){
//     while(1){
//         compileMulDiv(writer,tokens,count);
//         if(strcmp(tokens[*count].value,"+")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"add");
//         }else if(strcmp(tokens[*count].value,"-")==0){
//             (*count)++;
//             compileTerm(writer,tokens,count);
//             writeArithmetic(writer,"sub");
//         }else{
//             break;
//         }
//     }
// }

void compileExpression(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    char *Ops="+-*/&|<>=";
    compileTerm(writer,table,tokens,count);

    while(strstr(Ops,tokens[*count].value)){
        char *op=tokens[*count].value;
        (*count)++; //skip op
        compileTerm(writer,table,tokens,count);
        if(strcmp(op,"+")==0||strcmp(op,"-")==0){
            writeArithmetic(writer,mapOpToCommand(op));
        }else if(strcmp(op,"*")==0||strcmp(op,"/")==0){
            writeCall(writer,mapOpToCommand(op),2);
        }
        
    }
}

int compileExpressionList(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    int nArgs=0;

    while(1){
        compileExpression(writer,table,tokens,count);
        nArgs++;

        if(strcmp(tokens[*count].value,",")==0){
            (*count)++;
        }else{
            break;
        }
    }

    return nArgs;
}

void compileSubroutineCall(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    char *val=tokens[*count].value;
    if(strcmp(val,className)==0){
        (*count)++;
        (*count)++; //skip  "."
        char *subroutineName=tokens[*count].value;
        (*count)++;
        (*count)++; //skip "("
        compileExpressionList(writer,table,tokens,count);
        (*count)++; //skip ")"

        char functionName[N];
        sprintf(functionName,"%s.%s",val,subroutineName);
        writeCall(writer,functionName,1);
    }else if(strcmp(tokens[(*count)++].value,".")==0){
        Symbol *s=search(table,val);
        if(s==NULL){
            fprintf(stderr,"Error: Variable '%s' is not defined\n",val);
            return;
        }
        const char *segment=mapKindToSegment(s->kind);
        writePush(writer,segment,s->index);
        (*count)++;

        (*count)++; //skip  "."
        char *subroutineName=tokens[*count].value;
        (*count)++;
        (*count)++; //skip "("
        int nArgs=compileExpressionList(writer,table,tokens,count);
        (*count)++; //skip ")"

        char functionName[N];
        sprintf(functionName,"%s.%s",val,subroutineName);
        writeCall(writer,functionName,nArgs+1);
    }else if(strcmp(tokens[(*count)++].value,"(")==0){
        writePush(writer,"pointer",0);
        (*count)++;

        (*count)++; //skip "("
        int nArgs=compileExpressionList(writer,table,tokens,count);
        (*count)++; //skip ")"

        char functionName[N];
        sprintf(functionName,"%s.%s",val,className);
        writeCall(writer,functionName,nArgs+1);
    }else{
        fprintf(stderr,"Error: there are no subroutine calls");
        return;
    }
}

void compileLet(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    (*count)++; // skip "let"

    char *varName=tokens[*count].value; //get varName Ex:x
    (*count)++;

    Symbol *s=search(table,varName);
    if(s==NULL){
        fprintf(stderr,"Error: Variable '%s' is not defined\n",varName);
        return;
    }
    while(strcmp(tokens[*count].value,"[")==0){
        (*count)++; //skip "["
        compileExpression(writer,table,tokens,count);
        (*count)++; //skip "]"
    }

    (*count)++; //skip "="
    compileExpression(writer,table,tokens,count);

    const char *segment=mapKindToSegment(s->kind);
    writePop(writer,segment,s->index);
    (*count)++; //skip ";"
}

void compileIf(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    (*count)++; //skip "if"
    (*count)++; //skip "("
    compileExpression(writer,table,tokens,count);
    (*count)++; //skip ")"

    char *falseLabel=newLabel("IF_FALSE");
    char *endLabel=newLabel("IF_END");

    (*count)++; //skip "{"
    compileStatements(writer,table,tokens,count);
    (*count)++; //skip "}"
    

    char *trueLabel=newLabel("IF_TRUE");


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

void compileWhile(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    (*count)++; //skip "while"
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

void compileDo(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    (*count)++; //skip "do"

    compileSubroutineCall(writer,table,tokens,count);
    writePop(writer,"temp",0);

    (*count)++; //skip  ";"
}

void compileReturn(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    (*count)++; //skip "return"

    if(strcmp(tokens[*count].value,";")!=0){
        compileExpression(writer,table,tokens,count);
    }else{
        writePush(writer,"constant",0); // void return
    }
    writeReturn(writer);
    (*count)++; //skip ";"
}

int compileStatements(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    while(count){
        if(strcmp(tokens[*count].value,"let")==0){
            compileLet(writer,table,tokens,count);
        }else if(strcmp(tokens[*count].value,"if")==0){
            compileIf(writer,table,tokens,count);
        }else if(strcmp(tokens[*count].value,"while")==0){
            compileWhile(writer,table,tokens,count);
        }else if(strcmp(tokens[*count].value,"do")==0){
            compileDo(writer,table,tokens,count);
        }else if(strcmp(tokens[*count].value,"return")==0){
            compileReturn(writer,table,tokens,count);
        }else{
            break;
        }
    }
    return 0;
}

void compileVarDec(SymbolTable *table,Token tokens[],int *count){
    char name[N],type[N];
    (*count)++; //skip var

    strcpy(type,tokens[*count].value);  //get type  Ex:int
    (*count)++;
    while(1){
        strcpy(name,tokens[*count].value);  //get varName
        (*count)++;
        define(table,name,type,"var");  //kind="argument"で固定

        if(strcmp(tokens[*count].value,",")==0){
            (*count)++; //skip ","
        }else if(strcmp(tokens[*count].value,";")==0){
            (*count)++; //skip ";"
            break;
        }else{
            fprintf(stderr,"expected ';' here");
            return;
        }
    }
}

void compileSubroutineBody(VMWriter *writer,SymbolTable *table,Token tokens[],int *count,char* subroutineName,char* subroutineKind){
    (*count)++; // skip '{'

    while(strcmp(tokens[*count].value,"var")==0){
        compileVarDec(table,tokens,count);
    }

    char funcName[128];
    sprintf(funcName,"%s.%s",className,subroutineName); //get full function name
    
    int nLocals=table->varCount;    //get varCount

    writeFunction(writer,funcName,nLocals);

    if(strcmp(subroutineKind,"method")==0){
        writePush(writer,"argument",0);
        writePop(writer,"pointer",0);
    }else if(strcmp(subroutineKind,"constructor")==0){
        writePush(writer,"constant",table->fieldCount);
        writeCall(writer,"Memory.alloc",1);
        writePop(writer,"pointer",0);
    }

    compileStatements(writer,table,tokens,count);

    (*count)++; // skip '}'
}

int compileParameterList(SymbolTable *table,Token tokens[],int *count){
    if(strcmp(tokens[*count].value,")")==0){
        (*count)++; //skip ")"
        return 0;
    }
    
    while(1){
        char name[N],type[N];
        strcpy(type,tokens[*count].value);
        (*count)++;
        strcpy(name,tokens[*count].value);
        (*count)++;
        define(table,name,type,"argument");  //kind="argument"で固定

        if(strcmp(tokens[*count].value,",")==0){
            (*count)++;
        }else{
            (*count)++;
            break;
        }
    }
    return 0;
}

int compileSubroutineDec(VMWriter *writer,SymbolTable *table,Token tokens[],int *count){
    startSubroutine(table);
    char name[N],type[N],kind[N];
    strcpy(kind,tokens[*count].value);  //Ex:function
    (*count)++;
    strcpy(type,tokens[*count].value);  //void?
    (*count)++;
    strcpy(name,tokens[*count].value);  //Ex:main
    (*count)++;
    if(strcmp(kind,"method")==0){
        define(table,"this",className,"argument"); 
    }

    (*count)++; //skip "("
    compileParameterList(table,tokens,count);

    compileSubroutineBody(writer,table,tokens,count,name,kind);
    return 0;
}

int compileClassVarDec(SymbolTable *table,Token tokens[],int *count){
    char *name,*type,*kind;
    strcpy(kind,tokens[*count].value);
    (*count)++;
    strcpy(type,tokens[*count].value);
    (*count)++;
    strcpy(name,tokens[*count].value);
    (*count)++;
    define(table,name,type,kind);
    while(strcmp(tokens[*count].value,",")==0){
        (*count)++;
        strcpy(name,tokens[*count].value);
        (*count)++;
        define(table,name,type,kind);
    }
    if(strcmp(tokens[*count].value,";")==0){
        (*count)++;
    }else{
        fprintf(stderr,"';'expected\n");
        return 1;
    }
    return 0;
}

int compileClass(VMWriter *writer,Token tokens[],int *count){
    SymbolTable *table=createSymbolTable();
    (*count)++; //skip "class"

    strcpy(className,tokens[*count].value); //get className Ex:Main
    (*count)++;
    (*count)++; //skip "{"

    while(strcmp(tokens[*count].value,"static")==0||strcmp(tokens[*count].value,"field")==0){
        compileClassVarDec(table,tokens,count);
    }

    while(strcmp(tokens[*count].value,"function")==0||strcmp(tokens[*count].value,"constructor")==0||strcmp(tokens[*count].value,"method")==0){
        compileSubroutineDec(writer,table,tokens,count);
    }

    if(strcmp(tokens[*count].value,"}")!=0){
        fprintf(stderr,"'}'expected @Class\n");
        return 1;
    }
    (*count)++;
    
    return 0;
}