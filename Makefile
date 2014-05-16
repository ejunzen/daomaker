daomaker: main.o opdb.o utils.o
	gcc -g -L/usr/local/mysql/lib \
			-lmysqlclient opdb.o \
           utils.o main.o -lm -o daomaker
main.o: main.c
	gcc -g -I/usr/local/mysql/include -c main.c
opdb.o: opdb.c opdb.h
	gcc -g -I/usr/local/mysql/include -c opdb.c
utils.o: utils.c utils.h
	gcc -g -c utils.c
clean:
	rm -f main.o
	rm -f daomaker
install:
	cp -f daomaker /usr/local/bin/
