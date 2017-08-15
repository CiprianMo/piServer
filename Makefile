websocket: bin/Libsocket.c
	gcc -o websocket bin/Libwebsocket -L /usr/local/lib/ -l websockets

clean: 
	rm ./bin/*.o
