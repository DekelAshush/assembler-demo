#ifndef FIRSTLOOP_H
#define FIRSTLOOP_H
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

#define OPERAND_WHITE_SPACE_SKIP while(temp_curr_operand[i] == ' ' || temp_curr_operand[i] == '\n' || temp_curr_operand[i] == '\t'){i++;}
#define CURR_LINE_WHITE_SPACE_SKIP while(temp_curr_line[i] == ' ' || temp_curr_line[i] == '\n' || temp_curr_line[i] == '\t'){i++;}

fpos_t checkLength(FILE *assembly_code,fpos_t *p1,fpos_t *p2,int *numError,int *currentNumberLine);
/*
 get the File location 2 file pointers, take each time 80 char, check the legnth of each row and send error if its too long and
 go throw to the next line, function send the location of the row that has legal lenth size.
 the function gets num of errors and the line number if the function find an error she can trigger it and write it.
 */

void resetVars(char* label,char* operands,char* key_word);
/*reset the vars*/

int checkMacro(char *macroName);

int isLegalLabel(Schart *schartHead,char* label,int currentNumberLine, int *numError, int guidenceWord) ;

int DataImageLineCreate(dataImg *currDataPtr,Schart *head,char *currLine, long *adress,int key_word, char* operands, int lineNumber, int *numErrors);

Schart *searchSymbolName(Schart *pmcSymbol, char *symbolName);
/*the function get a name of symbol and the symbol chart,
 if it find the symbol in the chart it returns its node*/

int InsImageLineCreate(Schart *head,insImg *currInsPtr,char *currLine, long *adress,long *dc, int opcode, char* operands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail) ;

long checkTheLabelExist(Schart *schartHead,char* label);

long checkTheDefineExist(Schart *schartHead,char* label);

int instructionBinaryLine(Schart *head,insImg *currInsPtr, int opcode , char *operands,  int lineNumber,int *numErrors,int *ic,long *dc,long *adress,int loop,extChart **extHead,extChart **extTail);

#endif