#include "utils.h"

static int SIZE = 'A' - 'a';

int getUperName(const char* source,char* target){
	int len = strlen(source);
	int i = 0;
	int sign = 0;
	char temp[100];

	for(;i < len;i++){
		if(i == (len-1)){
			memset(temp,0,100);
			if(i > sign ){
				memcpy(temp,source+sign,i-sign+1);
				if(temp[0]>='a' && temp[0] <= 'z'){
					temp[0] = temp[0] + SIZE;
				}
				strcat(target,temp);
			}
		}
		if(source[i]!='_'){
			continue;
		}else{
			memset(temp,0,100);
			if(i > sign ){
				memcpy(temp,source+sign,i-sign);
				if(temp[0]>='a' && temp[0] <= 'z'){
					temp[0] = temp[0] + SIZE;
				}
				strcat(target,temp);
			}
			sign = i+1;
		}
	}
	return 1;
}

int count(const char* str,const char * delim){
    int num = 0;
    while( NULL != ( str = strstr(str,delim) ) ){
        str ++;    
        num ++;
    }            
    return num;
}

char** split(char* str, const char * del, int *len){
    int num = count(str,del);
	*len = num+1;
    char ** arr = ( char ** ) malloc( sizeof(char*) * ( num+1 ));
    char** result = arr;
    
    char * s = strtok(str,del);
    
    while( s != NULL){
        *arr++ = s;
        s = strtok(NULL,del);

    }    
    return result;
}

#if 0
int main(int argc, char* argv[]){
	char* target = (char*) malloc (1024);
	memset(target,0,1024);
	strcat(target,"a,b,c");
	int num;
	int i=0;
	char** tables = split(target,",",&num);
	printf("%d\n",num);
	return 0;
	while(tables[i]!=NULL){
		printf("%s\n",tables[i]);
		i++;
	}
}
#endif
