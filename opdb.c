
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opdb.h"

static MYSQL mysql_conn;
NODE* head;
NODE* end;

char* keys[1024];
char* values[1024];

void getConection(const char* host,
		const char* user,
		const char* pwd,
		const char* db,
		int port){
	if(mysql_init(&mysql_conn)==NULL){
		fprintf(stderr,"error happened in init mysql connection!");
		exit(101);
	}	
	if(mysql_real_connect(&mysql_conn, host, user, pwd, db, port, NULL, 0)==NULL){
		fprintf(stderr,"error happened in connect to mysql!");
		exit(102);
	}
}

NODE* getStructs(const char* tableName){
	char* sql = (char*)malloc(1024);
	memset(sql,0,1024);
	strcat(sql,"show full fields from ");
	strcat(sql,tableName);
	MYSQL_RES *mysql_result; /* Result handle */
	MYSQL_ROW mysql_row; /* Row data */
	int f1, f2, num_row, num_col;
	mysql_query(&mysql_conn,"set names utf8");
	if (mysql_query(&mysql_conn, sql) == 0) {        
		mysql_result = mysql_store_result(&mysql_conn); // get the result from the executing select query
		num_row = mysql_num_rows(mysql_result); /* Get the no. of row */
		num_col = mysql_num_fields(mysql_result); /* Get the no. of column */

		for (f1 = 0; f1 < num_row; f1++) {
			mysql_row = mysql_fetch_row(mysql_result); /* Fetch one by one */
			NODE* node = (NODE*) malloc(sizeof(NODE));
			memset(node,0,sizeof(NODE));
			node->column = mysql_row[0];
			node->type = mysql_row[1];
			node->comment = mysql_row[8];
			if(head == NULL){
				head = node;
			}else if(head!=NULL && end == NULL ){
				head->next = node;
				end = node;
			} else {
				end->next = node;
				end = node;
			}
		}
		//mysql_free_result(mysql_result);
	}else{
		fprintf(stderr,"error in query,please check your sql!\n");
	}
	return head;
}

