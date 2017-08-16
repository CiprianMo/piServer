websocket: bin/Libsocket.c
	gcc -o websocket bin/Libsocket.c -L /usr/local/lib/ -l websockets bcm2835

clean: 
	rm ./bin/*.o
