#ifndef SYNTAX_ANALYSER_H
#define SYNTAX_ANALYSER_H

#include <stdio.h>
#include "Structs.h"  // Token構造体を別ヘッダに分けると便利
#include "CodeGenerator.h"

const char* mapKindToSegment(const char* kind);

void compileTerm(VMWriter *writer,SymbolTable* table,Token tokens[],int *count);
void compileExpression(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
int compileClass(VMWriter *writer,Token tokens[],int *count);
int compileClassVarDec(SymbolTable *table,Token tokens[],int *count);
int compileSubroutineDec(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileSubroutineBody(VMWriter *writer,SymbolTable *table,Token tokens[],int *count,char* subroutineName,char* subroutineKind);
void compileVarDec(SymbolTable *table,Token tokens[],int *count);
int compileStatements(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileLet(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileIf(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileWhile(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileDo(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileReturn(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
void compileSubroutineCall(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
int compileExpressionList(VMWriter *writer,SymbolTable *table,Token tokens[],int *count);
// void compileMulDiv(VMWriter *writer,Token tokens[],int *count);
// void compileAddSub(VMWriter *writer,Token tokens[],int *count);
// void compileOthers(VMWriter *writer,Token tokens[],int *count);

#endif