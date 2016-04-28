all: server

server: linked_list.o
	clang -g -o server linked_list.o server.c -lpthread

linked_list.o: linked_list.c linked_list.h
	clang -c linked_list.c

clean: 
	rm -rf server *.o