#gcc -Wall -pthread count_line.c -lm -lz -std=c99 -Wextra -o count_line
IDIR=.
#CC=gcc -std=c99
CC=gcc -W -Wall -Werror -g -std=c99
CFLAGS=-I$(IDIR)

LIBS=-lm -lpthread -ldl -lz

#SQL=-I/hps/software/users/birney/xiaolei/sqlite3
#SQLOBJ=/hps/software/users/birney/xiaolei/sqlite3/sqlite3.o

OBJ=ibdstat.o read.o tool.o

DEPS=head.h read.h tool.h

ibdstat:$(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 

ibdstat.o:ibdstat.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

read.o:read.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS) 

tool.o:tool.c $(DEPS)
	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

##print.o:print.c $(DEPS)
##	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)

#parallel.o:parallel.c $(DEPS)
#	${CC} -c -o $@ $< $(CFLAGS) $(LIBS)
#	.PHONY:clean
#
#	clean:
#		rm -f *.o
