websocket: bin/Libsocket.c
	gcc -o websocket bin/Libsocket.c -L /usr/local/lib/ -l websockets -l bcm2835

clean: 
	rm ./bin/*.o
