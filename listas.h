/* Archivo listas.h
 * Contiene las definiciones utilizadas en la aplicacion frecpal
 */

#include <stdio.h>
#include <stdlib.h>

#define MAXNOMBRE 20

typedef struct caj {
	char sala[20];
	struct caj *sig;
} cajaSala;
/*Estructura base para crear la lista */
typedef struct elem {
	int id;
	char usuario[30];
	cajaSala *salas;
	struct elem *sig;
} NODO;

/*Tipo lista con apuntador a ultimo y primero */
typedef struct list {
	NODO *ult;
	NODO *primero;
} Lista;

/* Prototipos de funciones definidas en listas.c */
extern Lista crearLista(Lista);
extern Lista insertarNodo(Lista, int, char *);
extern Lista eliminarNodo(Lista, int);
extern NODO *iterador(NODO *);
extern int getElem(NODO *);
extern char *getUsuario(NODO *);
extern void imprimir(Lista);
extern Lista eliminarLista(Lista);
extern NODO *insertarSala(NODO *, char *);
extern int buscarSala(NODO *, char *);
extern NODO *borrarSala(NODO *, char *);
extern NODO *borrarSalas(NODO *);
extern void imprimirSalas(NODO *);
extern NODO *buscarUsuario(Lista, int);
extern char *getSala(cajaSala *);
extern int buscarEnSalas(NODO *, NODO *);

