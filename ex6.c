
#include <stdio.h>

int my_system(char *command)
{
	int status;
	
	switch(fork()){
	
	case 0:
	execl("/bin/bash", "bin/bash", "-c", command, NULL);
	break;

	case -1:
	perror("fork");
	exit(0);

	default:
	wait(&status);
	break;
}

return status;
}

int main(){

	my_system("ls -R");
	exit(0);

}
