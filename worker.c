#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include "worker.h"


void worker(char *filename, char* path){

	int fd, bytes, i, dotNum = 0,j, domainLength=0, k, domainNum = 0, *domainTimes, index;
	char buffer[8192]={0}, buffer2[8192]={0};
	char domain1[100]={0}, domain2[100]={0};;
	char domain[100]={0}, temp[500]={0};
	char outFile[100] = {0};
	char **domains;
	FILE* fp;


	//printf("WORKER HERE filename: %s\n",filename );

	domains = malloc(sizeof(char*));
	domainTimes = malloc(sizeof(int));

	if(strcmp(path,"")!=0){
		memset(temp,'\0',500);
		strcpy(temp, path);
		strcat(temp, "/");
		strcat(temp, filename);
		fd = open(temp, O_RDONLY);
	}
	else{
		fd = open(filename, O_RDONLY);
	}

	if(fd==-1){
		perror("open");
	}

	while ((bytes = read(fd,buffer,8191)) > 0){
		//printf("Bytes = %d\n", bytes);
		for(i=0; i<bytes-7; i++){
			if(buffer[i] !='\0'){
				if((buffer[i]=='h') && (buffer[i+1]=='t') && (buffer[i+2]=='t') &&(buffer[i+3]=='p') && (buffer[i+4]==':')
				 && (buffer[i+5]=='/') &&(buffer[i+6]=='/')){
					//printf("URL\n");
					i+=7;
					while((buffer[i]!='/') && (i<=bytes)){
						i++;
					}
					
						j=i-1;
						dotNum=0;
						domainLength=0;
						memset(domain,'\0',100);
						while(1){
							if(buffer[j]=='.'){
								dotNum++;
							}
							if((dotNum==2)||(buffer[j]=='/')){
								break;
							}
							domain[domainLength] = buffer[j];
							domainLength++;
							j--;
						}
						strcpy(domain, reverse(domain));
						//printf("domain = %s, len = %d\n",domain, (int)strlen(domain));
						index = searchDomain(domain, domains, domainNum);
						if(index==-1){
							domains = realloc(domains, (domainNum +1) * sizeof(char *));
							domains[domainNum] = malloc( (strlen(domain)+1) *sizeof(char) );
							strcpy(domains[domainNum], domain);

							domainTimes = realloc(domainTimes, (domainNum +1) * sizeof(int));
							domainTimes[domainNum] = 1;
							domainNum++;
						}
						else{
							domainTimes[index]++;
						}
				}
			}

		}

		
		memset (buffer,'\0',8192);
	}

	//for(i=0; i<domainNum; i++){
		//printf("Domain: %s, Times:%d\n",domains[i], domainTimes[i]);
	//}
	
	strcat(outFile, "./outFiles/");
	strcat(outFile, filename);
	strcat(outFile, ".out");
	fp = fopen(outFile, "w");
	if(fp==NULL){
		perror("fopen");
		exit(-1);
	}

	for(i=0; i<domainNum; i++){
		fprintf(fp, "%s %d\n",domains[i], domainTimes[i]);
	}

	fclose(fp);
}



char* reverse(char* str){//reverse a string

	int i;
	char *newStr;

	newStr = (char*) malloc((strlen(str)+1) * sizeof(char));

	for(i=strlen(str)-1; i>=0; i--){
		newStr[strlen(str)-i-1] = str[i];
	}

	return newStr;
}

int searchDomain(char *domain, char** domains, int num){//return index or -1 if does not exist
	int i;


	if(num!=0){
		for(i=0; i<num; i++){
			if (strcmp(domain, domains[i]) == 0){
				return i;
			}
		}
	}
	return -1;
}