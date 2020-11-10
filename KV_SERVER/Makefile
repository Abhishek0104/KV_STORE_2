CFLAG = -g -Wall -w

all: server

server: KVServer.o ConfigReader.o KVThreads.o KVCache.o KVStore.o
	g++ $(CFLAG) -o $@ $^ -lpthread

KVServer.o: KVServer.cpp 
	g++ $(CFLAG) -c $< -lpthread

ConfigReader.o: ConfigReader.cpp KVHeader.h
	g++ $(CFLAG) -c $< -lpthread

KVThreads.o: KVThreads.cpp
	g++ $(CFLAG) -c $< -lpthread

KVCache.o: KVCache.cpp
	g++ $(CFLAG) -c $< -lpthread

KVStore.o: KVStore.cpp
	g++ $(CFLAG) -c $< -lpthread
	
clean:
	rm *.o server

