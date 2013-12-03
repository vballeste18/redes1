/*
 Proyecto 1
 Integrantes:
 Vanessa Balleste. 08-10091
 Gabriel Freites. 08-10383
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAXLINE   512

// Variable global que se utiliza a forma de semaforo.
int hijo;

// Proceso para recibir mensajes del servidor.
void* recibir(void* p) {
	int id, readsize;
	char buffer[MAXLINE];
	id = *((int*) p);
	while (1) {

		readsize = recv(id, buffer, MAXLINE, 0);
		buffer[readsize] = '\0';
		if (strncmp("salida", buffer, strlen(buffer) + 1) == 0) {
			hijo = 2;
			pthread_exit(NULL);
		}
		printf("%s", buffer);
		fflush(stdout);
	}
}

// Proceso para enviar mensajes al servidor.
void* enviar(void* p) {
	int* id;
	char buffer[MAXLINE];
	id = (int*) p;
	while (1) {
		if (hijo == 2) {
			pthread_exit(NULL);
		}
		fgets(buffer, MAXLINE, stdin);
		strtok(buffer, "\n");
		send(*id, buffer, strlen(buffer), 0);
		sleep(1);
	}
}

int main(int argc, char **argv) {

	/* Validacion de parametros */
	char *host = NULL;
	int port, status;
	char *nombre = NULL;
	char *archivo = NULL;
	int index;
	int c, contador = 0;
	pthread_t hilos[2];

	opterr = 0;

	while ((c = getopt(argc, argv, "h:p:n:a:")) != -1)
		switch (c) {
		case 'h':
			host = optarg;
			contador++;
			break;
		case 'p':
			port = atoi(optarg);
			contador++;
			break;
		case 'n':
			nombre = optarg;
			contador++;
			break;
		case 'a':
			archivo = optarg;
			contador++;
			break;
		case '?':
			if (optopt == 'n' | optopt == 'h' | optopt == 'p' | optopt == 'a')
				fprintf(stderr, "La opcion -%c requiere un argumento.\n",
				optopt);
				else if (isprint(optopt))
				fprintf(stderr,
						"Opcion desconocida `-%c'.\nOpciones validas: -h -p -n -a \n",
						optopt);
			else
				fprintf(stderr, "Caracter desconocido en opcion `\\x%x'.\n",
						optopt);

				return 1;
				default:
				abort();
			}
	if (contador != 4) {
		printf(
				"Introduzca todos lo argumentos : -n (nombre) -p (puerto) -h (host) -a (archivo)\n");
		return 0;
	}
	//printf("host = %s, port = %d, nombre = %s, archivo = %s\n", host, port, nombre, archivo);

	for (index = optind; index < argc; index++)
		printf("Opcion sin argumento. %s\n", argv[index]);

// Obtener tipo de servidor

	struct sockaddr_in serveraddr;
	struct hostent *hoststruct;
	int sockfd;

	// Si el argumento se puede convertir en una direccion IP lo hace

	if (inet_aton(host, &serveraddr.sin_addr)) {
		hoststruct = gethostbyaddr((char *) &serveraddr.sin_addr,
				sizeof(serveraddr.sin_addr), AF_INET);
	} else {
		printf("No se pudo convertir es una direccion IP");
		return 1;
	}

	// Abriendo el socket, SOCK_STREAM (TCP)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("No se pudo abrir el socket");
		exit(1);
	}

	//printf("socket abierto\n");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	strcat(nombre, "\0");
	//printf("nombre%ses\n", nombre);
	if ((connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)))
			< 0) {
		printf("can't connect to server \n");
	} else {
		/* Pasamos el nombre de usuario al servidor para que lo guarde en su lista de sockets */

		if (send(sockfd, nombre, strlen(nombre), 0) < 0) {
			printf("Error enviando nombre.\n\0");
		};

		//manejando archivo
		if (status = pthread_create(&hilos[0], NULL, recibir,
				(void*) &sockfd)) {
			printf("Error al crear hilo para recibir\n");
			close(sockfd);
			exit(0);
		}
		if (status = pthread_create(&hilos[1], NULL, enviar, (void*) &sockfd)) {
			printf("Error al crear hilo para enviar\n");
			close(sockfd);
			exit(0);
		}
		sleep(1);

		FILE *fp;
		char linea[1000001];
		fp = fopen(archivo, "r");
		if (fp == NULL) {
			printf("\nError de apertura del archivo.\n");
		} else {
			while (feof(fp) == 0) {

				fgets(linea, 1000000, fp);
				int length = strlen(linea);
				if (linea[length - 1] == '\n') {
					linea[length - 1] = '\0';
				}
				//printf("luego '%s'\n", linea);
				send(sockfd, linea, strlen(linea), 0);
				sleep(1);

			}
		}
		fclose(fp);

		pthread_join(hilos[0], NULL);
		pthread_join(hilos[1], NULL);
	}

	close(sockfd);

	return 0;
}

