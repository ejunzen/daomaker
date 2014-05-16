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

/*
int main(int argc, char* argv[]){
	char* target = (char*) malloc (1024);
	memset(target,0,1024);
	getUperName("hotel_subway_info",target);
	printf("%s\n",target);
}
*/
