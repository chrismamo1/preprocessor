CC = g++
CFLAGS = -g -Wall -std=c++11 -O3
LFLAGS = -lpthread

main: main.cpp KMeans.hpp edge.o gradient.o filter.o image.o point.o color.hpp ColorRgb.hpp
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o main

%.o: %.cpp %.hpp
	$(CC) -c $(CFLAGS) $^

clean:
	rm *.o main