#include "census.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void) 
{
	char s[MAX_LINE + 1];
  	int num, ignored = 0, fail = FALSE;
  	struct person person;

  	censusADT census = newCensus();

  	if(!census)
    		printf("Error: Memory allocation\n");


  	do {

    		num = getReg(s);

    		if (num == 1) 
		{
      			if (verifyLine(s)) 
			{
        			if (!createPerson(s, &person)) {
            				fail = TRUE;
       				} 
				else {
          				if(!addPerson(person.condition,person.province,person.department,census)) 
					{
            					fail = TRUE;
            					printf("Fatal Error: Data input canceled.\n");    //Either lack of memory or overflow
          				}
        			}

        			freePerson(&person);
      			} 
			else {
        			ignored++;   //Invalid line
      			}
    		} 
		else if (num == 0)
      			ignored++;    //Couldn't get register from source

  	} while (num != -1 && !fail);

  	if (ignored)
		printf("Ignored lines: %d\n", ignored);

  	createCSV(census);
  	freeCensus(census);
  	return 0;
}

int verifyLine (char * line) 
{
	int comas=0; 	//Identifies the stage in the CSV
	int error=0; 	//flag for errors
  	int i=0;

	while (line[i] != 0 && !error) {
		if (line[i] != ',') {
			switch (comas) {
				case 0:
				{
					if ((line[i] < '0' || line[i] > '3') || line[i+1] != ',')
						error = 1;
					break;
				}
				case 1:
				{
					if (!isdigit(line[i]))
						error = 1;
					break;
				}
				default:
				{
					if (line[i] < 0 || line[i] > 127)
						error = 1;
					break;
				}
			}
		} else {
			if (comas >= 1 && line[i-1] == ',')
				error = 1; //if there are two comas together (missing value or error in typing)
      			comas++;
		}
		i++;
	}

  //Extra case, ends with the 3rd comma (1,357,Department,)

  	if (!line[i] && line[i-1] == ',')
    		error = 1;

	return !error; //if there was an error then it return 0 (FALSE) and if there wasnt it returns 1 (TRUE)
}

int createPerson (char * line, struct person * person) 
{
	int lengthD; //department name length

	person->condition = *line; //first character is the condition
	person->department = person->province = NULL;    
                                                        
	line += 2; //jumps to the start of the home code

	do
		line++;
	while (*(line-1) != ','); //skips to the start of the department since the home code is not necessary

	if ((person->department = copyString(line, &lengthD))==NULL)
	{
		return 0; //free at the end of the main
	}
	line += lengthD; //points to the start of the province name

	if ((person->province = copyString(line, &lengthD))==NULL) //lengthD will do nothing in this case
	{
		return 0;
	}

	return 1;
}

char * copyString(char * line, int *dim) 
{
	int i=0;
	char *copy;

	if ((copy = (char *)malloc(MAX_LENGTH)) == NULL) //no department/province will exceed MAX_LENGTH characters
		return NULL;

	while (line[i] !=',' && line[i] != 0)
	{
		copy[i] = line[i];
		i++;
	}

	copy[i] = 0;

	*dim = i+1;

	copy = realloc(copy, i+1); //will never return NULL because it is always shrinking the vector

	return copy; //this vector will be released once added to the census
}

void freePerson (struct person * person) 
{
	free(person->province);
	free(person->department);
	return;
}

int getReg(char * s) 
{
	int c,i;
  	int flag = FALSE; //Indicates if the Register ended
  	int ncomma = 0;	 //Counts the ammount of commas

  	for(i = 0 ; !flag && (i < MAX_LINE ) && (c = getchar()) != EOF ; i++)
	{
    		s[i] = c;

    		if (c == ',') {
      			ncomma++;
      			if (ncomma > 3) 	//if there are more than 3 comas, ignore line
        			i = MAX_LINE;
    		} else if (c == '\n')
      			flag = TRUE;
  	}

  	if (!flag && i >= MAX_LINE && c != EOF) 
	{
    		while((c=getchar()) != '\n' && c != EOF) ;   //Go to next line
    		return 0;
  	}

  	if (ncomma == 3) {
    		if (flag) {
        		s[i-1] = 0;	//It steps on the existing '\n'
    		} else if (c == EOF)
      			s[i] = 0;

    		return 1;
  	} else if (c == EOF)
    		return -1;

  	return 0;
}

int createCSV(censusADT c) 
{
  	if(!c)
    		return 0;

  	int fc = 0, fp = 0, fd = 0;
  	newIter(c);

  	FILE * fileC;
  	fileC = fopen("./Pais.csv","wt");

  	if (fileC == NULL)
    		printf("Error: Can not open/create Pais.csv\n");
  	else
    		fc = 1;

  	if (fc) {
    		fprintf(fileC,"%lu, %.2f\n", getNextPopu(c), getNextIndex(c));
		fclose(fileC);
    		printf("Pais.csv successfully loaded.\n");
  	}

  	FILE * fileP;
  	fileP = fopen("./Provincia.csv","wt");

  	if (fileP == NULL)
    		printf("Error: Can not open/create Provincia.csv\n");
  	else
    		fp = 1;

  	FILE * fileD;
  	fileD = fopen("./Departamento.csv","wt");

  	if (fileD == NULL) {
    		printf("Error: Can not open/create Departamento.csv\n");
    		fclose(fileP);
  	} else
    		fd = 1;

  	int flag = COUNTRY;
  	char * prov;

  	while (flag != ITER_DONE && (fd || fp)) {

    		flag = nextIter(c);

    		if (flag == PROVINCE) {
      			prov = getNextName(c);
      			
			if (fp)
        			fprintf(fileP,"%s, %lu, %.2f\n", prov, getNextPopu(c), getNextIndex(c));
    		}
    		else if (flag == DEPARTMENT) {
      			if (fd)
        			fprintf(fileD,"%s, %s, %lu, %.2f\n", prov, getNextName(c), getNextPopu(c), getNextIndex(c));
    		}

  	}

  	if (fp) {
		fclose(fileP);
    		printf("Provincia.csv successfully loaded\n");
  	}
  	if (fd) {
		fclose(fileD);
    		printf("Departamento.csv successfully loaded\n");
  	}

  	return 1;
}
