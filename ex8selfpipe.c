/* selfpipe.c */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGSIZE 16

char *msg[2] = { "Hello", "World"};

int main()
{
	char buf[MSGSIZE];
	int p[2], i;

	/* open pipe */
	if (pipe(p) == -1) {
	perror ("pipe call failed");
	exit(1);

}
/* write to pipe */
for (i = 0; i < 2; i++)
	write (p[1], msg[i], MSGSIZE);

/* read from pipe */
for (i = 0; i < 2; i++) {
	read (p[0], buf, MSGSIZE);
	printf ("%s\n", buf);
	}
}

