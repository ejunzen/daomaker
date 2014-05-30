AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=daomaker
daomaker_SOURCES=main.c opdb.c utils.c opdb.h utils.h
daomaker_LDFLAGS=$(MYSQL_LDFLAGS) -lmysqlclient
INCLUDES=$(MYSQL_CFLAGS)
