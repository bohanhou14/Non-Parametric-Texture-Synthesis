CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

# create executable(incomplete)
project: image.o ppm.o  texture_synthesis.o project.o
	$(CC) image.o ppm.o  texture_synthesis.o project.o -o project -lm

# create object file for testing linked list functions
image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

# create object file for linked list functions
ppm.o: ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c

texture_synthesis.o: texture_synthesis.c texture_synthesis.h image.h
	$(CC) $(CFLAGS) -c texture_synthesis.c texture_synthesis.h image.h -g

project.o: project.c
	$(CC) $(CFLAGS) -c project.c

# clean up
clean:
	rm -f *~ *.o project
