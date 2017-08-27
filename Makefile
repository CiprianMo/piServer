websocket: src/Libsocket.c src/darray.c src/hashmap.c src/bstrlib.c
	gcc -g -Wall -o websocket src/Libsocket.c  -L /usr/local/lib/ -l websockets -l bcm2835 src/darray.c -I. src/hashmap.c -I. src/bstrlib.c -I.

clean: 
	rm ./bin/*.o
