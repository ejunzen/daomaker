#ifndef _MYSQL_H
#define _MYSQL_H
#include <mysql.h>
#endif

typedef struct node{
	char* column;
	char* type;
	char* comment;
	struct NODE* next;
} NODE;

void getConection(const char* host,const char* user,const char* pwd,const char* db,	int port);

NODE* getStructs(const char* tableName);

void clean(NODE* start);

void formatComment(char* source);
