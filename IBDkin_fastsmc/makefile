IDIR=.
#CC=gcc -std=c99
CC=gcc -W -Wall -Werror -g -std=c99
CFLAGS=-I$(IDIR)

LIBS=-lm -lpthread -ldl -lz


OBJ=IBDkin_fastsmc.o read.o tools.o print.o parallel.o

DEPS=head.h read.h tools.h print.h parallel.h

IBDkin_fastsmc:$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 

IBDkin_fastsmc.o:IBDkin_fastsmc.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

read.o:read.c $(SQLOBJ) $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

tools.o:tools.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

print.o:print.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

parallel.o:parallel.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)
.PHONY:clean

clean:
	rm -f *.o
