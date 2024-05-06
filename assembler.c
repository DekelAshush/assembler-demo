
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "errors.h"
#include "processProgram.h"



int main(int argc, char *argv[]){
	int success = CORRECT, i;

	if (argc == 1)
	{
		printf("ERROR: there is no file in the input\nPlease insert an assembly file. \n");
		exit(0); 
	}
	for(i = 1; i < argc; i++)
	{

		success = processProgram(argv[i]);/*the proccess done succussefuly.*/
		if(success == FAILED)
		{
			printf("\n'%s' file failed!\n\n", argv[i]);
			print_errors(EQUATE_TO_ZERO, off, off, NULL);
		}
	} /*end for loop*/ 	

return success;
}/*END main() */


