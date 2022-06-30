#ifndef  __census_h
#define __census_h
#include "censusADT.h"

#define MAX_LINE 80
#define TRUE 1
#define FALSE !TRUE
#define MAX_LENGTH 78

/* ------- Definicion del tipo de dato ------- */

struct person
{
  char condition;
	char * province;
	char * department;
};

/* ------- Prototipos ------- */

int verifyLine (char * line);
int createPerson (char * line, struct person * person);
char * copyString (char * line, int * dim);
void freePerson (struct person * person);
int getReg(char * s);
int createCSV(censusADT c);

#endif
