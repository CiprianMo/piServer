websocket: src/Libsocket.c src/darray.c src/hashmap.c src/bstrlib.c
	gcc -g -Wall -std=c11 -o websocket src/Libsocket.c  -L /usr/local/lib/ -l websockets -l bcm2835 src/darray.c -I./src src/hashmap.c -I./src src/bstrlib.c -I./src -lcurl

clean: 
	rm ./bin/*.o
