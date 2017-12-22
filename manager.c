#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "worker.h"
#include "list.h"


struct ListNode *head;
struct ListNode *childenList;

void catchInt(int signo) {
	pid_t pid;
	unlink("myfifo");

	while(is_empty(childenList)!=1){
		kill(childenList->data->pid, SIGKILL);
		delete_head(&childenList);
	}
	kill(getpid(), SIGKILL);
}

void catchSIGCHLD(int signo) {
	pid_t pid;
	int status;
	struct Data* availableWorker;
	availableWorker = malloc(sizeof(struct Data));

	if((pid=waitpid(-1,&status,WUNTRACED|WNOHANG))>0){
		if (WIFSTOPPED(status)) {
			//availableWorker->pid = malloc(sizeof(pid_t));
			availableWorker->pid = pid;
			//availableWorker->fd;
			listInsertBeginning(&head,availableWorker);
			kill(pid, SIGSTOP);
		}
	}

}


int main(int argc, char* argv[]){
	
//Manager
	pid_t pid, pid2;
	int retval =0, bytes, fd, numOfWorkers=0;
	int p[2];
	char filename[100] = {0}, temp[500] = {0};
	char * fifo = "myfifo";

	head = (struct ListNode*) sizeof(struct ListNode);
    head=NULL;

    childenList = (struct ListNode*) sizeof(struct ListNode);
    childenList = NULL;

	sigset_t set1;
	sigemptyset(&set1);
    sigaddset(&set1, SIGINT);
	static struct sigaction act1;
	act1.sa_handler = catchInt;
	act1.sa_mask = set1;
	sigaction (SIGINT, &act1, NULL);

	sigset_t set2;
	sigemptyset(&set2);
    sigaddset(&set2, SIGCHLD);
	static struct sigaction act2;
	act2.sa_handler = catchSIGCHLD;
	act2.sa_mask = set2;
	sigaction (SIGCHLD, &act2, NULL);


	if (pipe(p) == -1){ 
		perror ("pipe call");
		return -1;
	}

	if ( mkfifo ( fifo , 0666) == -1 ){
		perror ("mkfifo" ) ;
		return -1;
	}

	mkdir("outFiles",  0777);

	pid = fork();
	if (pid == -1){
		perror ("Failed to fork ");
		return -1;
	}
	else if(pid==0){//Listener
		
		//sleep(1);

		close (p[0]);
		dup2(p[1], 1);
		close (p[1]);

		if(argc==1){
			retval = execl("/usr/bin/inotifywait","inotifywait", "-e" ,"create", "--format", "%f", "-m", "." ,NULL);
		}
		else{
			retval = execl("/usr/bin/inotifywait","inotifywait", "-e" ,"create", "--format", "%f", "-m", argv[2] ,NULL);
		}
		if (retval == -1){
			perror ("execl") ;
		}

	}
	else{//Manager
		//printf("I am Manager\n");
		while(1){
			memset(filename,'\0',100);
			bytes = read (p[0], filename, 100);
			filename[strlen(filename)-1] = '\0';
			if(bytes!=-1){
				//printf("bytes = %d\n", bytes);
				printf("New File:%s\n", filename);;


				if(is_empty(head)){
					pid2 = fork();
					if (pid2 == -1){
						perror ("Failed to fork ");
						return -1;
					}
					else if(pid2==0){//Worker

						fd = open(fifo, O_RDWR);
						struct Data* newWorker;
						newWorker = malloc(sizeof(struct Data));
						newWorker->pid = getpid();
						newWorker->fd = fd;
						listInsertBeginning(&childenList,newWorker);

						while(1){
							//fflush(stdout);
							//printf("WORKER STARTS\n");
							char filenameWorker[100] = {0};

							
							memset(filenameWorker,'\0',100);
							bytes = read(fd, filenameWorker, 100);
							if(bytes!=-1){
								//printf("I AM WORKER filename = %s len = %d\n", filenameWorker, (int) strlen(filenameWorker));

							}

							sleep(1);
							if(argc==1){
								worker(filenameWorker, "");
							}
							else{
								worker(filenameWorker, argv[2]);
							}

							kill(getpid(), SIGSTOP);
						}
					}
					else{
						fd = open(fifo, O_RDWR);
						write(fd, filename, 100);
					}
					numOfWorkers++;
				}
				else{
					fd = open(fifo, O_RDWR);
					write(fd, filename, 100);
					kill(head->data->pid, SIGCONT);
					delete_head(&head);
				}
				//continue;
			}
			if(bytes==0){
				printf("bytes = 0\n");
			}
		}
	}

	return 0;
}