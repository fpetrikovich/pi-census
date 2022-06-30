#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "censusADT.h"

/* ------- Definicion del tipo de dato ------- */

struct depart {
  char * name;
  unsigned long occ;
  unsigned long unocc;
  unsigned long other;
  struct depart * next;
};

struct province {
  char * name;
  unsigned long occ;
  unsigned long unocc;
  unsigned long other;
  struct depart * first;
  struct province * next;
};

struct censusCDT {
  unsigned long occ;
  unsigned long unocc;
  unsigned long other;
  char type;
  struct province * iterP;
  struct depart * iterD;
  struct province * first;
};

/* ------- Funciones auxiliares internas ------- */

static struct depart * newDepart (struct depart * next, char * name) {
  struct depart * aux;
  char * nameAux;

  aux = malloc(sizeof(struct depart));

  if(!aux)
    return NULL;

  nameAux = malloc(strlen(name) + 1);

  if(!nameAux) {
    free(aux);
    return NULL;
  }

  aux->name = nameAux;
  strcpy(aux->name, name);

  aux->occ = aux->unocc = aux->other = 0;
  aux->next = next;

  return aux;
}

static struct province * newProvince (struct province * next, char * name, char * depart) {
  struct province * aux;
  char * nameAux;

  aux = malloc(sizeof(struct province));

  if(!aux)
    return NULL;

  nameAux = malloc(strlen(name) + 1);

  if(!nameAux) {
      free(aux);
      return NULL;
    }

  aux->first = newDepart(NULL, depart);

  if (!aux->first) {
      free(nameAux);
      free(aux);
      return NULL;
  }

  aux->name = nameAux;
  strcpy(aux->name, name);

  aux->occ = aux->unocc = aux->other = 0;
  aux->next = next;

  return aux;
}

static struct depart * rAddPersonD (struct depart * department, char * nameD, struct depart ** dToChange) {
  int c;
  if (department == NULL || (c = strcmp(department->name, nameD)) > 0)
	{
		struct depart * aux = newDepart(department, nameD);

		if (aux != NULL) //If aux == NULL then newDepart did not create a new node
		{
			*dToChange = aux;
			return aux;
		}

    *dToChange = NULL;   //Flags the error from newDepart for addPersonP
		return department;  //if department could not be added then I need to ignore it (addPersonP will indicate this happened)
	}
	if (c == 0) //meaning the department already exists
	{
		*dToChange = department;
		return department;
	}

	department->next = rAddPersonD(department->next, nameD, dToChange);
	return department;
}

static struct province * rAddPersonP (struct province * province, char * nameP, char * nameD, struct province ** pToChange) {

  int c;

  if (province == NULL || (c = strcmp(province->name,nameP)) > 0) {

    struct province * aux = newProvince(province, nameP, nameD);

    if (aux != NULL) {
      *pToChange = aux;   //Marks the province we have to change (in this case the new one)
      return aux;
    }

    *pToChange = NULL;    //Flags the error from newProvince to addPersonP
    return province;      //Returns province to keep the list unchanged
  }
  if (c == 0)
  {
    *pToChange = province;   //Marks the province we have to change
    return province;
  }

  province->next = rAddPersonP(province->next, nameP, nameD, pToChange);
  return province;
}

static int addPersonP (struct province ** first, char condition, char * nameP, struct province ** pToChange, char * nameD, struct depart ** dToChange) {

  //Goes through the list of provinces, creates new provinces if necessary, and marks province to change

  *first = rAddPersonP(*first, nameP, nameD, pToChange);

  if(*pToChange == NULL) {
    return 0;                 //No more memory
  }

  //Goes through the list of departments of the marked province, creates new departments
  //if necessary, and marks the department to change

  (*pToChange)->first = rAddPersonD((*pToChange)->first, nameD, dToChange);

  if(*dToChange == NULL) {

    return 0;                 //No more memory
  }

  return 1;             //Indicates success to apply the changes later
}

static void freeDep (struct depart * department) {

	while (department != NULL) {

		free(department->name);
		struct depart * aux = department->next;
		free(department);
		department = aux;
	}

	return;
}

/* ------- Operaciones disponibles sobre el TAD ------- */

int addPerson (char condition, char * nameP, char * nameD, censusADT c) {
  if(!c)
    return 0;

  struct province * pToChange = NULL;
  struct depart * dToChange = NULL;

  if (!addPersonP(&c->first, condition, nameP, &pToChange, nameD, &dToChange)) {
    return 0;
  }
  unsigned long aux = c->occ + c->unocc + c->other;

  if (aux > aux+1) //OVERFLOW (we check the sum so later when calculate population we avoid the overflow)
    return 0;   //A 0 return indicates that the data input MUST STOP

  switch (condition) {
    case '1':
      c->occ ++;
      dToChange->occ ++;
      pToChange->occ ++;
      break;
    case '2':
      c->unocc ++;
      dToChange->unocc ++;
      pToChange->unocc ++;
      break;
    default:
      c->other ++;
      dToChange->other ++;
      pToChange->other ++;
  }

  return 1;
}

censusADT newCensus (void) {
  
  censusADT aux;

  aux = malloc(sizeof(struct censusCDT));

  if(!aux)
    return NULL;

  aux->occ = aux->unocc = aux->other = 0;
  aux->first = aux->iterP = NULL;
  aux->iterD = NULL;
  aux->type = COUNTRY;

  return aux;
}

void newIter (censusADT c) {

  if(!c)
    return;
  c->type = COUNTRY;

}

int nextIter (censusADT c) {

  if(!c)
    return -1;

  switch (c->type) {
    case COUNTRY:
      c->type = PROVINCE;
      c->iterP = c->first;
      break;
    case PROVINCE:
      if(!c->iterP)
        return ITER_DONE;
      c->type = DEPARTMENT;
      c->iterD = c->iterP->first;
      break;
    case DEPARTMENT:
      if(!c->iterD)
        return ITER_DONE;
      if(c->iterD->next != NULL) {
        c->iterD = c->iterD->next;
      } else {
        c->type = PROVINCE;
        if(c->iterP->next != NULL) {
          c->iterP = c->iterP->next;
        } else
          return ITER_DONE;
      }
  }

  return c->type;
}

unsigned long getNextPopu (censusADT c) {

  if(!c)
    return 0;

  unsigned long aux;

  switch (c->type) {
    case COUNTRY:
      aux = c->occ + c->unocc + c->other;
      break;
    case PROVINCE:
      if(!c->iterP)
        return 0;
      aux = c->iterP->occ + c->iterP->unocc + c->iterP->other;
      break;
    case DEPARTMENT:
      if(!c->iterD)
        return 0;
      aux = c->iterD->occ + c->iterD->unocc + c->iterD->other;
  }

  return aux;
}

float getNextIndex (censusADT c) {

  if(!c)
    return -1.0;

  float aux;

  switch (c->type) {
    case COUNTRY:
      if(c->occ + c->unocc != 0)
        aux = c->unocc / (float) (c->occ + c->unocc);
      else
        aux = 0.0;
      break;
    case PROVINCE:
      if(!c->iterP)
        return -1.0;
      if(c->iterP->occ + c->iterP->unocc != 0)
        aux = c->iterP->unocc / (float) (c->iterP->occ + c->iterP->unocc);
      else
        aux = 0.0;
      break;
    case DEPARTMENT:
      if(!c->iterD)
        return -1.0;
      if(c->iterD->occ + c->iterD->unocc != 0)
        aux = c->iterD->unocc / (float) (c->iterD->occ + c->iterD->unocc);
      else
        aux = 0.0;
  }

  return aux;
}

char * getNextName (censusADT c) {

  if(!c)
    return NULL;

  char * aux;
  switch (c->type) {
    case COUNTRY:
      aux = NULL;
      break;
    case PROVINCE:
      if(!c->iterP)
        return NULL;
      aux = c->iterP->name;
      break;
    case DEPARTMENT:
      if(!c->iterD)
        return NULL;
      aux = c->iterD->name;
  }
  return aux;
}

void freeCensus (censusADT census) {

  if(!census)
    return;

	census->iterP = census->first;

	while (census->iterP != NULL)
	{
		free(census->iterP->name);
		freeDep(census->iterP->first);
		struct province * aux = census->iterP->next;
		free(census->iterP);
		census->iterP = aux;
	}

	free(census);

	return;
}
