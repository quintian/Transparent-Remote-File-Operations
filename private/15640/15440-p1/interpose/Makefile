all: mylib.so server

server: server.c
	gcc server.c -o server -Wall -I/../include -L../lib -ldirtree -lc

mylib.o: mylib.c
	gcc -Wall -fPIC -DPIC -c mylib.c

mylib.so: mylib.o
	ld -shared -o mylib.so mylib.o -ldl

clean:
	rm -f *.o *.so

