#ifndef UTILS_H
#define UTILS_H

#include "globals.h"

enum TypeOfSentence{
    EMPTY_LINE = 1,
    NOTE_LINE,
    INSTRUCTION_LINE,
    GUIDANCE_LINE
};

int whatLineAmI(const char* curr_line);
/*
Checks what the type of the sentense, go through the whole line
and return the type.
[EMPTY_LINE, NOTE_LINE, INSTRUCTION_LINE, GUIDANCE_LINE ]
*/
void resetVars(char* label,char* operands,char* key_word);

int checkMacro(char *macroName);
/*
This function checks what type of instruction line,
and also brings the the current opcode funct.
*/

void getCommands(char *curr_line, char* label, char* key_word, char* operands, int number_line,int *numError,int loop);
/*
Get the label, key word and operands of the current line.
go through the whole line.
*/
int isLegalLabel(Schart *schartHead,char* label,int currentNumberLine, int *numError, int guidenceWord);

int ifRegister(char* key_word);

int getKeyWord (char* key_word);

void switchOperands(int *sourceOperandMethod,int *targetOperandMethod,int *targetOperand,int *sourceOperand);

void freeMacroTableNodes(mcNode *headMacroLine);

void freeSymbolTableNodes(Schart *headS);

void freeExtTableNodes(extChart *headEx);

#endif