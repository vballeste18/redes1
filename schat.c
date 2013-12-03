/*
 C socket server example, handles multiple clients using threads
 Compile
 gcc server.c -lpthread -o server
 */

#include <ctype.h> 
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> 
#include "listas.h"
#define MAXLINE 512

Lista l; // lista con los fd de cada socket cliente
Lista salas; // lista con las salas disponibles del server

void *connection_handler(void *);
// funcion con los comandos
int comandos(int, char*);

int main(int argc, char *argv[]) {
	/////////////////////////////////////////////////Validacion de argumentos////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	char *sala = "actual"; // sala por defecto
	int port = 8888;
	int code_getopt;
	int index, contador = 0;
	opterr = 0;

	while ((code_getopt = getopt(argc, argv, "p:s:")) != -1)
		switch (code_getopt) {
		case 's':
			sala = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			contador++;
			break;
		case '?':
			if (optopt == 's' | optopt == 'p')
				fprintf(stderr, "Opcion -%c requiere un argumento.\n", optopt);
				else if (isprint(optopt))
				fprintf(stderr,
						"Opcion no valida `-%c'.\nOpciones validas: -p : puerto -s : sala \n",
						optopt);
			else
				fprintf(stderr, "Caracteres desconocidos `\\x%x'.\n", optopt);
				return 1;
				default:
				abort();
			}
	if (port == 0) {
		printf("Puerto no valido\n");
		return 1;
	}
	if (contador < 1) {
		printf("Introduzca el puerto: -p (puerto)\n");
		return 0;
	}
	printf("port = %d, sala = %s\n", port, sala);

	for (index = optind; index < argc; index++)
		printf("Non-option argument %s\n", argv[index]);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int socket_desc, client_sock, c, reads = 0, i = 0;
	struct sockaddr_in server, client;
	char buffer[MAXLINE];

	//Crear socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1) {
		printf("No se pudo crear el socket");
	}
	puts("Socket creado");

	//Creamos la estructura del socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		//Comprobamos el bind.
		perror("Fallo bind. Error");
		return 1;
	}
	puts("Bind listo");

	//Listen
	listen(socket_desc, 3);

	//Esperamos y aceptamos las conexiones entrantes.
	puts("Esperando por conexiones entrantes...");
	c = sizeof(struct sockaddr_in);

	pthread_t thread_id;
	// Creamos la lista de usuarios, y en el server la lista de salas.
	l = crearLista(l);
	salas = crearLista(salas);
	salas = insertarNodo(salas, 0, "server");
	salas.primero = insertarSala(salas.primero, sala);
	while ((client_sock = accept(socket_desc, (struct sockaddr *) &client,
			(socklen_t*) &c))) {

		if ((reads = recv(client_sock, buffer, 50, 0)) <= 0) {
			printf("Error recibiendo informacion");
		}

		buffer[reads] = '\0';
		strcat(buffer, "\n");
		puts("Conexion aceptada, ");

		//Crear una lista de client_sock, para guardar el socket de cada cliente al cual se le acepto la conexion
		l = insertarNodo(l, client_sock, buffer);
		NODO *usu = buscarUsuario(l, client_sock);
		usu = insertarSala(usu, sala);
		imprimir(l);

		if (pthread_create(&thread_id, NULL, connection_handler,
				(void*) &client_sock) < 0) {
			perror("No se pudo crear hilo");
			return 1;
		}
		puts("Manejador asignado al hilo");
		//memset(buffer,0,strlen(buffer)+1);
	}

	if (client_sock < 0) {
		perror("Fallo Accept");
		return 1;
	}

	return 0;
}

// comandos
int comandos(int sock, char* client_message) {
	char *token, sal[10] = "salida";
	char * mensaje;

	mensaje = (char*) malloc(strlen(client_message) + 1);
	strcpy(mensaje, client_message);
	token = strtok(client_message, " ");
	printf("mensaje pasado:%s\n", mensaje);
	// Comando fue, desconecta al cliente que lo envio del server.
	if (0 == strncmp("fue", token, 3)) {

		write(sock, sal, strlen(sal));
		l = eliminarNodo(l, sock);

		// Comando sal, imprime en el cliente las salas disponibles.
	} else if (0 == strncmp("sal", token, 3)) {

		char *sala, error[60];
		cajaSala *aux = salas.primero->salas;
		strcpy(error, "El servidor no posee salas en este momento.\n\0");
		if (aux == NULL) {
			write(sock, error, strlen(error));

			// Indica si el server no tiene salas creadas.
		} else {
			while (aux != NULL) {
				sala = getSala(aux);
				write(sock, sala, 50);
				sleep(1);
				aux = aux->sig;
			}
		}
		// Imprime en el cliente la lista de usuarios conectados al servidor.
	} else if (0 == strncmp("usu", token, 3)) {
		char *auxN;
		NODO *aux = l.primero;
		while (aux != NULL) {
			auxN = getUsuario(aux);
			printf("auxN: '%s'", auxN);
			write(sock, auxN, strlen(auxN));
			aux = aux->sig;
			//memset(nombre,0,50);
		}
		// Manda un mensaje a todos los usuarios conectados a las salas del cliente.
	} else if (0 == strncmp("men", token, 3)) {

		// acceder a un lista de sock para enviar el mensaje a todos los clientes que esten conectados
		int fdSock;
		unsigned i = 0;
		NODO *aux = l.primero, *actual = buscarUsuario(l, sock);
		char msg[100], *listuser = getUsuario(actual), *user = (char*) malloc(
				sizeof(char) * strlen(listuser));
		strcpy(user, listuser);

		while (i != strlen(user)) {
			if (user[i] == '\n') {
				user[i] = ' ';
			} else {
				user[i] = user[i];
			}
			i++;
		}
		// Se le agrega el nombre del usuario que lo mando.
		snprintf(msg, sizeof(msg), "%sdice: %s\n", user, &client_message[4]);
		while (aux != NULL) {
			fdSock = getElem(aux);
			if (fdSock != sock) {
				if (buscarEnSalas(actual, aux) == 1)
					write(fdSock, msg, strlen(msg));
			}
			aux = iterador(aux);
		}
		// Desuscribe al cliente de todas las salas de chat.
	} else if (0 == strncmp("des", token, 3)) {

		char borrar[50] = "Se ha desuscrito de todas las salas con exito.\n\0";
		NODO *aux = buscarUsuario(l, sock);
		aux = borrarSalas(aux);
		write(sock, borrar, strlen(borrar));

		// Crea una sala de chat en el servidor.
	} else if (0 == strncmp("cre", token, 3)) {

		char error[100];
		strncpy(error,
				"La sala no puede tener un nombre vacio o que empieze por espacio.\n",
				strlen(error) + 1);

		if ((strcmp(&mensaje[4], "") == 0) || (strcmp(&mensaje[4], " ") == 0)
				|| (strlen(&mensaje[4]) == 0)) {
			write(sock, error, strlen(error));
		} else {

			salas.primero = insertarSala(salas.primero, &mensaje[4]);
			imprimirSalas(salas.primero);
			write(sock, "Sala creada\n\0", 15);
		}

		// Elimina la sala del servidor y de los usuarios que la tengan.
	} else if (0 == strncmp("eli", token, 3)) {

		NODO *aux = l.primero;
		char *sala = (char *) malloc(strlen(mensaje));
		strcpy(sala, &mensaje[4]);
		strcat(sala, "\n");

		while (aux != NULL) {

			aux = borrarSala(aux, sala);
			imprimirSalas(aux);
			aux = aux->sig;
		}
		salas.primero = borrarSala(salas.primero, sala);

		// Suscribe al cliente a una sala de chat.
	} else if (strncmp(token, "sus", 3) == 0) {

		char *sala = (char *) malloc(strlen(mensaje) + 1);
		NODO *aux = buscarUsuario(l, sock);
		strcpy(sala, &mensaje[4]);
		strcat(sala, "\n");
		int i = buscarSala(salas.primero, sala);

		if (i == 1) {

			aux = insertarSala(aux, &mensaje[4]);
			imprimirSalas(aux);
			write(sock, "Registrado en la sala\n\0", 25);

		} else {
			write(sock, "Sala no encontrada\n\0", 25);
			printf("sala no encontrada");

		}

		// Opciones desconocidas.
	} else {
		printf("comando no valido: %s\n", token);
		write(sock, "Comando no valido.\n\0", 25);
		;
	}
	fflush(stdout);
	//memset(mensaje,0,strlen(mensaje)+1);
	//memset(token, '\0', sizeof(token) );
		}

	/*
	 * Funcion que manejas las conexiones de cada cliente.
	 * */
void *connection_handler(void *socket_desc) {
	//Obtiene el descriptor del socket
	int sock = *(int*) socket_desc;
	int read_size;
	char sal[10] = "salida", *message, client_message[2001], *token;

	//Recibe el mensaje del servidor
	while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		//Marca el final del mensaje con un caracter especial.
		client_message[read_size] = '\0';
		comandos(sock, client_message);
	}

	// Al desconectarse el cliente cierra el socket.
	close(sock);

	if (read_size == 0) {

		puts("Cliente desconectado.");
		fflush(stdout);

	} else if (read_size == -1) {

		perror("Fallo el recv.");

	}

	return 0;
}
