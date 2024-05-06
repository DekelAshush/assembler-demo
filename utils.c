#include <ctype.h>
#include <string.h>
#include "utils.h"
#include "errors.h"
#include "firstLoop.h"


int whatLineAmI(const char* curr_line){

    /*declarations*/
    int label_flag = off;
    int i = 0;

    while(curr_line[i] == ' ' || curr_line[i] == '\t'){
        i++;
    }

    if(curr_line[i] == '\0'){
        return EMPTY_LINE;
    }

    /*check if there is a label*/
    while(curr_line[i] != '\0')
    {
        if(curr_line[i] == ':'){
            label_flag = on;
        }
        i++;
    }/*end while loop */

    i = 0;

    if(label_flag == on){
        while(curr_line[i] != ':'){i++;}
    }
    if(curr_line[i] == ':'){i++;}

    while(curr_line[i] == ' ' || curr_line[i] == '\t'){
        i++;
    }

    if(curr_line[i] == '.'){return GUIDANCE_LINE;}
    if(curr_line[i] == '\n'){return EMPTY_LINE;}
    if(curr_line[i] == ';'){return NOTE_LINE;}

    return INSTRUCTION_LINE;

}/*END whatLineAmI()*/



void resetVars(char* label,char* operands,char* key_word){
    /*clear the vars*/
    label[0] = '\0';
    operands[0] = '\0';
    key_word [0] = '\0';
}

int checkMacro(char *macroName){

    int i = 0, temp_string_length = 0;
    char temp_curr_line[LINE_MAX_LENGTH], temp_string[LINE_MAX_LENGTH];

    strcpy(temp_curr_line, macroName);
    strcat(temp_curr_line, "\0");

    while(temp_curr_line[i] != ' ' && temp_curr_line[i] != '\t' && temp_curr_line[i] != '\n' && temp_curr_line[i] != '\0')
    {
        temp_string[temp_string_length] = temp_curr_line[i];
        temp_string_length++;
        i++;
    }

    temp_string[temp_string_length] = '\0';
    strcpy(macroName, temp_string);
    temp_string[0] = '\0';

    while(temp_curr_line[i] == ' ' || temp_curr_line[i] == '\t' || temp_curr_line[i] == '\n'){
        i++;
    }

    if (temp_curr_line[i]!='\0'){
        return FAILED;
    }

    return CORRECT;
}


void getCommands(char *curr_line, char* label, char* key_word, char* operands, int number_line,int *numError,int loop){

    /*declarations*/
    int i = 0,label_flag = off, temp_string_length = 0;
    char temp_curr_line[LINE_MAX_LENGTH], temp_string[LINE_MAX_LENGTH];

    strcpy(temp_curr_line, curr_line);
    strcat(temp_curr_line, "\0");

    /*--get the first field--*/
    CURR_LINE_WHITE_SPACE_SKIP;
    while(temp_curr_line[i] != ' ' && temp_curr_line[i] != '\n' && temp_curr_line[i] != '\t' && temp_curr_line[i] != ':')
    {
        temp_string[temp_string_length] = temp_curr_line[i];
        temp_string_length++;
        i++;
    }
    temp_string[temp_string_length] = '\0';

    /*label scenario*/
    if(temp_curr_line[i] == ':')
    {
        label_flag = on;
        strcpy(label, temp_string);
        temp_string[0] = '\0';
        if(temp_string_length > LABEL_MAX_LENGTH && loop==FIRST){/*in macro we dont check for syntax error*/
            print_errors(LENGTH_LABEL_ERROR, number_line, numError, NULL);
        }
        temp_string_length = 0;
        i++;

    }/*end label scenario*/

    /*key word scenario*/
    if(temp_curr_line[i] == ' ' || temp_curr_line[i] == '\n' || temp_curr_line[i] == '\t' )
    {
        strcpy(key_word, temp_string);
        temp_string[0] = '\0';
        temp_string_length = 0;
    }
    /*end key word scenario*/

    /*--get the second field--*/
    CURR_LINE_WHITE_SPACE_SKIP;

    if(temp_curr_line[i] == ':'&& loop==FIRST){
        print_errors(SYNTAX_ERROR, number_line, numError, curr_line) ;
    }

    /*key word scenario*/
    if (label_flag == on)
    {
        while(temp_curr_line[i] != ' ' && temp_curr_line[i] != '\n' && temp_curr_line[i] != '\t' )
        {
            temp_string[temp_string_length] = temp_curr_line[i];
            temp_string_length++;
            i++;
        }
        temp_string[temp_string_length] = '\0';
        strcpy(key_word, temp_string);

        temp_string[0] = '\0';
        temp_string_length = 0;

        /*operands scenario*/
    }else {
        while (temp_curr_line[i] != '\0' && temp_curr_line[i] != '\n'  && temp_curr_line[i] != '\r') {
            temp_string[temp_string_length] = temp_curr_line[i];
            temp_string_length++;
            i++;
           /* if(operands[i] == '\n'){i++;}*/
        }
    }

    temp_string[temp_string_length] = '\0';
    strcpy(operands, temp_string);
    temp_string[0] = '\0';
    temp_string_length = 0;

    /*--(if there is) get the third field--*/

    /*operands scenario*/
    if (label_flag == on && (strcmp("hlt", key_word)!=0))
    {
        CURR_LINE_WHITE_SPACE_SKIP;

        while(temp_curr_line[i] != '\0' && temp_curr_line[i] != '\n')
        {
            temp_string[temp_string_length] = temp_curr_line[i];
            temp_string_length++;
            i++;
            /*if(operands[i] == '\n'){i++;}*/
        }

        temp_string[temp_string_length] = '\0';
        strcpy(operands, temp_string);
        temp_string[0] = '\0';
    }

}/*END get_commands();*/


int isLegalLabel(Schart *schartHead,char* label,int currentNumberLine, int *numError, int guidenceWord) {

    int check=SUCCESSED;

    switch(guidenceWord){
        case(LABEL):
            if(!islower(label[0]) && !isupper(label[0]) ){/*checks first letter is a letter,not number or sign (i=0)*/
                print_errors(LABEL_ERROR_NAME, currentNumberLine, numError, label);
                return FAILED;
            }

            check = (int)checkTheLabelExist(schartHead, label);/*check that there is not already a label added with the same name*/
            if(check != CORRECT){
                print_errors(LABEL_ALREADY_EXIST_ERROR, currentNumberLine, numError, label);
                return check;
            }

            check = getKeyWord(label);/* check that the label is not as guidance and assembly function names*/
            if(check != CORRECT){
                print_errors(LABEL_SYSTEM_WORD_ERROR, currentNumberLine, numError, label);
                return check;
            }
            check = ifRegister(label);/* check that the label is not a register name*/
            if(check != CORRECT){
                print_errors(LABEL_REGISTER_ERROR, currentNumberLine, numError, label);
                return check;
            }
            break;

    case(DEFINE):
            if(!islower(label[0]) && !isupper(label[0]) ){/*checks first letter is a letter,not number or sign (i=0)*/
                print_errors(DEFINE_ERROR_NAME, currentNumberLine, numError, label);
                return FAILED;
            }

            check = (int)checkTheLabelExist(schartHead, label);/*check that there is not already a label added with the same name*/
            if(check != CORRECT){
                print_errors(DEFINE_ALREADY_EXIST_ERROR, currentNumberLine, numError, label);
                return check;
            }

            check = getKeyWord(label);/* check that the label is not as guidance and assembly function names*/
            if(check != CORRECT){
                print_errors(DEFINE_SYSTEM_WORD_ERROR, currentNumberLine, numError, label);
                return check;
            }
            check = ifRegister(label);/* check that the label is not a register name*/
            if(check != CORRECT){
                print_errors(DEFINE_REGISTER_ERROR, currentNumberLine, numError, label);
                return check;
            }
            break;

    case(ARR):
            if(!islower(label[0]) && !isupper(label[0]) ){/*checks first letter is a letter,not number or sign (i=0)*/
                print_errors(LABEL_ERROR_NAME, currentNumberLine, numError, label);
                return FAILED;
            }

            check = getKeyWord(label);/* check that the label is not as guidance and assembly function names*/
            if(check != CORRECT){
                print_errors(LABEL_SYSTEM_WORD_ERROR, currentNumberLine, numError, label);
                return check;
            }
            check = ifRegister(label);/* check that the label is not a register name*/
            if(check != CORRECT){
                print_errors(LABEL_REGISTER_ERROR, currentNumberLine, numError, label);
                return check;
            }
            break;
        }


    return check;
}


int ifRegister(char* key_word) {

    int i;
    int numOfRegisters=8;
    const int registersArray[] = {R0,R1,R2,R3,R4,R5,R6,R7};
    const char *(registers_key_word[]) = {"r0","r1","r2","r3","r4","r5","r6","r7"};

    for(i=0; i<numOfRegisters; i++)
    {
        if(!strcmp(key_word, registers_key_word[i]))
        {
            return registersArray[i];
        }
    }
    return FALSE;
}/*END ifRegister*/



int getKeyWord (char* key_word) {

    /*declarations*/
    /*sizes*/
    int i;

    /*key word arrays*/
    int  numOfKeyWords=16,numOfGuidWords=7;
    const char *opcodes_key_word[] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","hlt"};
    const char *guidence_key_words[] = {"label","mcr","data","string","entry","extern","define"};

    /*opcode arrays*/
    const int opcodesArray[] = {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RTS,HLT};
    const int guidenceWordArray[] = {LABEL,MACRO,DATA,STRING,ENTRY,EXTERN,DEFINE};

    /*All possible scenarios*/
    for(i=0; i<numOfKeyWords; i++)
    {
        if(!strcmp(key_word, opcodes_key_word[i]))
        {
            return opcodesArray[i];
        }
    }

    for(i=0; i<numOfGuidWords ; i++)
    {
        if(!strcmp(key_word, guidence_key_words[i]))
        {
            return guidenceWordArray[i];
        }
    }

    return FALSE;
}/*END getKeyWord;*/

void switchOperands(int *sourceOperandMethod,int *targetOperandMethod,int *targetOperand,int *sourceOperand) {/*switch value of source operand and target operand*/

    int tempMethod,tempValue;

    tempMethod=*sourceOperandMethod;
    *sourceOperandMethod=*targetOperandMethod;
    *targetOperandMethod=tempMethod;

    tempValue=*sourceOperand;
    *sourceOperand=*targetOperand;
    *targetOperand=tempValue;

}






void freeMacroTableNodes(mcNode *headMacroLine)
{
    mcNode *temp=NULL;
    /*Go through the whole line*/
    while(headMacroLine->next  != NULL)
    {
        temp = headMacroLine;
        headMacroLine = headMacroLine->next;  /* Update head pointer to next node*/
        free(temp);  /* Free the memory of the head node*/
    }

}/*END free_symbol_table_nodes*/


void freeSymbolTableNodes(Schart *headS)
{
    Schart *temp=NULL;
    /*Go through the whole line*/
    while(headS->next != NULL)
    {
        temp = headS;
        headS = headS->next;  /* Update head pointer to next node*/
        free(temp);  /* Free the memory of the head node*/
    }

}/*END free_symbol_table_nodes*/


void freeExtTableNodes(extChart *headEx)
{
    extChart *temp=NULL;
    /*Go through the whole line*/
    while(headEx->next != NULL)
    {
        temp = headEx;
        headEx = headEx->next;  /* Update head pointer to next node*/
        free(temp);  /* Free the memory of the head node*/
    }

}/*END free_symbol_table_nodes*/