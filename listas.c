/* Archivo listas.c
 Contiene funciones para el manejo de una lista doble enlazada de la
 aplicacion frecpal
 */
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include "listas.h"

/* Crear Lista
 * Crea una lista vacia
 */
Lista crearLista(Lista l) {
	l.primero = NULL;
	l.ult = NULL;
	return l;
}

/*Inserta un nodo en la lista*/
Lista insertarNodo(Lista l, int id, char *usu) {
	char *user = (char *) malloc(strlen(usu + 2) * sizeof(char));
	strcpy(user, usu);
	printf("user: %s\n", user);
//strcat(user,"\n");

	Lista aux;
	NODO *elem = (NODO *) malloc(sizeof(NODO));
	elem->id = id;
	elem->salas = NULL;
	strcpy(elem->usuario, user);
	elem->sig = NULL;

	if (l.primero == NULL) {
		l.primero = elem;
		l.ult = l.primero;
	} else {
		l.ult->sig = elem;
		l.ult = l.ult->sig;
	}
	return l;
}

// Inserta una sala en el nodo seleccionado
NODO *insertarSala(NODO *e, char *usu) {
	char *sala = (char *) malloc(strlen(usu + 2) * sizeof(char));

	cajaSala *aux, *auxant;
	strcpy(sala, usu);
	printf("sala: %s\n", sala);
	strcat(sala, "\n");

	if (e->salas->sala == NULL) {
		e->salas = (cajaSala *) malloc(sizeof(cajaSala));
		strcpy(e->salas->sala, sala);
		e->salas->sig = NULL;
	} else {
		aux = e->salas;
		while (aux != NULL) {
			if (strcmp(aux->sala, sala) == 0) {
				printf("No se agregan salas repetidas \n");
				return e;
			}
			auxant = aux;
			aux = aux->sig;
		}
		auxant->sig = (cajaSala *) malloc(sizeof(cajaSala));
		strcpy(auxant->sig->sala, sala);
		auxant->sig->sig = NULL;
		printf("El usuario se ha registrado en la sala %s\n", sala);
	}
	return (e);
}

// Busca la sala en el nodo seleccionado
int buscarSala(NODO *e, char *sala) {
	if (e->salas == NULL) {
		return 0;
	} else {
		cajaSala *aux = e->salas;
		while (aux != NULL) {
			if (strcmp(aux->sala, sala) == 0) {
				return 1;
			}
			aux = aux->sig;
		}
		return 0;
	}
}

//Borra la sala indicada
NODO *borrarSala(NODO *e, char *sala) {
	cajaSala *aux = e->salas, *auxant;
	if (aux == NULL)
		return e;
	if ((strcmp(aux->sala, sala) == 0) && (aux->sig == NULL)) {
		memset(aux->sala, 0, sizeof(aux->sala));
		e->salas = NULL;
		return e;
	} else if ((strcmp(aux->sala, sala) == 0) && (aux->sig != NULL)) {
		e->salas = aux->sig;
		aux->sig = NULL;
		memset(aux->sala, 0, sizeof(aux->sala));
		free(aux);
		return e;
	} else if ((strcmp(aux->sala, sala) != 0) && (aux->sig != NULL)) {
		auxant = aux;
		aux = aux->sig;
		while (aux != NULL) {
			if (strcmp(aux->sala, sala) == 0) {
				auxant->sig = aux->sig;
				aux->sig = NULL;
				memset(aux->sala, 0, sizeof(aux->sala));
				free(aux);
				return e;
			}
			auxant = aux;
			aux = aux->sig;
		}
	}
	printf("sala no existente");
	return e;
}

// Borra todas las salas de un nodo
NODO *borrarSalas(NODO *e) {
	cajaSala *aux = e->salas, *auxant;
	while (aux != NULL) {
		auxant = aux;
		aux = aux->sig;
		auxant->sig = NULL;
		free(auxant);
	}
	e->salas = NULL;
	return e;
}

// Imprime las salas de un nodo
void imprimirSalas(NODO *e) {
	cajaSala *aux = e->salas;
	if (aux == NULL) {
		printf("El usuario no esta suscrito a ninguna sala.\n");
		return;
	}
	while (aux != NULL) {
		printf("salas: %s \n", aux->sala);
		aux = aux->sig;
	}
	return;
}

/*Muestra la lista en el terminal */
void imprimir(Lista l) {
	NODO *aux;
	if (l.primero == NULL) {
		printf("Lista vacia.\n");
		return;
	}
	aux = l.primero;
	while (aux != NULL) {
		printf("id: %d usuario: %s \n", aux->id, aux->usuario);
		aux = aux->sig;
	}
	return;
}

// Itera en la lista
NODO *iterador(NODO *e) {
	return (e->sig);
}

// Devuelve el id de un nodo
int getElem(NODO *e) {
	return (e->id);
}

// Devuelve el usuario de un nodo
char *getUsuario(NODO *e) {
	return (e->usuario);
}

// Borra el nodo con el id indicado
Lista eliminarNodo(Lista l, int id) {
	NODO *aux, *auxant = NULL;
	aux = l.primero;
	if (l.primero->id == id) {
		l.primero = l.primero->sig;
		if (l.ult->id == id) {
			l.ult = l.ult->sig;
		}
	} else {
		while (aux->id != id) {
			auxant = aux;
			aux = aux->sig;
		}
		if (l.ult->id == id) {
			auxant->sig = NULL;
			l.ult = auxant;
		} else {
			if (auxant != NULL)
				auxant->sig = aux->sig;
		}
	}
	if (aux != NULL)
		free(aux);
	return l;
}

/*Libera el espacio de memoria utilizado por la lista */
Lista eliminarLista(Lista l) {
	NODO *aux, *auxant;
	aux = l.primero;
	while (aux != NULL) {
		if (aux->sig != NULL) {
			auxant = aux;
			aux = aux->sig;
			free(auxant);
		} else {
			free(aux);
			aux = aux->sig;
		}
	}
	l.primero = NULL;
	l.ult = NULL;
	return l;
}

// Busca el nodo con el usuario indicado en la lista
NODO *buscarUsuario(Lista l, int id) {
	NODO *aux = l.primero;
	while (aux != NULL) {
		if (aux->id == id) {
			return aux;
		}
		aux = aux->sig;
	}
	printf("Usuario no existente");
	return aux;
}

// Devuelve la sala
char *getSala(cajaSala *s) {
	return s->sala;
}

// Busca ocurrencias de alguna sala del nodo a en el b
int buscarEnSalas(NODO *a, NODO *b) {
	cajaSala *aux1 = a->salas, *aux2;
	while (aux1 != NULL) {
		aux2 = b->salas;
		while (aux2 != NULL) {
			if (strcmp(aux1->sala, aux2->sala) == 0) {
				return 1;
			}
			aux2 = aux2->sig;
		}
		aux1 = aux1->sig;
	}
	return 0;
}
