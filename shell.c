#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 10
#define MAXARGS 5

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

int main()
{
	char cmdline[MAXLINE]; //command line

	while(1){
		/*Read*/
		printf("> ");
		fgets(cmdline,MAXLINE,stdin);
		if(feof(stdin))
			exit(0);

		/*Evaluate*/
		eval(cmdline);
	}
}

void eval(char *cmdline)
{
	char *argv[MAXARGS];  // argument list execve()
	char buf[MAXLINE];	//holds modified command line
	int bg;
	int pid;	//process id

	strcpy(buf,cmdline);
	bg = parseline(buf, argv);
	if(argv[0] == NULL)
		return;	//ignore empty lines
	
	if(!builtin_command(argv)){
		if((pid=fork())==0){
			if(execve(argv[0],argv,NULL)<0){
				printf("%s: Command not found.\n", argv[0]);
				exit(0);
			}
		}
		
		if(!bg){
			int status;
			if(waitpid(pid,&status,0)<0)
				printf("waitfg:waitpid error");
		}
		else
			printf("%d %s", pid, cmdline);
	}
	return;
}

int builtin_command(char **argv)
{
	if(!strcmp(argv[0],"quit"))
		exit(0);
	if(!strcmp(argv[0],"&"))
		return 1;
	return 0;
}

int parseline(char *buf, char **argv)//공백을 찾아 공백마다 끊어주기.
{
	char *delim;
	int argc=0;
	int bg;

	buf[strlen(buf)-1] = ' ';
	while(*buf && (*buf == ' '))
		buf++;

	while((delim = strchr(buf, ' '))){
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim+1;
		while(*buf && (*buf==' '))
			buf++;
	}
	argv[argc] = NULL;

	if(argc==0)
		return 1;

	if((bg=(*argv[argc-1]=='&')) != 0)
		argv[--argc] = NULL;
	return bg;
}
