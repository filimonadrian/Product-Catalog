CC=g++
CFLAGS= -std=c++11 -Wall
SRC_CLIENT=client.cpp requests.cpp helpers.cpp buffer.cpp
SRC_SERVER=server.cpp helpers.cpp response.cpp buffer.cpp server_helper.cpp
SRC_TEST=test.cpp requests.cpp helpers.cpp buffer.cpp
PORT="12345"
#client: $(SRC_CLIENT)
#	$(CC) -o client $(SRC_CLIENT) -Wall

test: $(SRC_TEST)
	$(CC) -o test $(SRC_TEST) -$(CFLAGS)

server: $(SRC_SERVER)
	$(CC) -o server $(SRC_SERVER) -$(CFLAGS)
	
run_server: server
	./server $(PORT)
run_test: test
	./test $(PORT)	

clean:
	rm -f *.o test server
