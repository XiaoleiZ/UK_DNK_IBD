IDIR=.
#CC=gcc -std=c99
CC=gcc -W -Wall -Werror -g -std=c99
CFLAGS=-I$(IDIR)

LIBS=-lm -lpthread -ldl -lz


OBJ=sumchr_IBDkin.o read.o tools.o print.o parallel.o

DEPS=head.h read.h tools.h print.h parallel.h

sumchr_IBDkin:$(OBJ) $(SQLOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(SQL)

sumchr_IBDkin.o:sumchr_IBDkin.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

read.o:read.c $(SQLOBJ) $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS) $(SQL) 

tools.o:tools.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

print.o:print.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

parallel.o:parallel.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)
.PHONY:clean

clean:
	rm -f *.o
