#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "globals.h"
#include "errors.h"
#include "firstLoop.h"
#include "utils.h"
#include "secondLoop.h"

void switchOperands(int *sourceOperandMethod,int *targetOperandMethod,int *targetOperand,int *sourceOperand);

int checkErrorsAddressMethod(int error,int targetOperand,int sourceOperand, int lineNumber,char *operandOne,char *operandTwo,int *numErrors) ;

int instructionToBinary(insImg *currInsPtr,Schart *head,char *operandOne,char * operandTwo,long *adress,int sourceOperand,int targetOperand,int sourceOperandMethod,int targetOperandMethod,int opcode,int numOperands);

int handleOperands(Schart *head,char *operands, int lineNumber,char *operand,char *intValue,int *numErrors) ;

Schart *addToSymbolTable(char *label, long adress, int type_of_sentence,long dc);


fpos_t checkLength(FILE *assembly_code,fpos_t *p1,fpos_t *p2,int *numError,int *currentNumberLine){

    /*declarations*/
    int flagLengthError,rowLength;

    do {
        (*currentNumberLine)++;
        flagLengthError=off;
        rowLength=0;/*(1-80 is 80 chars)*/


        (*p2)=(*p1);
       while (fgetc(assembly_code) != '\n' && !feof(assembly_code)){/*check row length is max 80*/
            rowLength++;
        }/*assembly_code now point to the row '\n' position*/
        fgetpos(assembly_code, p1); /* Get current position into *p1*/

        if (rowLength > (LINE_MAX_LENGTH)) {/*include '\n' char*/
            flagLengthError = on;
            print_errors(TOO_LONG_LINE_COMMAND_ERROR, *currentNumberLine, numError, NULL);
        }

    } while(flagLengthError==on) ;/*line length is too long, check that there are max 80 chars(81 with \n) in text if more print Error and go check the next line*/

    return *p2;
}

/*not used in the code*/
void print(dataImg *currDataPtr,long currentAdress) {
    printf(" %u", currDataPtr[currentAdress].machineCode.A_R_E);
    printf(" %u", currDataPtr[currentAdress].machineCode.addresTargetOp);
    printf(" %u", currDataPtr[currentAdress].machineCode.addresSourceOp);
    printf(" %u", currDataPtr[currentAdress].machineCode.opcode);
    printf(" %u\n", currDataPtr[currentAdress].machineCode.NONE);
}
/*not used in the code*/


int DataImageLineCreate(dataImg *currDataPtr,Schart *head,char *currLine, long *adress,int key_word, char* operands, int lineNumber, int *numErrors) {
    int dc=0;
    int i = 0 ,ascii,dataArgumentLength=0,checkOp,letter_flag=off,digit_flag=off;
    long currentAdress=*adress+dc;
    char dataValue[LABEL_MAX_LENGTH],dataName[LABEL_MAX_LENGTH],temp_curr_operand[DATA_MAX_LENGTH];/*dataname for define name and dara value store eeach val number or define*/
    unsigned int *p1;

    strcpy(temp_curr_operand, operands);/*using temp to go through the string*/
    strcat(temp_curr_operand, "\0");

    if(operands[0]=='\0') {/*there is no operands*/
         print_errors(NO_DATA_ARGUMENTS,lineNumber,numErrors,NULL);
         return EOF;
    }
    strcpy(currDataPtr[*adress].sourceCode, currLine);
    strcpy(temp_curr_operand, operands);/*using temp to go through the string*/
    strcat(temp_curr_operand, "\0");
    switch (key_word) {
        case DATA:

                OPERAND_WHITE_SPACE_SKIP;
                if(temp_curr_operand[i] == ',') {/*comma at start*/
                    print_errors(NO_DATA_ARGUMENTS, lineNumber, numErrors, NULL);
                    return EOF;
                }

                while(temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t'  && temp_curr_operand[i] != '\0'){
                    OPERAND_WHITE_SPACE_SKIP;

                    if(temp_curr_operand[i]=='+' || temp_curr_operand[i]=='-'){
                        digit_flag=on;
                        dataValue[dataArgumentLength]= temp_curr_operand[i];
                        dataArgumentLength++;
                        i++;
                    }
                    while(temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t' && temp_curr_operand[i] != ',' && temp_curr_operand[i] != '\0'){

                        if(!isdigit(temp_curr_operand[i])) {
                            if(digit_flag==off) {
                                letter_flag =on;
                            }else{/*cant be a digit and a define*/
                                print_errors(SYNTAX_ERROR, lineNumber, numErrors, operands);
                                return EOF;
                            }
                        }
                        dataValue[dataArgumentLength]= temp_curr_operand[i];
                        dataArgumentLength++;
                        i++;

                    }/*finish copy the first operand*/

                    dataValue[dataArgumentLength] = '\0';



                    if(letter_flag==on) {/*it has to be a symbol*/
                        checkOp = (int)checkTheDefineExist(head, dataValue);/*check that there is a symbol already the same name else it is an error*/
                        if(checkOp == FALSE){/*there is not a symbol in symbol chart*/
                            print_errors(DEFINE_ERROR_NOT_EXIST_YET, lineNumber, numErrors, dataValue);
                            return EOF;
                        }/*we already check define is good so dont need to do range check again*/
                        strcpy(dataName,dataValue);/*all good and its a define const put its name in operand*/
                        currDataPtr[currentAdress].address=currentAdress;
                        p1 = (unsigned int *)(&currDataPtr[currentAdress].machineCode) ;
                        *(p1) = checkOp;/*set to be first number value value*/
                        dc=dc+NEXT_ADRESS;
                        currentAdress=currentAdress+NEXT_ADRESS;

                    }else{/*its a number*/
                        checkOp=atoi(dataValue);/*transfer from string to int*/
                        if(checkOp>MIN_NUM_14_BIT && checkOp<MAX_NUM_14_BIT) {
                            /*you can add 14bit number to data but if you use it, it will send error*/
                            currDataPtr[currentAdress].address=currentAdress;
                            p1 = (unsigned int *)(&currDataPtr[currentAdress].machineCode) ;
                            *(p1) = checkOp;/*set to be first number value value*/
                            dc=dc+NEXT_ADRESS;
                            currentAdress=currentAdress+NEXT_ADRESS;
                        }else {
                            print_errors(SIZE_NUMBER_ERROR,lineNumber,numErrors,NULL);
                        }
                    }
                    OPERAND_WHITE_SPACE_SKIP;
                    if(temp_curr_operand[i] != ',' && temp_curr_operand[i] != '\0'){
                        print_errors(SYNTAX_ERROR,lineNumber,numErrors,temp_curr_operand);/* no comma*/
                    }else if(temp_curr_operand[i] == ',') {
                        i++;
                        OPERAND_WHITE_SPACE_SKIP;
                    }
                    if(temp_curr_operand[i] == ',') {/*comma after comma*/
                        print_errors(TWO_COMMAS_IN_ROW_ERROR, lineNumber, numErrors, NULL);
                        return EOF;
                    }
                    letter_flag=off;
                    digit_flag=off;
                    dataValue[0]='\0';
                    dataArgumentLength=0;
                    OPERAND_WHITE_SPACE_SKIP;

                }/*END (temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t' )*/
        break;

        case STRING:
            OPERAND_WHITE_SPACE_SKIP;
        if(temp_curr_operand[i] != '"') {
            print_errors(SYNTAX_ERROR, lineNumber, numErrors, operands);
        }
        i++;
        while(temp_curr_operand[i]!='"') {
            ascii=temp_curr_operand[i];
            currDataPtr[currentAdress].address=currentAdress;
            p1 = (unsigned int *)(&currDataPtr[currentAdress].machineCode) ;
            *(p1) = ascii;/*set to be first number value value*/
            dc=dc+NEXT_ADRESS;
            currentAdress=currentAdress+NEXT_ADRESS;

            dataValue[dataArgumentLength]= temp_curr_operand[i];
            dataArgumentLength++;
            i++;
        }

        ascii=0;/*code ascii of '\0'*/
        currDataPtr[currentAdress].address=currentAdress;
        p1 = (unsigned int *)(&currDataPtr[currentAdress].machineCode) ;
        *(p1) = ascii;/*set to be first number value value*/
        dc=dc+NEXT_ADRESS;
        currentAdress=currentAdress+NEXT_ADRESS;
        break;
    }

    return dc;
}/*end DataImageLineCreate*/



int InsImageLineCreate(Schart *head,insImg *currInsPtr,char *currLine, long *adress,long *dc, int opcode, char* operands, int lineNumber,int *numErrors,extChart **extHead,extChart **extTail) {

    int ic=0;/*ic can be int cuz max input return is 5*/
    /*put the correct value*/
    strcpy(currInsPtr[*adress].sourceCode, currLine);
    instructionBinaryLine(head, currInsPtr , opcode, operands, lineNumber,numErrors, &ic,dc, adress,FIRST,extHead,extTail);
    if(ic==EOF) {
        return 0;/*if there is an error dont count ic*/
    }

    return ic;
}





int instructionBinaryLine(Schart *head,insImg *currInsPtr, int opcode , char *operands,  int lineNumber,int *numErrors,int *ic,long *dc,long *adress,int loop,extChart **extHead,extChart **extTail)
{
    int sourceOperandMethod,targetOperandMethod,targetOperand=0,sourceOperand = 0;
    int i=0,error;
    char operadOne[KEY_WORD_MAX_LENGTH],operadTwo[KEY_WORD_MAX_LENGTH],intValue[KEY_WORD_MAX_LENGTH];

    sourceOperandMethod=handleOperands(head,operands,lineNumber,operadOne,intValue,numErrors);
    if(sourceOperandMethod!=BAD) {
        sourceOperand=atoi(intValue);/*it can be define\number\register value\value inside arr\label  */
    }else{
        *ic=EOF;
        return *ic;
    }

/**We took first operand from the operands now if there are second operand he will be taken too else targetOperandMethod will get not_exist*/

    targetOperandMethod=handleOperands(head,operands,lineNumber,operadTwo,intValue,numErrors);
    if(targetOperandMethod!=BAD) {
        targetOperand=atoi(intValue);/*it can be define\number\register value\value inside arr\label  */
    }else{
        *ic=EOF;
        return *ic;
    }
    if(operands[i] == ' ' || operands[i] == '\t' || operands[i] == '\n') {
        while(operands[i] == ' ' || operands[i] == '\t' || operands[i] == '\n') {i++;}/*skip white char*/
    }
    if(operands[i] != '\0'){
        print_errors(TOO_MANY_INSTRUCTION_ARGUMENTS,lineNumber,numErrors,NULL);
        return *ic=EOF;
    }


    if(sourceOperand<MIN_NUM_12_BIT || sourceOperand>MAX_NUM_12_BIT) {
        print_errors(SIZE_NUMBER_ERROR,lineNumber,numErrors,NULL);
        return *ic=EOF;
    }

    if(targetOperand<MIN_NUM_12_BIT || targetOperand>MAX_NUM_12_BIT) {
        print_errors(SIZE_NUMBER_ERROR,lineNumber,numErrors,NULL);
        return *ic=EOF;
    }

    if(loop==FIRST){
        switch(opcode)/*call to the correct func depened the type*/{
            case MOV:
                error = checkErrorsAddressMethod(FIRST,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED){
                    break;
                }
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,MOV,TWO_OPERANDS);
                break;

            case CMP:
                error = checkErrorsAddressMethod(SECOND,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,CMP,TWO_OPERANDS);
                break;

            case ADD:
                error = checkErrorsAddressMethod(FIRST,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,ADD,TWO_OPERANDS);
                break;


            case SUB:
                error = checkErrorsAddressMethod(FIRST,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,SUB,TWO_OPERANDS);
                break;

            case NOT:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(THIRD,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                 if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,NOT,ONE_OPERAND);
                break;


            case CLR:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(THIRD,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,CLR,ONE_OPERAND);
                break;


            case LEA:
                error = checkErrorsAddressMethod(FORTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,LEA,TWO_OPERANDS);
                break;

            case INC:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(THIRD,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,INC,ONE_OPERAND);
                break;

            case DEC:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(THIRD,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,DEC,ONE_OPERAND);
                break;

            case JMP:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(FIFTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,JMP,ONE_OPERAND);
                break;

            case BNE:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(FIFTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,BNE,ONE_OPERAND);
                break;

            case RED:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(THIRD,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,RED,ONE_OPERAND);
                break;

            case PRN:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(SIXTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,PRN,ONE_OPERAND);
                break;

            case JSR:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                error = checkErrorsAddressMethod(FIFTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,JSR,ONE_OPERAND);
                break;

            case RTS:
                error = checkErrorsAddressMethod(SEVENTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                targetOperandMethod=0;
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,RTS,ZERO_OPERANDS);
                break;

            case HLT:
                error = checkErrorsAddressMethod(SEVENTH,targetOperandMethod,sourceOperandMethod,lineNumber,operadOne,operadTwo,numErrors);
                if(error==FAILED) {
                    break;
                }
                sourceOperandMethod=0;/*need to reset for the command*/
                targetOperandMethod=0;
                *ic = instructionToBinary(currInsPtr,head,operadOne,operadTwo,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,HLT,ZERO_OPERANDS);
                break;

        }/*end switch(opcode)*/

    }else/*secondLoop*/ {
        switch(opcode)/*call to the correct func depened the type*/{
            case MOV:
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,MOV,TWO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;

            case CMP:
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,CMP,TWO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;

            case ADD:
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,ADD,TWO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;


            case SUB:
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,SUB,TWO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;

            case NOT:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,NOT,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;


            case CLR:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,CLR,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;


            case LEA:
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,LEA,TWO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;

            case INC:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,INC,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case DEC:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,DEC,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case JMP:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,JMP,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case BNE:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,BNE,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case RED:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,RED,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case PRN:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,PRN,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case JSR:
                switchOperands(&sourceOperandMethod,&targetOperandMethod,&targetOperand,&sourceOperand);/*target opperand is sent not source*/
                sourceOperandMethod=0;/*need to reset for the command*/
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,JSR,ONE_OPERAND,lineNumber,numErrors,extHead,extTail);
                break;

            case RTS:
                sourceOperandMethod=0;/*need to reset for the command*/
                targetOperandMethod=0;
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,RTS,ZERO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;

            case HLT:
                sourceOperandMethod=0;/*need to reset for the command*/
                targetOperandMethod=0;
                *ic = instructionToBinarySecondLoop(currInsPtr,head,operadOne,operadTwo,dc,adress, sourceOperand,targetOperand,sourceOperandMethod,targetOperandMethod,HLT,ZERO_OPERANDS,lineNumber,numErrors,extHead,extTail);
                break;
        }
    }

    return *ic;
}/*END createInstruction()*/

int instructionToBinary(insImg *currInsPtr,Schart *head,char *operandOne,char * operandTwo,long *adress,int sourceOperand,int targetOperand,int sourceOperandMethod,int targetOperandMethod,int opcode,int numOperands){

    int ic=0;/*can use int cuz it wont pass 5 in the function*/
    Schart *symbolNameExist;
    unsigned int *p1,temp;
    long currentAdress = *adress+ic;/*the address start with the program number */
    int arrayLength=(int)*adress+ic-PROGRAM_NUMBER ;/*i want array to start from 0 i can use int cuz address wont pass 4096 which is input_max_length*/

    /**********Instruction line code*************/
            currInsPtr[arrayLength].address=currentAdress;
            currInsPtr[arrayLength].machineCode.A_R_E=0;
            currInsPtr[arrayLength].machineCode.NONE=0;
            currInsPtr[arrayLength].machineCode.opcode=opcode-1;  /*MOV=1 ... */
            currInsPtr[arrayLength].machineCode.addresSourceOp=sourceOperandMethod;/*if TWO_OPERANDS than gets the value else gets 0,if ONE_OPERAND gets value 0*/
            currInsPtr[arrayLength].machineCode.addresTargetOp=targetOperandMethod;/*ZERO_OPERANDS both gets zero*/

            ic=ic+NEXT_ADRESS;
            currentAdress=currentAdress+NEXT_ADRESS;
            arrayLength++;


    if(numOperands==TWO_OPERANDS){
        if(sourceOperandMethod != DIRECT_REGISTER || targetOperandMethod!=DIRECT_REGISTER){
            switch(sourceOperandMethod){
                case IMMIDIATE:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = sourceOperand<< 2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    /*********************finished label now value inside the label(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = sourceOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                currInsPtr[arrayLength].machineCode.A_R_E=0;
                p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                *(p1) = sourceOperand << 5;/*set sourceOperand in bits 5-7 */

                ic=ic+NEXT_ADRESS;
                currentAdress=currentAdress+NEXT_ADRESS;
                arrayLength++;
                break;
            }
            switch (targetOperandMethod){
                case IMMIDIATE:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand << 2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandTwo);
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandTwo);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    /*********************finished label now value inside the labe(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand<< 2 ;/*set targetOperand in bits 2-4*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

            }
        }else {/*both operands are registers*/
            currInsPtr[arrayLength].address=currentAdress;
            currInsPtr[arrayLength].machineCode.A_R_E=0;
            p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
            *(p1) = sourceOperand << 5;/*set sourceOperand in bits  5-7 */
            temp = targetOperand << 2;
            *(p1) =  *(p1)+temp ;/*set targetOperand in bits 2-4 */

            ic=ic+NEXT_ADRESS;
            currentAdress=currentAdress+NEXT_ADRESS;
            arrayLength++;

        }
    }else if(numOperands==ONE_OPERAND){
        /*if TWO_OPERANDS*/
        switch(targetOperandMethod) {
                case IMMIDIATE:/*PRN CASE*/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;
                    case DIRECT:
                        currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case FIXED_INDEX:
                    currInsPtr[arrayLength].address=currentAdress;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    symbolNameExist=searchSymbolName(head,operandOne);/*searching name of array in symbolchart*/
                    if(symbolNameExist==NULL) {
                        *(p1)=EOF;/*dont exist yet in symbolchart, will be find on second loop*/
                    }else {
                        if(!strcmp(symbolNameExist->feature,"external")) {
                            currInsPtr[arrayLength].machineCode.A_R_E=1;/*external label*/
                        }else {
                            currInsPtr[arrayLength].machineCode.A_R_E=2;/*label in file Code */
                        }
                        *(p1)=symbolNameExist->address<<2;
                    }

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    /*********************finished label now value inside the labe(array)****************************/
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode);/*p1 gets pointer to thestart of the machinecode addres*/
                    *(p1) = targetOperand <<2;/*set 12 bits to be sourceOperand value*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;

                case DIRECT_REGISTER:
                    currInsPtr[arrayLength].address=currentAdress;
                    currInsPtr[arrayLength].machineCode.A_R_E=0;
                    p1 = (unsigned int *)(&currInsPtr[arrayLength].machineCode) ;
                    *(p1) = targetOperand << 2;/*set targetOperandMethod in bits 2-4*/

                    ic=ic+NEXT_ADRESS;
                    currentAdress=currentAdress+NEXT_ADRESS;
                    arrayLength++;
                    break;
            }
    }
    return ic;
}




int handleOperands(Schart *head,char *operands, int lineNumber,char *operand,char *intValue,int *numErrors) {

     int i = 0 ,operandLength=0,checkOp,letter_flag=off,digit_flag=off,end_arr_flag=off;
    char operandName[LABEL_MAX_LENGTH],temp_curr_operand[LABEL_MAX_LENGTH];

    if(operands[0]=='\0') {/*there is no operands*/
        return NOT_EXIST;
    }

    strcpy(temp_curr_operand, operands);/*using temp to go through the string*/
    strcat(temp_curr_operand, "\0");

    OPERAND_WHITE_SPACE_SKIP;
    if(temp_curr_operand[i] == ',') {i++;}
    OPERAND_WHITE_SPACE_SKIP;

    if(operands[i]=='\0') {/*there is no operands*/
        strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/
        return NOT_EXIST;
    }

    while(temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t' && temp_curr_operand[i] != ',' && temp_curr_operand[i] != '\0'){

        /**************IMMIDIATE**************/
        if(temp_curr_operand[i] =='#' && operandLength==0){/*needs to be a numebr*/
            i++;
            if(temp_curr_operand[i]=='+' || temp_curr_operand[i]=='-'){
                digit_flag=on;
                operandName[operandLength]= temp_curr_operand[i];
                operandLength++;
                i++;
            }
            while(temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t' && temp_curr_operand[i] != ',' && temp_curr_operand[i] != '\0'){

                if(!isdigit(temp_curr_operand[i]) ) {
                    if(digit_flag==off) {
                        letter_flag =on;
                    }else{/*cant be a digit and a define*/
                        print_errors(SYNTAX_ERROR, lineNumber, numErrors, operands);
                        return BAD;
                    }
                }
                operandName[operandLength]= temp_curr_operand[i];
                operandLength++;
                i++;
            }/*finish copy the first operand*/
            operandName[operandLength] = '\0';

            if(letter_flag==on) {/*it has to be a symbol*/
                checkOp = (int)checkTheDefineExist(head, operandName);/*check that there is a symbol already the same name else it is an error*/
                if(checkOp == FALSE){/*there is not a symbol in symbol chart*/
                    print_errors(DEFINE_ERROR_NOT_EXIST_YET, lineNumber, numErrors, operandName);
                    return BAD;
                }
                strcpy(operand,operandName);/*all good and its a define const put its name in operand*/
                sprintf(intValue, "%d", checkOp);/*copy the value of the define into string and put in operand one */

                strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/

                return IMMIDIATE;
            }/*its a number*/

            strcpy(operands,&temp_curr_operand[i]);/*send operand without first operand*/

            strcpy(operand,operandName);/*all good and its a number*/
            strcpy(intValue,operand);
            return IMMIDIATE;

        }/*END (temp_curr_operand[i] =='#')*/

        /**************FIXED_INDEX**************/
        if(temp_curr_operand[i] =='[' && operandLength!=0 ) {/*array*/

            operandName[operandLength] = '\0';
            checkOp = isLegalLabel(head, operandName,lineNumber,numErrors,ARR);/*check that the arr is a legal label*/
            if(checkOp != CORRECT){
                print_errors(LABEL_ALREADY_EXIST_ERROR, lineNumber, numErrors, operandName);
                return BAD;
            }
            strcpy(operand,operandName);/*all good and operand gets the name of the array*/

            operandName[0]='\0';/*reset char for int value*/
            operandLength=0;

            i++;/*current position arr[  */
            OPERAND_WHITE_SPACE_SKIP;
            while(temp_curr_operand[i] != ' ' && temp_curr_operand[i] != '\n' && temp_curr_operand[i] != '\t'  && end_arr_flag!=on){/*searching what value inside the array*/
                if((temp_curr_operand[i]=='+' || temp_curr_operand[i]=='-') && operandLength==0) {
                    operandName[operandLength] = temp_curr_operand[i];
                    operandLength++;
                    i++;
                    digit_flag=on;
                }
                if(temp_curr_operand[i] == ']') {/*if the ] is without spaces arr[ 5]*/
                    end_arr_flag=on;
                    i++;
                }

                if(!isdigit(temp_curr_operand[i]) && end_arr_flag==off) {/*has to be a define*/
                    if(digit_flag==off) {
                        letter_flag =on;
                    }else {
                        print_errors(SYNTAX_ERROR, lineNumber, numErrors, operands);
                        return BAD;
                    }
                }

                if(end_arr_flag==off) {/*if the ] is without spaces arr[ 5]*/
                    operandName[operandLength]= temp_curr_operand[i];
                    operandLength++;
                    i++;
                }
            }/*finish taking one argument from the array*/

            OPERAND_WHITE_SPACE_SKIP;
            if(temp_curr_operand[i] == ']') {/*if the ] is with spaces arr[ 5 ]*/
                end_arr_flag=on;
                i++;
            }
            if(end_arr_flag==off) {/*there is some thing else inside the array and sytax error*/
                print_errors(TOO_MANY_ARRAY_ARGUMENTS, lineNumber, numErrors, operand);
                return BAD;
            }
            operandName[operandLength] = '\0';

            if(letter_flag==on) {/*it has to be a symbol*/
                checkOp = (int)checkTheDefineExist(head, operandName);/*check that there is a symbol with the same name or else it is an error*/
                if(checkOp == FALSE){/*there is not a symbol*/
                    print_errors(DEFINE_ERROR_NOT_EXIST_YET, lineNumber, numErrors, operandName);
                    return BAD;
                }
                sprintf(intValue, "%d", checkOp);/*copy the value of the define into string and put in operand one */
                strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/
                return FIXED_INDEX;
            }

            /*its a number*/
            strcpy(intValue,operandName);/*copy the number to int value*/
            if(intValue[0]=='\0') {
                print_errors(NO_ARRAY_ARGUMENTS,lineNumber,numErrors,NULL);
                return BAD;
            }
            strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/
            return FIXED_INDEX;
        }
        operandName[operandLength]= temp_curr_operand[i];
        operandLength++;
        i++;
    }

    operandName[operandLength] = '\0';

    /**************DIRECT_REGISTER**************/
    checkOp=ifRegister(operandName);
    if(checkOp!=FALSE) {/*it is a register*/
        strcpy(operand,operandName);/*copy the register name into operand*/
        sprintf(intValue, "%d", checkOp-1);/*copy the register num into int value r0=1.... that why (-1)*/
        strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/
        return DIRECT_REGISTER;
    }

    /**************DIRECT**************/
    checkOp = isLegalLabel(head, operandName,lineNumber,numErrors,ARR);/*check that the Operand can be a label send with ARR cuz dont have to be in schart*/
    if(checkOp==SUCCESSED) {/*it is a register*/
        strcpy(operand,operandName);/*copy the label name into operand*/
        strcpy(operands,&temp_curr_operand[i]);/*now operand is without first operand*/
        return DIRECT;
    }

    /*all FAILED returm BAD*/
    return  BAD;

}



int checkErrorsAddressMethod(int error,int targetOperand,int sourceOperand, int lineNumber,char *operandOne,char *operandTwo,int *numErrors){

    switch(error)/*call to the correct func depened the type*/ {
        case FIRST:
            if(targetOperand == NOT_EXIST || sourceOperand==NOT_EXIST) {
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand == IMMIDIATE) {
                print_errors(ADDRESING_METHOD_ERROR,lineNumber,numErrors,operandOne);
                return FAILED;
            }
        break;

        case SECOND:
            if(targetOperand == NOT_EXIST || sourceOperand==NOT_EXIST) {
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }
        break;

        case THIRD:
            if(sourceOperand!=NOT_EXIST) {
                print_errors(TOO_MANY_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand==NOT_EXIST) {
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand == IMMIDIATE){
                print_errors(ADDRESING_METHOD_ERROR,lineNumber,numErrors,operandOne);
                return FAILED;
            }
        break;

        case FORTH:
            if(targetOperand == NOT_EXIST || sourceOperand==NOT_EXIST){
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand == IMMIDIATE){
                print_errors(ADDRESING_METHOD_ERROR,lineNumber,numErrors,operandOne);
                return FAILED;
            }else if(sourceOperand!=DIRECT && sourceOperand!=FIXED_INDEX){
                print_errors(ADDRESING_METHOD_ERROR,lineNumber,numErrors,operandTwo);
                return FAILED;
            }
        break;

        case FIFTH:
            if(sourceOperand!=NOT_EXIST) {
                print_errors(TOO_MANY_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand==NOT_EXIST){
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if((targetOperand != DIRECT) && (targetOperand != DIRECT_REGISTER)){
                print_errors(ADDRESING_METHOD_ERROR,lineNumber,numErrors,operandOne);
                return FAILED;
            }
        break;

        case SIXTH:
            if(sourceOperand!=NOT_EXIST) {
                print_errors(TOO_MANY_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }else if(targetOperand==NOT_EXIST) {
                print_errors(FEWER_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }
        break;

        case SEVENTH:
            if(targetOperand!=NOT_EXIST || sourceOperand!=NOT_EXIST ) {
                print_errors(TOO_MANY_OPERANDS,lineNumber,numErrors,NULL);
                return FAILED;
            }
        break;
    }
    return SUCCESSED;
}



Schart *searchSymbolName(Schart *pmcSymbol, char *symbolName)/*return the schart Node*/
{
    /*go through the whole Macro table */
    while(pmcSymbol != NULL)
    {
        if(strcmp(pmcSymbol->symbol, symbolName)==SUCCESSED) {/*we found the symbol*/
            return pmcSymbol;
        }
        pmcSymbol = pmcSymbol -> next;
    }
    return NULL;
}/*END searchMacroName*/



long checkTheLabelExist(Schart *schartHead,char* label){/*return address*/
    /*go through the whole symbol table */
    while(schartHead != NULL) {
        if(!strcmp(schartHead -> symbol, label)){/*if the label exist*/
            return schartHead -> address;
        }
        schartHead = schartHead -> next;
    }/*end while loop*/
    return FALSE;/*return 0 if didnt find label*/
}/*END check_the_label_exist()*/



long checkTheDefineExist(Schart *schartHead,char* label){/*return address*/
    /*go through the whole symbol table */
    while(schartHead != NULL) {
        if(!strcmp(schartHead -> feature, "mdefine")) {
            if(!strcmp(schartHead -> symbol, label)){/*if the label exist*/
                return schartHead -> address;
            }
            schartHead = schartHead -> next;
        }else {
            schartHead = schartHead -> next;
        }
    }/*end while loop*/
    return FALSE;/*return 0 if didnt find label*/
}/*END check_the_label_exist()*/