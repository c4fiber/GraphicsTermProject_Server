server: main.o server.o
	g++ -o server main.o server.o -lpthread

main.o: main.cpp
	g++ -c -o main.o main.cpp

server.o: server.cpp
	g++ -c -o server.o server.cpp

clean:
	rm main.o server.o server