# the compiler: g++ for C++
CC = g++

# compiler flags:
CFLAGS  = -c 

# the build target executable:
target = server

$(target): dijkstra.o server.o digraph.o
	$(CC) dijkstra.o server.o digraph.o -o $(target)
dijkstra.o:
	$(CC) $(CFLAGS) dijkstra.cpp
server.o:
	$(CC) $(CFLAGS) server.cpp
digraph.o:
	$(CC) $(CFLAGS) digraph.cpp
clean:
	$(RM) $(target) digraph.o server.o dijkstra.o
