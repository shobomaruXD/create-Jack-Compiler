#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <stdio.h>
#include "Structs.h"

VMWriter *createVMWriter(const char *filename);
void closeVMWriter(VMWriter *writer);

void writePush(VMWriter *writer,const char *segment,int index);
void writePop(VMWriter *writer,const char *segment,int index);
void writeArithmetic(VMWriter *writer,const char *command);
void writeLabel(VMWriter *writer,const char *label);
void writeGoto(VMWriter *writer,const char *label);
void writeIf(VMWriter *writer,const char *label);
void writeCall(VMWriter *writer,const char *name,int nArgs);
void writeFunction(VMWriter *writer,const char *name,int nLocals);
void writeReturn(VMWriter *writer);

#endif