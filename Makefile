websocket: bin/Libsocket.c
	gcc -g -Wall -o websocket bin/Libsocket.c -L /usr/local/lib/ -l websockets -l bcm2835
server: bin/Libsocketserver.c
	gcc -g -Wall -o server bin/Libsocketserver.c -L /user/local/lib/ -l websockets
clean: 
	rm ./bin/*.o
