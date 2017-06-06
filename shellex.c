#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N	100
#define MAXLINE 128
#define MAXARGS 100

void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
int pid[N];
int exit_value[N];
//int pid_n=0;
int mark = 0;
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
	int i;
	int len=0;
	int status;

	strcpy(buf,cmdline);
	bg = parseline(buf, argv);
	if(argv[0] == NULL)
		return;	//ignore empty lines
	
	for(i=0;;i++)
	{
		if(argv[i]!=NULL){
			len++;
		}
		else
			break;
	}	

	if(!builtin_command(argv)){
		for(i=0;i<len;i++){
			if(!strcmp(argv[i],"&"))
                        {
                        	continue;
                        }
			
			if((pid[i]=fork())==0){
				if(execve(argv[i],argv,NULL)<0){
				printf("%s: Command not found.\n", argv[i]);	
				kill(getpid(),SIGINT);	
				}	
			}
			if(!strcmp(argv[i],";"))
			{
				waitpid(pid[i-1],&status,0);
			}
			if(!strcmp(argv[i],"&&"))
                        {
				waitpid(pid[i-1],&status,0);
                              if(status != 0){
					printf("&& error\n:");
					break;
				}
				else
					continue;
                        }
//			if(mark){
//				kill(pid[i],SIGINT);		
//			}
		}
	}
	if(!bg){
                        if(waitpid(pid[i],&status,0)<0)
                                printf("waitfg:waitpid error");
               }

	return;
}

int builtin_command(char **argv)
{
         if(!strcmp(argv[0],"quit"))     //command의 첫 부분이 quit이면 prog 종료
         {
                int i;
                for(i=0;i<10;i++)
                      kill(pid[i],SIGKILL);
                exit(0);
         }
        if(!strcmp(argv[0],"&"))        //command의 첫부분이 &이면 return 1
                return 1;
        return 0;                       //둘다 아니면 return 0
}

int parseline(char *buf, char **argv)//공백을 찾아 공백마다 끊어주기.
{
        int argc=0;
        int bg;

        buf[strlen(buf)-1]='\0';

        argv[argc++] = strtok(buf," ");
        while (argv[argc-1] != NULL)
        {
            argv[argc++] = strtok(NULL, " ");
        }

        argc--;
        argv[argc] = NULL;
        int i;
     //   for(i=0;i<argc;i++)
      //          printf("%s\n",argv[i]);
        if(argc==0)
                return 1;       //argc가 0이라면 즉, 주어진 command에 공백이 없다면 paseline함수는 1을 return한다.

        if((bg=(*argv[argc-1]=='&')) != 0)      //주어진 command의 마지막 부분이 '&' 이면 bg=1이 할당되고 argv의 마지막 부분, 즉 &였던 부분을 NULL로 바꿔준다.
//              argv[--argc] = NULL;
        return bg;
}


