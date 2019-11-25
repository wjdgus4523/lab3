#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <errno.h> 
main(int argc, char *argv[]) 
{ 
int fd; /* file descriptor */ 
char *buf = "This is a test."; 
ssize_t cnt; /* write count */ 
int flags = O_WRONLY | O_CREAT | O_TRUNC; 
mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; 

if (argc < 2) { 
fprintf(stderr, "Usage: file_creat filename\n"); 
exit(1); 
} 
if ( (fd = open(argv[1], flags, mode)) == -1 )
{ 
perror("open"); 
exit(1); 
} 
cnt = write(fd, buf, strlen(buf)); 
printf("write count = %d\n", cnt); 
close(fd); 
} 
