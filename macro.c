#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "globals.h"
#include "errors.h"
#include "firstLoop.h"
#include "secondLoop.h"
#include "utils.h"


void printFileContent(FILE *fileAssembly,FILE *filemmcro, fpos_t p1, fpos_t p2);

mcNode *searchMacroName(mcNode *pmcMac, char *macroName);

void freeMacroTableNodes(mcNode *headMacroLine);

mcNode *addEntryToMacroTable(mcNode *pHeadMac,fpos_t start, fpos_t end,char *macroName,int currentNumberLine, int *numError,int *flagErrorFound);

int isLegalMacro(char* macro,int currentNumberLine, int *numError);

int macroLayout(FILE *assembly_code,char *fileName,int *numError,mcNode **mcHead){

    /*declarations*/
    FILE *obj_file;
    char curr_object_name[MAX_FILE_NAME];

    int legalMacro,flagErrorFound=off,flagEndMcr=off;
    long point;
    int macro_flag=off,copyMacroFlag = off, currentNumberLine = 0, typeOfSentence;
    char currLine[LINE_MAX_LENGTH], macro[LABEL_MAX_LENGTH], operands[LINE_MAX_LENGTH];
    char label[LABEL_MAX_LENGTH], key_word[KEY_WORD_MAX_LENGTH],temp_curr_line[LINE_MAX_LENGTH];
    fpos_t p1, p2,pStartMacro,pEndMacro;
    mcNode *mcTail = NULL, *pmcNode = NULL;

    rewind(assembly_code);
    fgetpos(assembly_code, &p1);/*;p1 gets the current file location(first letter of first row)*/

    /*create the current file name with the correct suffix*/
    strcpy(curr_object_name, fileName);
    point= strchr(curr_object_name,'.') -curr_object_name;/*find the position of '.' in file name*/

    curr_object_name[point]='\0';
    strcat(curr_object_name,".am");    /*instead of "file".as now its "file".am  */

    obj_file = fopen (curr_object_name, "w");
    if (obj_file == NULL) {
        print_errors(DONT_EXIST_FILE_ERROR, currentNumberLine, numError,curr_object_name);
        return FAILED;
    }
    /*start reading from assembly file*/
    while (!feof(assembly_code)) {
        copyMacroFlag=off;
        flagEndMcr=off;

        p2 =checkLength(assembly_code,&p1,&p2,numError,&currentNumberLine);
        fsetpos(assembly_code, &p2);/*current file gets p2 location(new row that is legal by length)*/
        /*read the current line and define the type.*/
        if (fgets(currLine, LINE_MAX_LENGTH, assembly_code) != NULL) {/*80 char +'\n' +fgets put '\0' at the end of the line so if there are 80 chars he need 1 more place for '\0'*/

            typeOfSentence = whatLineAmI((const char *) &currLine);

            if (typeOfSentence == GUIDANCE_LINE || typeOfSentence == NOTE_LINE) {
                fprintf(obj_file, "%s", currLine);
            }else if (typeOfSentence == INSTRUCTION_LINE) {

                strcpy(temp_curr_line, currLine);

                /*get the data from the current line*/
                getCommands(temp_curr_line, label, key_word, operands,currentNumberLine,numError,MACRO);/*fill the label, the key word and the data of the line.*/

                if(*mcHead!=NULL && flagErrorFound==off){ /*we copied macro and checking we found its place only if there are no errors*/

                    /*pmc=mcHead*/
                    pmcNode = searchMacroName(*mcHead,key_word);
                    if(pmcNode!= NULL){
                        fgetpos(assembly_code,&p1);/*;p1 gets the current file location(macro call)*/
                        printFileContent(assembly_code,obj_file, pmcNode->start_macro, pmcNode->end_macro);/*prints the macro where we say a macro call*/
                        fsetpos(assembly_code, &p1);/*;p1 sets the current file location(macro call)*/
                        copyMacroFlag=on;
                    }
                }

                if(strcmp(key_word, "mcr")== SUCCESSED ){/*if macro line found*/

                    macro_flag=on;

                    legalMacro= isLegalMacro(operands,currentNumberLine,numError);/*check all possible errors*/
                    strcpy(macro, operands);

                    if(legalMacro!=SUCCESSED){
                        flagErrorFound=on;
                    }/*if found an error stop program and delete am file*/

                    while(macro_flag==on){/*found macro start searching for endmcr */

                        fgetpos(assembly_code, &pStartMacro);/*;pMacro gets the current file location(start of macro code)*/

                        resetVars(label,operands,key_word);

                        p2 =checkLength(assembly_code,&p1,&p2,numError,&currentNumberLine);/*get the next line while checking its length*/

                        fsetpos(assembly_code, &p2);/*/*current file gets p2 location(new row that is legal by length)*/

                        /* found mcr now searching for endmcr*/
                        while(flagEndMcr==off) {/*while you didnt find endmcr keep going to the next row*/

                            /*read the current line and define the type.*/
                            fgets(currLine, LINE_MAX_LENGTH ,assembly_code);/*80 char +'\n' +fgets put '\0' at the end of the line so if there are 80 chars he need 1 more place for '\0'*/
                            strcpy(temp_curr_line, currLine);

                            /*get the data from the current line*/
                            getCommands(temp_curr_line, label, key_word, operands,currentNumberLine,numError,MACRO);/*fill the label, the key word and the data of the line.*/

                            if(strcmp(key_word, "endmcr") == SUCCESSED) {

                                fsetpos(assembly_code,&pEndMacro);/*the current filegetsp EndMacro location(starts of endmcr)*/

                                if (operands[0] != '\0')/*there are arguments after endmcr*/
                                {
                                    flagErrorFound = on;
                                    print_errors(END_MACRO_ARGUMENTS, currentNumberLine, numError, NULL);
                                }

                                pmcNode = addEntryToMacroTable(*mcHead,pStartMacro, pEndMacro, macro, currentNumberLine, numError, &flagErrorFound);

                                if (pmcNode !=NULL){
                                    /* Check if this is the first node*/
                                    if (*mcHead == NULL) {
                                        *mcHead = pmcNode;  /* Set head*/
                                        mcTail = pmcNode;  /* Set tail*/
                                    } else {
                                        /* Append new node to the end of the list*/
                                        mcTail->next = pmcNode;
                                        mcTail = pmcNode;
                                    }
                                }
                                /*********************/
                                flagEndMcr=on;
                                macro_flag = off;
                                /* flagFinishMcr=on;*/
                            }

                            if(flagEndMcr==off){

                                resetVars(label,operands,key_word);
                                p2 =checkLength(assembly_code,&p1,&p2,numError,&currentNumberLine);/*get the next line while checking its length*/

                                fsetpos(assembly_code, &p2);/*current file gets p2 location(new row that is legal by length)*/

                                fgetpos(assembly_code, &pEndMacro);/*;pEndMacro gets the current file location*/
                            }

                        }
                    }

                }/*END if(strcmp(key_word, "mcr")== SUCCESSED)*/

                if(copyMacroFlag==off){
                    /*if key_word=mcr*/
                    if(strcmp(key_word, "endmcr") != SUCCESSED) {
                        fprintf(obj_file, "%s" , currLine);
                    }else {
                        while ((fgetc(assembly_code)) != '\n' && !feof(assembly_code));/*until the file get to the next row after endmcr*/
                    }
                }
                resetVars(label,operands,key_word);

            }/*END if (typeOfSentence == INSTRUCTION_LINE) */


        }

    }/*END while(!feof(assembly_code))loop*/


     if(flagErrorFound==on){/*if there are errors in macro syntax delete file and stop process*/
         fclose(obj_file);
         remove(curr_object_name);
         return FAILED;
     }

    fclose(obj_file);
    printf("macro layout went well\n\n");
    return SUCCESSED;
}

int isLegalMacro(char* macro,int currentNumberLine, int *numError){

    int check_macro=SUCCESSED;

    if(!islower(macro[0]) && !isupper(macro[0]) ){/*checks first letter is a letter,not number or sign (i=0)*/
        print_errors(MACRO_NAME_INCORRECT, currentNumberLine, numError, macro);
        return FAILED;
    }


    check_macro= checkMacro(macro);
    if(check_macro!=CORRECT){
        print_errors(TOO_MANY_MACRO_ARGUMENTS, currentNumberLine, numError, NULL);
        return check_macro;
    }


    check_macro = getKeyWord(macro);/* check that the label is not as guidance and assembly function names*/
    if(check_macro != CORRECT){
        print_errors(LABEL_SYSTEM_WORD_ERROR, currentNumberLine, numError, macro);
        return check_macro;
    }


    check_macro = ifRegister(macro);/* check that the label is not a register name*/
    if(check_macro != CORRECT){
        print_errors(LABEL_REGISTER_ERROR, currentNumberLine, numError, macro);
        return check_macro;
    }

    return check_macro;
}


mcNode *addEntryToMacroTable(mcNode *pHeadMac,fpos_t start, fpos_t end,char *macroName,int currentNumberLine, int *numError,int *flagErrorFound){
    mcNode *curr_macro_line;
    curr_macro_line= searchMacroName(pHeadMac,macroName);/*checking if there are 2 macro with same name*/

    if(curr_macro_line==NULL){/*there isnt macro with same name*/
        /*allocation*/
        curr_macro_line = (mcNode *) malloc(sizeof(mcNode));

        if (curr_macro_line == NULL) {
            print_errors(ALLOCATION_ERROR, currentNumberLine, numError, NULL);
            printf("Memory allocation failed for new macro node\n");
            return NULL;
        }
        curr_macro_line->oneTime=0;
    }

        /*adds the macro and the pointers*/
        strcpy(curr_macro_line -> macroName, macroName);

        curr_macro_line->start_macro=start;
        curr_macro_line->end_macro= end;
        curr_macro_line -> next = NULL;
        curr_macro_line->oneTime++;

        if(curr_macro_line->oneTime==TRUE){/*checking if it appears only one time*/
            return curr_macro_line;
        }


    print_errors(MACRO_ALREADY_EXIST_ERROR, currentNumberLine, numError, macroName);
    *flagErrorFound=on;
    return NULL;

}/*END add_entry_to_macro_table*/





mcNode *searchMacroName(mcNode *pmcMac, char *macroName)
{
    /*go through the whole Macro table */
    while(pmcMac != NULL)
    {
        if(strcmp(pmcMac -> macroName, macroName)==SUCCESSED) {/*we found the macro */
            return pmcMac;
        }
        pmcMac = pmcMac -> next;
    }

    return NULL;
}/*END searchMacroName*/

void printFileContent(FILE *fileAssembly,FILE *fileMacro, fpos_t p1, fpos_t p2) {
    int ch;

    fsetpos(fileAssembly,&p1);
    while (p1.__pos!= p2.__pos) {
        ch = fgetc(fileAssembly);/*Read a character from the file*/
        fputc(ch, fileMacro);        /* Print the character*/
        fgetpos(fileAssembly,&p1);
    }
}/*END printFileContent*/





/*
        int ch;
        while ((ch = fgetc(assembly_code) )!= '\n' && !feof(assembly_code)) {
            putchar(ch);
};until the file get to the next row after endmcr*/
