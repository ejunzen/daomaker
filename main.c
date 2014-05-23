#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "opdb.h"
#include "utils.h"

static char* host = NULL;
static char* user = NULL;
static char* pwd  = NULL;
static char* db   = NULL;
static char* table= NULL;

static char* package = NULL;

void printUsage(void);
void fixArgv(void);
void genorate_model(NODE* list,char* model_name);
void genorate_param(NODE* list,char* model_name);
void genorate_dao(NODE* list,char* model_name);
void genorate_daoimpl(NODE* list,char* model_name);
void genorate_mapper(NODE* list,char* model_name,char* table_name);
void genorate_service(NODE* list,char* model_name);
void genorate_serviceimpl(NODE* list,char* model_name);

char* getJavaType(char* t);
char* getJdbcType(char* t);

const int TYPE_SIZE = 6;

char* sql_type[] = {
	"int","datetime","varchar","smallint","bigint","tinyint"
};

char* java_type[] = {
	"Integer ","Date ", "String ", "Integer ", "Long ","Integer "
};

char* jdbc_type[] = {
	"INTEGER","TIMESTAMP","VARCHAR","INTEGER","INTEGER","INTEGER"
};

void printUsage(void){
	fprintf(stdout,"DAOmaker v1.0\nejunzen@gmail.com\n");
	fprintf(stdout,"daomaker -h 192.168.2.229 -u q3boy -p123 -d mobile_service -t hotel_subway_info\n");
	return;
}

int main(int argc, char* argv[]){

	char ch;

	while ((ch = getopt(argc, argv, "vh:u:p:d:t:P:")) != EOF) {
		switch(ch){
			case 'v':
				printUsage();
				break;
			case 'h':
				host = optarg;
				break;
			case 'u':
				user = optarg;
				break;
			case 'p':
				pwd = optarg;
				break;
			case 't':
				table = optarg;
				break;
			case 'd':
				db = optarg;
				break;
			case 'P':
				package = optarg;
				break;
			default:
				break;
		}
	}	
	fixArgv();
	getConection(host,user,pwd,db,3306);

	char* model_name = (char*) malloc(1024);
	memset(model_name,0,1024);
	getUperName(table,model_name);

	NODE* list = getStructs(table);

	if(list != NULL){
		genorate_model(list,model_name);
		genorate_param(list,model_name);
		genorate_dao(list,model_name);
		genorate_daoimpl(list,model_name);
		genorate_mapper(list,model_name,table);
		genorate_service(list,model_name);
		genorate_serviceimpl(list,model_name);
	}else{
		fprintf(stderr,"can not get any info from db!");
		return 127;
	}

	return 0;
}

void fixArgv(void){
	if(host==NULL){
		host = "192.168.2.229";
	}
	if(user==NULL){
		user = "q3boy";
	}
	if(pwd==NULL){
		pwd = "123";
	}
	if(db == NULL){
		db = "mobile_service";
	}
	if(table == NULL ){
		fprintf(stderr,"please see usage:\n daomaker -v\n");
		exit(127);
	}
	if(package == NULL){
		package = "com.meituan.service";
	}
#ifdef DEBUG
	printf("host:%s\n",host);
	printf("user:%s\n",user);
	printf("pwd:%s\n",pwd);
	printf("db:%s\n",db);
	printf("table:%s\n",table);
#endif
}

void genorate_model(NODE* list,char* model_name){

#ifdef DEBUG
	printf("%s\n",model_name);
	while(list!=NULL){
		printf("column=%s,type=%s,comment=%s\n",
				list->column,list->type,list->comment);
		list = list->next;
	}
#endif
	char temp[1024];
	memset(temp,0,1024);
	strcpy(temp,model_name);
	strcat(temp,"DO.java");
	FILE* file = fopen(temp,"w");
	fprintf(file,"package %s.domain;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");

	fwrite("public class ",1,13,file);
	fwrite(model_name,1,strlen(model_name),file);
	fwrite("{\n",1,2,file);

	NODE* head = list;
	while(head!=NULL){

		memset(temp,0,1024);
		getUperName(head->column,temp);

		fwrite("\t//",1,3,file);
		fwrite(head->comment,1,strlen(head->comment),file);
		fwrite("\n",1,1,file);
		fwrite("\t",1,1,file);
		char* type = getJavaType(head->type);	
		fwrite("private ",1,8,file);
		fwrite(type,1,strlen(type),file);
		fwrite(temp,1,strlen(temp),file);
		fwrite(";\n\n",1,3,file);
		head = (NODE*) head->next;
	}

	head = list;
	while(head != NULL){

		memset(temp,0,1024);
		getUperName(head->column,temp);
		char* type = getJavaType(head->type);	

		fwrite("\tpublic void set",1,16,file);
		fwrite(temp,1,strlen(temp),file);
		fwrite("(",1,1,file);
		fwrite(type,1,strlen(type),file);
		temp[0] = temp[0]+'a'-'A';
		fwrite(temp,1,strlen(temp),file);
		fwrite("){\n",1,3,file);
		fprintf(file,"\t\tthis.%s = %s;\n\t}\n\n",temp,temp);
		temp[0] = temp[0]+'A'-'a';
		fprintf(file,"\tpublic %s get%s(){\n",type,temp);
		temp[0] = temp[0]+'a'-'A';
		fprintf(file,"\t\treturn %s;\n\t}\n\n",temp);
		head = (NODE*) head->next;
	}
	fwrite("}\n",1,2,file);

	fclose(file);
}

void genorate_param(NODE* list,char* model_name){

#ifdef DEBUG
	printf("%s\n",model_name);
	while(list!=NULL){
		printf("column=%s,type=%s,comment=%s\n",
				list->column,list->type,list->comment);
		list = list->next;
	}
#endif
	char temp[1024];
	memset(temp,0,1024);
	strcpy(temp,model_name);
	strcat(temp,"SearchParam.java");
	FILE* file = fopen(temp,"w");
	fprintf(file,"package %s.param;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");

	fwrite("public class ",1,13,file);
	fwrite(model_name,1,strlen(model_name),file);
	fprintf(file,"SearchParam{\n");

	NODE* head = list;
	while(head!=NULL){

		memset(temp,0,1024);
		getUperName(head->column,temp);

		fwrite("\t//",1,3,file);
		fwrite(head->comment,1,strlen(head->comment),file);
		fwrite("\n",1,1,file);
		fwrite("\t",1,1,file);
		char* type = getJavaType(head->type);	
		fwrite("private ",1,8,file);
		fwrite(type,1,strlen(type),file);
		fwrite(temp,1,strlen(temp),file);
		fwrite(";\n\n",1,3,file);
		head = (NODE*) head->next;
	}

	head = list;
	while(head != NULL){

		memset(temp,0,1024);
		getUperName(head->column,temp);
		char* type = getJavaType(head->type);	

		fwrite("\tpublic void set",1,16,file);
		fwrite(temp,1,strlen(temp),file);
		fwrite("(",1,1,file);
		fwrite(type,1,strlen(type),file);
		temp[0] = temp[0]+'a'-'A';
		fwrite(temp,1,strlen(temp),file);
		fwrite("){\n",1,3,file);
		fprintf(file,"\t\tthis.%s = %s;\n\t}\n\n",temp,temp);
		temp[0] = temp[0]+'A'-'a';
		fprintf(file,"\tpublic %s get%s(){\n",type,temp);
		temp[0] = temp[0]+'a'-'A';
		fprintf(file,"\t\treturn %s;\n\t}\n\n",temp);
		head = (NODE*) head->next;
	}

	fwrite("}\n",1,2,file);

	fclose(file);
}

void genorate_dao(NODE* list,char* model_name){
	char temp[1024];
	memset(temp,0,1024);
	strcat(temp,"I");
	strcat(temp,model_name);
	strcat(temp,"DAO.java");
	FILE* file = fopen(temp,"w");
	fprintf(file,"package %s.dao;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");
	fprintf(file,"import %s.domain.%sDO;\n",package,model_name);
	fprintf(file,"import %s.param.%sSearchParam;\n\n",package,model_name);

	fprintf(file,"public interface I%sDAO {\n",model_name);

	//insert
	fprintf(file,"\tpublic boolean insert(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n",model_name);

	//update
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tpublic boolean update(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n", model_name);

	//delete
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tpublic boolean delete(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n",model_name);

	//select
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tpublic List<%sDO> listByParam(%sSearchParam",model_name,model_name);
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file," %sSearchParam);\n\n",model_name);
	fprintf(file,"}\n");

	//recovery
	model_name[0] = model_name[0]+'A'-'a';

	fclose(file);
}

void genorate_service(NODE* list,char* model_name){
	char temp[1024];
	memset(temp,0,1024);
	strcat(temp,"I");
	strcat(temp,model_name);
	strcat(temp,"Service.java");
	FILE* file = fopen(temp,"w");
	fprintf(file,"package %s.service;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");
	fprintf(file,"import %s.domain.%sDO;\n",package,model_name);
	fprintf(file,"import %s.param.%sSearchParam;\n\n",package,model_name);

	fprintf(file,"public interface %sSerivce{\n",model_name);

	//insert
	fprintf(file,"\tboolean insert(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n",model_name);

	//update
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tboolean update(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n", model_name);

	//delete
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tboolean delete(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO);\n\n",model_name);

	//select
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tList<%sDO> listByParam(%sSearchParam",model_name,model_name);
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file," %sSearchParam);\n\n",model_name);
	fprintf(file,"}\n");

	//recovery
	model_name[0] = model_name[0]+'A'-'a';

	fclose(file);
}

void genorate_daoimpl(NODE* list,char* model_name){

	char temp[1024];
	memset(temp,0,1024);
	strcpy(temp,model_name);
	strcat(temp,"DAOImpl.java");
	FILE* file = fopen(temp,"w");

	fprintf(file,"package %s.dao.impl;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");
	fprintf(file,"import org.mybatis.spring.SqlSessionTemplate;\n");
	fprintf(file,"import %s.domain.%sDO;\n",package,model_name);
	fprintf(file,"import %s.param.%sSearchParam;\n\n",package,model_name);
	fprintf(file,"import %s.dao.%sDAO;\n\n",package,model_name);

	fprintf(file,"public class %sDAOImpl implements I%sDAO {\n\n",model_name,model_name,model_name);

	fprintf(file,"\tprivate final static String BASE = \"%s.%sMapper.\";\n\n",package,model_name);
	fprintf(file,"\tprivate final static String STATEMENT_INSERT = BASE + \"insert\";\n\n");
	fprintf(file,"\tprivate final static String STATEMENT_SELECT = BASE + \"listByParam\";\n\n");
	fprintf(file,"\tprivate final static String STATEMENT_UPDATE = BASE + \"update\";\n\n");
	fprintf(file,"\tprivate final static String STATEMENT_DELETE = BASE + \"delete\";\n\n");

	fprintf(file,"\tprivate SqlSessionTemplate sqlSessionTemplate;\n\n");
	fprintf(file,"\tpublic void setSqlSessionTemplate(SqlSessionTemplate sqlSessionTemplate) {\n\t\tthis.sqlSessionTemplate = sqlSessionTemplate;\n\t}\n\n");

	//insert
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic tboolean insert(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n",model_name);
	fprintf(file,"\t\tint res = sqlSessionTemplate.insert(STATEMENT_INSERT, %sDO);\n",model_name);
	fprintf(file,"\t\treturn res > 0;\n");
	fprintf(file,"\t}\n\n");

	//select
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic List<%sDO> listByParam(%sSearchParam param){\n",model_name, model_name);
	fprintf(file,"\t\treturn (List<%sDO>)sqlSessionTemplate.selectList(STATEMENT_SELECT,param);\n",model_name);
	fprintf(file,"\t}\n\n");

	//update
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic boolean update(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n", model_name);
	fprintf(file,"\t\tint res = sqlSessionTemplate.update(STATEMENT_UPDATE, %sDO);\n",model_name);
	fprintf(file,"\t\treturn res > 0;\n");
	fprintf(file,"\t}\n\n");

	//delete
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic boolean delete(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n", model_name);
	fprintf(file,"\t\tint res = sqlSessionTemplate.delete(STATEMENT_DELETE,%sDO);\n",model_name);
	fprintf(file,"\t\treturn res > 0;\n");
	fprintf(file,"\t}\n\n");

	fprintf(file,"}\n");
	model_name[0] = model_name[0]+'A'-'a';

	fclose(file);

}

void genorate_serviceimpl(NODE* list,char* model_name){

	char temp[1024];
	memset(temp,0,1024);
	strcpy(temp,model_name);
	strcat(temp,"ServiceImpl.java");
	FILE* file = fopen(temp,"w");

	fprintf(file,"package %s.service.impl;\n\n",package);

	fprintf(file,"import java.util.Date\n");
	fprintf(file,"import java.io.Serializable;\n");
	fprintf(file,"import java.util.List;\n");
	fprintf(file,"import java.util.Map;\n");
	fprintf(file,"import %s.domain.%sDO;\n",package,model_name);
	fprintf(file,"import %s.param.%sSearchParam;\n\n",package,model_name);
	fprintf(file,"import %s.service.%sService;\n",package,model_name);

	fprintf(file,"public Class %sServiceImpl implements I%sSerivce{\n\n",model_name,model_name,model_name);

	fprintf(file,"\tprivate I%sDAO",model_name); 
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file," %sDAO;\n\n",model_name);
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\tpublic void set%sDAO(I%sDAO",model_name,model_name);
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDAO) {\n\t\tthis.%sDAO= %sDAO;\n\t}\n\n",model_name,model_name,model_name);
	model_name[0] = model_name[0]+'A'-'a';

	//insert
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic boolean insert(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n",model_name);
	fprintf(file,"\t\tboolean res = %sDAO.insert(%sDO);\n",model_name,model_name);
	fprintf(file,"\t\treturn res;\n");
	fprintf(file,"\t}\n\n");

	//select
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic List<%sDO> listByParam(%sSearchParam param){\n",model_name, model_name);
	fprintf(file,"\t\treturn ",model_name);
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDAO.listByParam(param);\n",model_name);
	fprintf(file,"\t}\n\n");
	model_name[0] = model_name[0]+'A'-'a';

	//update
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic boolean update(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n", model_name);
	fprintf(file,"\t\tint res = sqlSessionTemplate.update(STATEMENT_UPDATE, %sDO);\n",model_name);
	fprintf(file,"\t\treturn res > 0;\n");
	fprintf(file,"\t}\n\n");

	//delete
	model_name[0] = model_name[0]+'A'-'a';
	fprintf(file,"\t@Override\n");
	fprintf(file,"\tpublic boolean delete(%sDO ",model_name);	
	model_name[0] = model_name[0]+'a'-'A';
	fprintf(file,"%sDO){\n", model_name);
	fprintf(file,"\t\tint res = sqlSessionTemplate.delete(STATEMENT_DELETE,%sDO);\n",model_name);
	fprintf(file,"\t\treturn res > 0;\n");
	fprintf(file,"\t}\n\n");

	fprintf(file,"}\n");
	model_name[0] = model_name[0]+'A'-'a';

	fclose(file);

}

/**
 * @brief genorete mapper file
 *
 * @param list
 * @param model_name
 */
void genorate_mapper(NODE* list,char* model_name,char* table_name){
	char temp[1024];
	memset(temp,0,1024);
	sprintf(temp,"mapper-%s.xml",table_name);
	int i=0;
	for(;i<strlen(temp);i++){
		if(temp[i]=='_'){
			temp[i] = '-';
		}
	}
	FILE* file = fopen(temp,"w");
	//xml head
	fprintf(file,"<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fprintf(file,"<!DOCTYPE mapper PUBLIC \"-//mybatis.org//DTD Mapper 3.0//EN\" \"http://mybatis.org/dtd/mybatis-3-mapper.dtd\" >\n");
	fprintf(file,"<mapper namespace=\"%s.%sMapper\" >\n",package,model_name);	
	//resultMap
	fprintf(file,"\t<resultMap id=\"BaseResultMap\" type=\"%s.domain.%sDO\">\n",package,model_name);
	NODE* head = list;
	while(head != NULL){
		memset(temp,0,1024);
		getUperName(head->column,temp);
		temp[0] = temp[0] + 'a' - 'A';
		char* type = getJdbcType(head->type);
		fprintf(file,"\t\t<result column=\"%s\" property=\"%s\" jdbcType=\"%s\"/>\n",head->column,temp,type);
		head = (NODE*) head->next;
	}
	fprintf(file,"\t</resultMap>\n\n");
	//Base_Column_List
	fprintf(file,"\t<sql id=\"Base_Column_List\">\n\t\t");
	head = list;
	while(head != NULL){
		fprintf(file,"%s",head->column);
		head = (NODE*)head->next;
		if(head!=NULL){
			fprintf(file,",");
		}
	}
	fprintf(file,"\n\t</sql>\n\n");

	//select
	fprintf(file,"\t<select id=\"listByParam\" parameterType=\"%s.param.%sSearchParam\" resultMap=\"BaseResultMap\">\n",package,model_name);
	fprintf(file,"\t\tselect <include refid=\"Base_Column_List\"/>\n");
	fprintf(file,"\t\tfrom %s where 1=1 \n",table_name);
	head = list;
	while(head != NULL){
		memset(temp,0,1024);
		getUperName(head->column,temp);
		temp[0] = temp[0] + 'a' - 'A';
		char* type = getJdbcType(head->type);
		fprintf(file,"\t\t<if test=\"%s != null\">\n",temp);
		fprintf(file,"\t\t\tand %s = #{%s,jdbcType=%s}\n",head->column,temp,type);
		fprintf(file,"\t\t</if>\n");
		head = (NODE*)head->next;
	}
	fprintf(file,"\t</select>\n");

	//update
	fprintf(file,"\t<update id=\"update\" parameterType=\"%s.domain.%sDO\">\n",package,model_name);
	fprintf(file,"\t\tupdate %s \n\t\t<set> \n",table_name);
	head = list;
	while(head != NULL){
		memset(temp,0,1024);
		getUperName(head->column,temp);
		temp[0] = temp[0] + 'a' - 'A';
		char* type = getJdbcType(head->type);
		fprintf(file,"\t\t<if test=\"%s != null\">\n",temp);
		fprintf(file,"\t\t\t%s = #{%s,jdbcType=%s},\n",head->column,temp,type);
		fprintf(file,"\t\t</if>\n");
		head = (NODE*)head->next;
	}
	fprintf(file,"\t\t</set>\n");
	memset(temp,0,1024);
	getUperName(list->column,temp);
	temp[0] = temp[0] + 'a' - 'A';
	char* type = getJdbcType(list->type);
	fprintf(file,"\t\twhere %s = #{%s,jdbcType=%s}\n",list->column,temp,type);

	fprintf(file,"\t</update>\n");

	//insert
	fprintf(file,"\t<insert id=\"insert\" parameterType=\"%s.domain.%sDO\">\n",package,model_name);
	fprintf(file,"\t\tinsert into %s(<include refid=\"Base_Column_List\" />) \n",table_name);
	fprintf(file,"\t\tvalues(\n");
	head = list;
	while(head != NULL){
		memset(temp,0,1024);
		getUperName(head->column,temp);
		temp[0] = temp[0] + 'a' - 'A';
		char* type = getJdbcType(head->type);
		fprintf(file,"\t\t\t#{%s,property=%s,jdbcType=%s}",head->column,temp,type);
		head = (NODE*)head->next;
		if(head != NULL){
			fprintf(file,",\n");
		}
	}	

	fprintf(file,"\t\t)\n");
	fprintf(file,"\t</insert>\n");

	//delete	
	fprintf(file,"\t<delete id=\"delete\" parameterType=\"java.lang.Integer\">\n");	
	fprintf(file,"\t\tdelete from %s \n",table_name);
	memset(temp,0,1024);
	getUperName(list->column,temp);
	temp[0] = temp[0] + 'a' - 'A';
	type = getJdbcType(list->type);
	fprintf(file,"\t\twhere %s = #{%s,jdbcType=%s}\n",list->column,temp,type);
		
	fprintf(file,"\t</delete>\n");

	//xml tail
	fprintf(file,"</mapper>\n");

	fclose(file);
}

char* getJavaType(char* t){
	int i=0;
	for(;i< TYPE_SIZE;i++){
		if(strstr(t,sql_type[i])==t){
			break;
		}	
	}
	return java_type[i];
}

char* getJdbcType(char* t){
	int i=0;
	for(;i< TYPE_SIZE;i++){
		if(strstr(t,sql_type[i])==t){
			break;
		}	
	}
	return jdbc_type[i];
}
