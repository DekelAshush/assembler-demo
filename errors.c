#include <stdio.h>
#include <string.h>
#include "errors.h"


void print_errors(int error_type, int num_of_line, int *exeptions, char *note)
{ 
	static int num_of_errors = 0;
  
	switch(error_type)
    	{ 
		case GET_NUM_OF_ERRORS: 
            		*exeptions = num_of_errors;
            		break;
	
		case EQUATE_TO_ZERO: 
            		num_of_errors = 0;
            		break;

		/*file*/
        	case NAME_ERROR: 
            		printf("ERROR: '%s' file name is incorrect.\n", note);
					num_of_errors++;
            		break;
 
 		case DONT_EXIST_FILE_ERROR: 
            		if(num_of_errors == CORRECT){
                        printf("ERROR: '%s' file is not exist.\n", note);
                    }
				num_of_errors++;
            		break;

        /*macro*/
        case MACRO_NAME_INCORRECT:
                printf("line %d: macro name '%s' not correct.\n", num_of_line,note);
                num_of_errors++;
                break;


        case TOO_MANY_MACRO_ARGUMENTS:
            printf("line %d: too many macro arguments.\n", num_of_line);
                num_of_errors++;
                break;

        case END_MACRO_ARGUMENTS:
            printf("line %d:endmcr has arguments.\n", num_of_line);
                num_of_errors++;
                break;

		case MACRO_SYSTEM_WORD_ERROR:
            printf("line %d: The label '%s' is belongs to the system.\n",  num_of_line, note);
                num_of_errors++;
                break;

        case MACRO_REGISTER_ERROR:
            printf("line %d: The label '%s' is a register to the system.\n",  num_of_line, note);
                num_of_errors++;
                break;

		case MACRO_ALREADY_EXIST_ERROR:
			printf("The macro '%s' that ends at line %d: is already exist please change its name.\n",note,num_of_line);
				num_of_errors++;
				break;


		/*line*/
		case SYNTAX_ERROR:
			/*remove the '\n' [enter]*/
			if(note[strlen(note) - 1] == '\n')
			{
				note[strlen(note) - 1] = '\0';
			}
            		printf("line %d: syntax error <%s>\n", num_of_line, note);
					num_of_errors++;
            		break;

		case TOO_LONG_LINE_COMMAND_ERROR: 
            		printf("line %d: too long line.\n", num_of_line);
					num_of_errors++;
            		break;

		/*data*/
			case NO_DATA_ARGUMENTS:
				printf("line %d: data is missing arguments.\n", num_of_line);
			num_of_errors++;
			break;

			case NOT_ENOUGH_DATA:
				printf("Data limit is 4096 words and you passed it.\n");
			num_of_errors++;
			break;

			case NOT_EXIST_ADRESS_IN_DATA:
				printf("The adress '%d'you were looking for is not exist in your data.\n",num_of_line);
			num_of_errors++;
			break;

			case TWO_COMMAS_IN_ROW_ERROR:
				printf("line %d: data has two commas in a row.\n",num_of_line);
			num_of_errors++;
			break;


		
		/*label*/
		case LABEL_ERROR_NAME:
			printf("line %d:the label '%s' have to start with lower or upper letters.\n", num_of_line, note);
			num_of_errors++;
			break;

		case LENGTH_LABEL_ERROR:
			printf("line %d: too long label.\n", num_of_line);
			num_of_errors++;
            		break;
      		
		case LABEL_DONT_EXIST: 
            printf("line %d: '%s' label not founded in symbol chart while in code\n", num_of_line, note);
			num_of_errors++;
            		break;
		
		case LABEL_ALREADY_EXIST_ERROR:
			printf("line %d: the label '%s' already exist.\n",  num_of_line, note);
			num_of_errors++;
            		break;
		
 		case LABEL_SYSTEM_WORD_ERROR:
			printf("line %d: The label '%s' is belongs to the system.\n",  num_of_line, note);
			num_of_errors++;
            		break;
        case LABEL_REGISTER_ERROR:
                printf("line %d: The label '%s' is a register name.\n",  num_of_line, note);
                num_of_errors++;
                break;
		case WARNING_LABEL:
			printf("WARNING: line %d: The label '%s' is before .define or .extern so it has no meaning .\n",  num_of_line, note);
			break;/*warning but dont count errors cuz program can continue with the warning(not a error)*/

		case LABEL_MACRO_ERROR:
			printf("line %d: There is already a macro with the label name:'%s'.\n",  num_of_line, note);
			num_of_errors++;
			break;


		/*Symbol*/
		case SYMBOL_ALREADY_EXIST_ERROR:
			printf("line %d: the symbol '%s' already exist.\n",  num_of_line, note);
		num_of_errors++;
		break;

		case DEFINE_ERROR_NAME:
			printf("line %d:the define '%s' have to start with lower or upper letters.\n", num_of_line, note);
		num_of_errors++;
		break;

		case DEFINE_ALREADY_EXIST_ERROR:
			printf("line %d: the define '%s' already exist.\n",  num_of_line, note);
		num_of_errors++;
		break;

		case DEFINE_SYSTEM_WORD_ERROR:
			printf("line %d: The define '%s' is belongs to the system.\n",  num_of_line, note);
		num_of_errors++;
		break;

		case DEFINE_REGISTER_ERROR:
			printf("line %d: The define '%s' is a register name.\n",  num_of_line, note);
		num_of_errors++;
		break;

		case DEFINE_ERROR_NUMBER:
			printf("line %d:the define constant have to be only digits.\n", num_of_line);
		num_of_errors++;
		break;

		case DEFINE_ERROR_EQUALL:
			printf("line %d:the define have to have '=' in its syntax  .\n", num_of_line);
		num_of_errors++;
		break;

		case DEFINE_ERROR_NOT_EXIST_YET:
			printf("line %d:the define '%s' have to be added before its use.\n", num_of_line,note);
		num_of_errors++;
		break;


		/*array*/
		case TOO_MANY_ARRAY_ARGUMENTS:
			printf("line %d:there are too many argument in array '%s'.\n", num_of_line,note);
		num_of_errors++;
		break;

		case NO_ARRAY_ARGUMENTS:
			printf("line %d:there isn't argument in array.\n", num_of_line);
		num_of_errors++;
		break;

		/*instruction*/
		case TOO_MANY_INSTRUCTION_ARGUMENTS:
			printf("line %d:there are too many argument in the line instruction.\n", num_of_line);
		num_of_errors++;
		break;

		case TOO_MANY_ENTRY_ARGUMENTS:
			printf("line %d:there are too many argument in the entry.\n", num_of_line);
		num_of_errors++;
		break;

		case TOO_MANY_EXTERNAL_ARGUMENTS:
			printf("line %d:there are too many argument in the external.\n", num_of_line);
		num_of_errors++;
		break;


		/*operands*/
		case TOO_MANY_OPERANDS:  
            printf("line %d: extraneous operand.\n", num_of_line);
			num_of_errors++;
            		break;

		case FEWER_OPERANDS:  
            printf("line %d: few operands than expected.\n", num_of_line);
			num_of_errors++;
            		break;

		case REGISTER_DONT_EXIST: 
            printf("line %d: register dont exist.\n", num_of_line);
			num_of_errors++;
            		break;
		
		case KEY_WORD_INCORRECT:
			printf("line %d: key word '%s' is incorrect.\n", num_of_line, note);
			num_of_errors++;
            		break;

		case EXTERNAL_ENTRY_TOGETHER_ERROR: 
            printf("line %d: '%s' already used.\n", num_of_line, note);
			num_of_errors++;
            		break;

		case SIZE_NUMBER_ERROR:
			printf("line %d: number not in range, code range values are (-2048) - 2047 and data range values are (-8192) - 8191.\n", num_of_line);
			num_of_errors++;
            		break;

		case ADDRESING_METHOD_ERROR:
			printf("line %d: Addressing method error, please check that the operand '%s' is legal.\n", num_of_line,note);
			num_of_errors++;
			break;

		/*allocation*/
		case ALLOCATION_ERROR: 
            printf("ERROR: there is a problem to allocate memory.\n");
			num_of_errors++;
            		break;




    	}/*end of switch*/

}/*end printError func*/


