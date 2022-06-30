#ifndef  __censusADT_h
#define __censusADT_h

#define COUNTRY 0
#define PROVINCE 1
#define DEPARTMENT 2
#define ITER_DONE 3

/* ------- Definicion del tipo de dato ------- */

typedef struct censusCDT * censusADT;

/* ------- Operaciones disponibles sobre el TAD ------- */

/* Funcion: Crea un nuevo censo
 * Uso: census = newCensus();
 * Crea un censo e incializa los datos,
 * si no hay memoria devuelve NULL
 */
censusADT newCensus(void);

/* Funcion: Agrega los datos de una persona al censo
 * Uso: addPerson(condition, provinceName, departmentName, census);
 * Actualiza los datos del censo con los datos de la persona ingresada,
 * retorna 0 si falla, 1 si tuvo exito.
 */
int addPerson(char condition, char * nameP, char * nameD, censusADT c);

/* Funcion: Libera la mememoria ocupada por el censo
 * Uso: freeCensus(census);
 * Libera toda la memoria del heap usada por el TAD y sus operaciones
 */
void freeCensus(censusADT census);

/* Funcion: Inicializa los iteradores
 * Uso: newIter(census);
 * Luego de su uso las funciones getNext... retornaran los datos del censo a nivel pais.
 * Se debe usar 1 vez antes de iterar sobre el ADT
 */
void newIter(censusADT c);

/* Funcion: Apunta el iterador a la siguiente provincia/departamento
 * Uso: int flag = nextIter(census);
 * La iteracion recorre el ADT en el siguiente orden:
 * Pais -> Provincia -> Sus departamentos -> Provincia -> Departamentos-> ... hasta finalizar
 * Esta funcion retorna PROVINCE si lo siguiente es provincia,
 * DEPARTMENT si es un departamento o -1 si termina la iteracion.
 */
int nextIter(censusADT c);

/* Funcion: Retorna cantidad de habitantes
 * Uso: getNextPopu(census);
 * Retorna cantidad de habitantes del pais/provincia/departamento siguiente en la iteracion
 */
unsigned long getNextPopu(censusADT c);

/* Funcion: Retorna indice de desocupacion
 * Uso: getNextIndex(census);
 * Retona indice de desocupacion del pais/provincia/departamento siguiente en la iteracion
 */
float getNextIndex(censusADT c);

/* Funcion: Devuelve el nombre de la siguiente provincia/departamento
 * Uso: name = getNext(census);
 * Devuelve el nombre de la provincia/departamento siguiente en la iteracion,
 * si el iterador esta ubicado en los datos a nivel pais retorna NULL.
 */
char * getNextName(censusADT c);


#endif
