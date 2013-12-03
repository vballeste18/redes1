# Makefile para aplicacion "sschat, cchat"
# Uso: ver comentarios en "arbol.c"
#CFLAGS =	-pg 
CFLAGS =	-ggdb -pthread -g  -Wextra
MODULES = schat cchat 
OBJECTS = listas.o

all:	$(MODULES)

$(MODULES) : $(OBJECTS) 

%.o:	$@.c $@.h

clean:
	-@rm -f *.o $(MODULES)