websocket: bin/Libsocket.c
	gcc -o websocket bin/Libsocket.c -L /usr/local/lib/ -l websockets -l bcm2835
server: bin/Libsocketserver.c
	gcc -o server bin/Libsocketserver.c -L /user/local/lib/ -l websockets
clean: 
	rm ./bin/*.o
