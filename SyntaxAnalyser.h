#ifndef SYNTAX_ANALYSER_H
#define SYNTAX_ANALYSER_H

#include <stdio.h>
#include "Structs.h"  // Token構造体を別ヘッダに分けると便利
#include "CodeGenerator.h"

void compileTerm(VMWriter *writer,Token tokens[],int *count);
void compileExpression(VMWriter *writer,Token tokens[],int *count);
void compileClass(VMWriter *writer,Token tokens[],int *count);
void compileSubroutine(VMWriter *writer,Token tokens[],int *count,char *className);
void compileSubroutineBody(VMWriter *writer,Token tokens[],int *count);
void compileStatements(VMWriter *writer,Token tokens[],int *count);
void compileLet(VMWriter *writer,Token tokens[],int *count);
void compileDo(VMWriter *writer,Token tokens[],int *count);
void compileReturn(VMWriter *writer,Token tokens[],int *count);
void compileWhile(VMWriter *writer,Token tokens[],int *count);
void compileIf(VMWriter *writer,Token tokens[],int *count);
void compileMulDiv(VMWriter *writer,Token tokens[],int *count);
void compileAddSub(VMWriter *writer,Token tokens[],int *count);
void compileOthers(VMWriter *writer,Token tokens[],int *count);

#endif