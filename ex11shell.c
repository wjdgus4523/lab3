#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>



#define CMD_ARGU_SIZE 10
#define CMD_LIST_SIZE 10
#define CMD_GRP_SIZE 10

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif



const char *title = "shell>> ";

char* c_grp[CMD_GRP_SIZE];
char* c_list[CMD_LIST_SIZE];
char* c_args[CMD_ARGU_SIZE];
char c_line[BUFSIZ];

void fatal_err(char *str);
void redirect(char* cmd);
void act_cmd(char *c_list);
void act_c_line(char* c_line);
void act_c_grp(char* c_grp);
void zombie(int signum);
int M_argv(char *s, const char *deli, char** argvp, int MAX_LIST);


struct sigaction act;
static int status;
static int CMD_BACKGROUND=0;

typedef struct { //command
    char* name;
    char* desc;
    int ( *function )( int argc, char* argv[] ); //point
} COMMAND;

int my_cd( int argc, char* argv[] ){ //cd
    
    if( argc == 1 )
        chdir( getenv( "HOME" ) );
    else if( argc == 2 ){
        if( chdir( argv[1] ) )
            printf( "No directory\n" );
    }
    else
        printf( "cd [dir]\n" );
    
    return TRUE;
}

int my_exit( int argc, char* argv[] ){//exit
   exit(0);
   return TRUE;
}


static COMMAND builtin_cmds[] =
{
    { "cd", "I/O directory", my_cd },
    { "exit", "exit~", my_exit },
    
 };


void zombie(int signum)
{
    pid_t pid ;
    int stat ;
    
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d \n", pid) ;
}

void fatal_err(char *str)
{
	perror(str);
	exit(1);
}

int M_argv(char *s, const char *deli, char** argvp, int MAX_LIST)
{
	int i = 0;
	int token = 0;
	char *snew = NULL;
    
	if( (s==NULL) || (deli==NULL) )
	{
		return -1;
	}
    
	snew = s+strspn(s, deli);
	
	argvp[token]=strtok(snew, deli);
	
	if( argvp[token] !=NULL)
		for(token=1; (argvp[token]=strtok(NULL, deli)) != NULL; token++)
		{
			if(token == (MAX_LIST-1)) return -1;
		}
    
	if( token > MAX_LIST) return -1;
    
	return token;
}

void redirect(char* cmd)
{
	char *arg;
	int cmdlen = strlen(cmd);
	int fd, i;
	
	for(i = cmdlen-1;i >= 0;i--)
	{
		switch(cmd[i])
		{
			case '<':
				arg = strtok(&cmd[i+1], " \t");
				if( (fd = open(arg, O_RDONLY | O_CREAT, 0644)) < 0)
					fatal_err("file error");
				dup2(fd, STDIN_FILENO);
				close(fd);
				cmd[i] = '\0';
				break;
			case '>':
				arg = strtok(&cmd[i+1], " \t");
                		if( (fd = open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
					fatal_err("file error");
                		dup2(fd, STDOUT_FILENO);
                		close(fd);
                		cmd[i] = '\0';
				break;

                         default:
				break;
		}
	}
    
}

int background(char *cmd)
{
	int i;
    
    for(i=0; i < strlen(cmd); i++)
        if(cmd[i] == '&')
        {
            cmd[i] = ' ';
            return 1;
        }
			
    
	return 0;
}

void act_cmd(char *c_list)
{
    redirect(c_list);
    
    if(M_argv(c_list, " \t", c_args, CMD_ARGU_SIZE) <= 0)
		fatal_err("argv error");
	
    execvp(c_args[0], c_args);
    fatal_err("exe error");
}

void act_c_grp(char *c_grp)
{
	int i=0;
	int count = 0;
	int pfd[2];
    sigset_t set;
    
	setpgid(0,0);
 	if(!CMD_BACKGROUND)
        tcsetpgrp(STDIN_FILENO, getpid());
    
    sigfillset(&set);
    sigprocmask(SIG_UNBLOCK,&set,NULL);
    
    if((count = M_argv(c_grp, "|", c_list, CMD_LIST_SIZE)) <= 0)
        fatal_err("GRP error");
    
	for(i=0; i<count-1; i++)
    {
		pipe(pfd);
		switch(fork())
		{
			case -1: fatal_err("fork error");
            case  0: close(pfd[0]);
                dup2(pfd[1], STDOUT_FILENO);
                act_cmd(c_list[i]);
            default: close(pfd[1]);
                dup2(pfd[0], STDIN_FILENO);
		}
	}
	act_cmd(c_list[i]);
    
}

void act_c_line(char* c_line)
{
    int count = 0;
    int i=0, j=0, pid;
    char* cmdvector[CMD_ARGU_SIZE];
    char c_grptemp[BUFSIZ];
    int token = 0;
    
    count = M_argv(c_line, ";", c_grp, CMD_GRP_SIZE);
    
    for(i=0; i<count; ++i)
    {
        memcpy(c_grptemp, c_grp[i], strlen(c_grp[i]) + 1);
        token = M_argv(c_grp[i], " \t", cmdvector, CMD_GRP_SIZE);
        
        for( j = 0; j < sizeof( builtin_cmds ) / sizeof( COMMAND ); j++ ){            
		if( strcmp( builtin_cmds[j].name, cmdvector[0] ) == 0 ){
	                builtin_cmds[j].function( token , cmdvector );
                return;
            }
        }
        
		CMD_BACKGROUND = background(c_grptemp);
        
        switch(pid=fork())
        {
            case -1:
                fatal_err("err");
            case  0:
                act_c_grp(c_grptemp);
            default:
                if(CMD_BACKGROUND) 
			break;
                waitpid(pid, NULL, 0);
                tcsetpgrp(STDIN_FILENO, getpgid(0));
                fflush(stdout);
        }
    }
    
}

int main(int argc, char**argv)
{
	int i;
	sigset_t set;
	
	sigfillset(&set);
	sigdelset(&set,SIGCHLD);
	sigprocmask(SIG_SETMASK,&set,NULL);
    
	act.sa_flags = SA_RESTART;
	sigemptyset(&act.sa_mask);
	act.sa_handler = zombie;
	sigaction(SIGCHLD, &act, 0);
	
    while (1) {
        fputs(title, stdout);
        fgets(c_line, BUFSIZ, stdin);
        c_line[ strlen(c_line) -1] ='\0';
		act_c_line(c_line);
	}
	return 0;
}

