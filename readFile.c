
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "readFile.h"
#include "globals.h"
#include "errors.h"
#include "secondLoop.h"
#include "firstLoop.h"
#include "utils.h"

void compareMacroToLabel(mcNode *mcHead,Schart *schartHead,int *numError );

void updateDataChart(dataImg *dataIm,long *ic, long *dc);

void updateSymbolChart(Schart *schartHead,long *ic);

Schart *searchSymbolName(Schart *pmcSymbol, char *symbolName);

Schart *addToSymbolTable(Schart *pHeadSymbol, char *label, long adress, int currentNumberLine,int *numError,int keyword);

int checkDefine(Schart *schartHead,char *parameter,int currentNumberLine,int *numError);

long checkTheLabelExist(Schart *schartHead,char* label);

int isLegalLabel(Schart *schartHead,char* label,int currentNumberLine, int *numError, int guidenceWord);

int readAssemblyFile(FILE *assembly_code,mcNode *mcHead,int *numError,long *ic,long *dc,dataImg *dataIm,insImg *insIm,Schart **schartHead){

    int parameter,label_flag=off,none=0;
    int check_label,dataLineCount=0,keyword;
    int currentNumberLine = 0, typeOfSentence;
	char currLine[LINE_MAX_LENGTH],label[LABEL_MAX_LENGTH], key_word[KEY_WORD_MAX_LENGTH], operands[LINE_MAX_LENGTH];
    char temp_curr_line[LINE_MAX_LENGTH];
    fpos_t p1;
	Schart *schartTail=NULL, *pSchart=NULL;
    rewind(assembly_code);/*file.am set to start*/
    fgetpos(assembly_code,&p1);/*;p1 gets the current file location(first letter of first row)*/
	*ic=*ic +PROGRAM_NUMBER; /*how much to add ic to give him his starting address,in this program the number is 100*/

    /*start reading from assembly file*/
    while (fgets(currLine, LINE_MAX_LENGTH, assembly_code) != NULL) {/*checked row length in macroLayout so can go throw all wor without check*/
        resetVars(label,operands,key_word);
        currentNumberLine++;
    	label_flag=off;
        /*read the current line and define the type.*/

        typeOfSentence = whatLineAmI((const char *) &currLine);

        /*not an empty or note line*/
		if(typeOfSentence == INSTRUCTION_LINE || typeOfSentence == GUIDANCE_LINE){

			strcpy(temp_curr_line, currLine);

            /*get the data from the current line*/
			getCommands(temp_curr_line, label, key_word, operands, currentNumberLine,numError,FIRST);/*fill the label, the key word and the data of the line.*/
		    /*check the label*/


		    if(label[0]!= '\0'){
		    	check_label= isLegalLabel(*schartHead,label,currentNumberLine,numError,LABEL);
		    	if(check_label==SUCCESSED) {
		    		label_flag=on;
		    	}
		    }/*end check the label*/

		    if(typeOfSentence == INSTRUCTION_LINE ){

		    	if(label_flag==on) {
		    		/*if there is a label*/
		    		pSchart = addToSymbolTable(*schartHead,label,(*ic),currentNumberLine,numError,LABEL);
		    		if (pSchart!=NULL) {
		    			if(*schartHead==NULL) {
		    				*schartHead = pSchart;
		    				schartTail =pSchart;
		    			}else {
		    				/* Append new node to the end of the list*/
		    				schartTail->next = pSchart;
		    				schartTail = pSchart;
		    			}
		    		}/*label added to the symbol chart*/
		    	}

		        keyword=getKeyWord(key_word);
		        if(keyword==FALSE) {
		            print_errors(KEY_WORD_INCORRECT, currentNumberLine, numError, key_word);
		        }else{
		            (*ic) += InsImageLineCreate(*schartHead,insIm, currLine, ic,dc, keyword, operands, currentNumberLine,numError,NULL,NULL);
		        }

		    }else {/*GUIDANCE_LINE*/
		        keyword=getKeyWord(&key_word[1]);/*send key word without '.' (point)*/
		        if(keyword==FALSE) {
		            print_errors(KEY_WORD_INCORRECT, currentNumberLine, numError, key_word);
		        }else{
		            if(keyword==DEFINE){
		                if(label_flag==off) {/*cant have a define and label in the same row*/
		                    parameter= checkDefine(*schartHead, operands,currentNumberLine,numError);/*operands will get the name of the parameter, and parameter will get it value*/

		                	if(parameter!=WRONG) {
								if(parameter<MIN_NUM_14_BIT || parameter>MAX_NUM_14_BIT) {/*you can add 14bit number to data but if you use it, it will send error*/
									print_errors(SIZE_NUMBER_ERROR,currentNumberLine,numError,NULL);
									parameter=EOF;
								}
		                        if(operands[0]!='\0') {/*if define is set good add it to symbol chart*/
		                            pSchart = addToSymbolTable(*schartHead,operands,parameter,currentNumberLine,numError,DEFINE);
		                        	if (pSchart!=NULL) {
		                        		if(*schartHead==NULL) {
		                        			*schartHead = pSchart;
		                        			schartTail =pSchart;
		                        		}else {
		                        			/* Append new node to the end of the list*/
		                        			schartTail->next = pSchart;
		                        			schartTail = pSchart;
		                        		}
		                        	}
		                        }
		                    }


		                }else {
		                    print_errors(SYNTAX_ERROR,currentNumberLine,numError,label);
		                }
		            }else if(keyword==STRING || keyword==DATA ){
		            			if(label_flag==on) {/*if there is a label*/
		            				pSchart = addToSymbolTable(*schartHead,label,(*dc),currentNumberLine,numError,DATA);
		            				if (pSchart!=NULL) {
		            					if(*schartHead==NULL) {
		            						*schartHead = pSchart;
		            						schartTail =pSchart;
		            					}else {
		            						/* Append new node to the end of the list*/
		            						schartTail->next = pSchart;
		            						schartTail = pSchart;
		            					}
		            				}
		            			}/*label added to the symbol chart*/

		            	dataLineCount = DataImageLineCreate(dataIm,*schartHead, currLine, dc, keyword, operands, currentNumberLine,numError);
		            	if(dataLineCount!=EOF) {
		            		*dc += dataLineCount;
		            	}
		            }else if(keyword==EXTERN ||keyword==ENTRY ) {
		            		if(label_flag==on) {
		            			print_errors(WARNING_LABEL, currentNumberLine, numError, label);
		            		}
		            		if(keyword==EXTERN) {
									pSchart = addToSymbolTable(*schartHead,operands,0,currentNumberLine,numError,EXTERN);
									if (pSchart!=NULL) {
										if(*schartHead==NULL) {
											*schartHead = pSchart;
											schartTail =pSchart;
										}else {
											/* Append new node to the end of the list*/
											schartTail->next = pSchart;
											schartTail = pSchart;
										}
									}
		            		}
		            }else if(label_flag==on){
		            				/* schartHead=*/
		            				pSchart = addToSymbolTable(*schartHead,operands,*ic,currentNumberLine,numError,LABEL);
		            				if (pSchart!=NULL) {
		            					if(*schartHead==NULL) {
		            						*schartHead = pSchart;
		            						schartTail =pSchart;
		            					}else {
		            						/* Append new node to the end of the list*/
		            						schartTail->next = pSchart;
		            						schartTail = pSchart;
		            					}
		            				}
		            }
				}/*end of guidance line*/
		    }/*end if(typeOfSentence == INSTRUCTION_LINE)*/
		}/*end if(typeOfSentence == INSTRUCTION_LINE || typeOfSentence == GUIDANCE_LINE)*/
    }/*END while(!feof(assembly_code)) loop */

	compareMacroToLabel(mcHead,*schartHead,numError);/*checks that there isnt macro with the same name as label else send error*/

	if(*ic-PROGRAM_NUMBER+*dc>MAX_INPUT){
		print_errors(NOT_ENOUGH_DATA, off, numError, NULL) ;
	}

	if(mcHead != NULL) {/*free macro table*/
		freeMacroTableNodes(mcHead);
	}

    /*if we found error stop loopone and return false*/
    print_errors(GET_NUM_OF_ERRORS, off, &none, NULL) ;
    if(none!=0) {

        return FAILED;
    }

	updateSymbolChart(*schartHead, ic);/*add all data segment ic number*/
	updateDataChart(dataIm,ic,dc);

    return SUCCESSED;
}


void compareMacroToLabel(mcNode *mcHead,Schart *schartHead,int *numError ) {
	Schart *tempHead= schartHead;
	while(mcHead!=NULL) {
		while(tempHead != NULL) {
			if(strcmp(tempHead->feature,"define"))/*if feature is not a define,define and macro can have same name*/ {
				if(!strcmp(tempHead->symbol,mcHead->macroName)) {/*if macro name equall to label*/
					print_errors(LABEL_MACRO_ERROR,off,numError,mcHead->macroName);
					return;
				}
			}
			tempHead = tempHead -> next;
		}
		tempHead=schartHead;/*reset pointer*/
		mcHead = mcHead -> next;
	}
}

void updateDataChart(dataImg *dataIm,long *ic, long *dc){
	int i;
	for(i=0; i < *dc ; i++) {
		dataIm[i].address= dataIm[i].address +*ic;

	}
}

void updateSymbolChart(Schart *schartHead,long *ic) {
		while(schartHead != NULL) {
			if(!strcmp(schartHead -> feature, "data")) {
				schartHead -> address+= *ic;

			}
			schartHead = schartHead -> next;
		}/*end while loop*/
}




int checkDefine(Schart *schartHead,char *parameter,int currentNumberLine,int *numError) {
    int i = 0 , equall_flag = off,paramLength=0,check_param,result;
    char paramName[LABEL_MAX_LENGTH],temp_curr_param[LABEL_MAX_LENGTH];

    strcpy(temp_curr_param, parameter);/*using temp to go throuw the string*/
    strcat(temp_curr_param, "\0");

    PARAMETER_WHITE_SPACE_SKIP;
    while(temp_curr_param[i] != ' ' && temp_curr_param[i] != '\n' && temp_curr_param[i] != '\t' && temp_curr_param[i] != '=') {
        paramName[paramLength]= temp_curr_param[i];
        paramLength++;
        i++;
    }
    paramName[paramLength] = '\0';

    check_param= isLegalLabel(schartHead,paramName,currentNumberLine,numError,DEFINE);/*The syntax of the constant is the same as the syntax of the labels*/
    if(check_param!=SUCCESSED) {/*throws error if needed*/
        return WRONG;/*there is an error go to the next line*/
    }

    strcpy(parameter,paramName);/*parameter get the constant name if legal*/
    paramName[0] = '\0';
    paramLength = 0;

    PARAMETER_WHITE_SPACE_SKIP;
    if(temp_curr_param[i] == '='){
        i++;
        equall_flag = on;
    }/*finish the name constant now need to get only digits*/

    if(equall_flag==on){
        PARAMETER_WHITE_SPACE_SKIP;
        while(temp_curr_param[i] != ' ' && temp_curr_param[i] != '\n' && temp_curr_param[i] != '\t' && temp_curr_param[i] != '\0' ) {
        	if((temp_curr_param[i]=='+' || temp_curr_param[i]=='-') && paramLength==0) {
        		paramName[paramLength] = temp_curr_param[i];
        		paramLength++;
        		i++;
        	}
        	if(isdigit(temp_curr_param[i])){
        		paramName[paramLength] = temp_curr_param[i];
        		paramLength++;
        		i++;
        	}else {
        		print_errors(DEFINE_ERROR_NUMBER,currentNumberLine,numError,NULL);
        		return WRONG;
        	}
        }
    }else {/*no equall sign syntax error*/
        print_errors(DEFINE_ERROR_EQUALL,currentNumberLine,numError,NULL);
        return WRONG;
    }

	paramName[paramLength] = '\0';
    result=atoi(paramName);
    return result;
}


Schart *addToSymbolTable(Schart *pHeadSymbol, char *label, long adress, int currentNumberLine,int *numError,int keyword){

	int check_label,i=0;
	int label_length=0;
	char temp_curr_label[LABEL_MAX_LENGTH];

    Schart *curr_symbol_line;
    curr_symbol_line= searchSymbolName(pHeadSymbol,label);/*checking if there are 2 label with same name*/


    if(curr_symbol_line==NULL){/*there isnt symbol with same name*/
        /*allocation*/
        curr_symbol_line = (Schart *) malloc(sizeof(Schart));

        if (curr_symbol_line == NULL) {
            print_errors(ALLOCATION_ERROR, currentNumberLine, numError, NULL);
            printf("Memory allocation failed for new sChart node\n");
            return NULL;
        }
    }else {
        print_errors(LABEL_ALREADY_EXIST_ERROR, currentNumberLine, numError, curr_symbol_line->symbol);
        return NULL;
    }

	check_label= isLegalLabel(pHeadSymbol,label,currentNumberLine,numError,ARR);/*we already checked that its not in symbol chart*/
	if(check_label!=SUCCESSED) {
		return NULL;
	}

	switch (keyword) {
		case DEFINE:
			/*adds the symbol and the value*/
			strcpy(curr_symbol_line -> symbol, label);
			strcpy(curr_symbol_line -> feature, "mdefine");
			curr_symbol_line->address = adress;/*the value of the define*/
			break;

		case LABEL:
			/*adds the symbol and the value*/
			strcpy(curr_symbol_line -> symbol, label);
			strcpy(curr_symbol_line -> feature, "code");
			curr_symbol_line->address = adress;
			break;

		case DATA:
			/*adds the symbol and the value*/
			strcpy(curr_symbol_line -> symbol, label);
			strcpy(curr_symbol_line -> feature, "data");
			curr_symbol_line->address = adress;
			break;

		case EXTERN:

			LABEL_WHITE_SPACE_SKIP;
			while(label[i] != ' ' && label[i] != '\n' && label[i] != '\t' && label[i] != '\0') {
				temp_curr_label[label_length]= label[i];
				label_length++;
				i++;
			}
			LABEL_WHITE_SPACE_SKIP
			if(label[i]!='\0') {
				print_errors(TOO_MANY_EXTERNAL_ARGUMENTS,currentNumberLine,numError,&label[0]);
			}
			temp_curr_label[label_length]='\0';

			/*adds the symbol and the value*/
			strcpy(curr_symbol_line -> symbol, temp_curr_label);
			strcpy(curr_symbol_line -> feature, "external");
			curr_symbol_line->address = adress;/*the value of the define*/
		break;
	}

	curr_symbol_line->next =NULL;
	return curr_symbol_line;
}



