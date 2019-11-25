#include<stdio.h>
#include<fcntl.h>
#include<termios.h>
#include<time.h>
#include<sys/types.h>
#include<unistd.h>

void typing(int time1, int time2){
  
  int time_count;
  float avg_typing;

  time_count = time2 - time1;
  printf("sec: %dsec\n",time_count);
  avg_typing = (74/time_count)*60;
  printf("avg: %.lf\n",avg_typing);
}
int main(void)
{
int fd,fd1,fd2;
int nread, count=0, errcount=0;
int nread1, count1=0, errcount1=0;
int nread2, count2=0, errcount2=0;
char ch, text[] = "HiImFineThankyou";
char ch1, text1[] = "SystemProgramming";
char ch2, text2[] = "LinuxUbuntuLab";

struct termios init_attr, new_attr;
time_t time1, time2;

fd = open(ttyname(fileno(stdin)), O_RDWR);
fd1 = open(ttyname(fileno(stdin)), O_RDWR);
fd2 = open(ttyname(fileno(stdin)), O_RDWR);

tcgetattr(fd, &init_attr);
new_attr = init_attr;
new_attr.c_lflag &= ~ICANON;
new_attr.c_lflag &= ~ECHO;
new_attr.c_cc[VMIN] = 1;
new_attr.c_cc[VTIME] = 0;

if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
  fprintf(stderr, "don't set\n");
}
if (tcsetattr(fd1, TCSANOW, &new_attr) != 0) {
  fprintf(stderr, "don't set\n");
}
if (tcsetattr(fd2, TCSANOW, &new_attr) != 0) {
  fprintf(stderr, "don't set\n");
}

fd1 = open(ttyname(fileno(stdin)), O_RDWR);
tcgetattr(fd, &init_attr);
new_attr = init_attr;
new_attr.c_lflag &= ~ICANON;
new_attr.c_lflag &= ~ECHO;
new_attr.c_cc[VMIN] = 1;
new_attr.c_cc[VTIME] = 0;
fd2 = open(ttyname(fileno(stdin)), O_RDWR);
tcgetattr(fd, &init_attr);
new_attr = init_attr;
new_attr.c_lflag &= ~ICANON;
new_attr.c_lflag &= ~ECHO;
new_attr.c_cc[VMIN] = 1;
new_attr.c_cc[VTIME] = 0;
  time(&time1);
  
printf("DO Typing\n\n%s\n", text);
while ((nread=read(fd, &ch, 1)) > 0 && ch != '\n') {
  if (ch == text[count++])
  write(fd, &ch, 1);
  else {
    write(fd, "*", 1);
    errcount++;
  }
}

printf("\n\nDO Typing\n\n%s\n", text1);
while ((nread=read(fd1, &ch1, 1)) > 0 && ch1 != '\n') {
  if (ch1 == text1[count1++])
  write(fd1, &ch1, 1);
  else {
    write(fd1, "*", 1);
    errcount++;
  }
}

printf("\n\nDO Typing\n\n%s\n", text2);
while ((nread=read(fd2, &ch2, 1)) > 0 && ch2 != '\n') {
  if (ch2 == text2[count2++])
  write(fd2, &ch2, 1);
  else {
    write(fd2, "*", 1);
    errcount++;
  }
}

printf("\nerr %d\n", errcount);
time(&time2);
typing(time1,time2);

tcsetattr(fd, TCSANOW, &init_attr);
tcsetattr(fd1, TCSANOW, &init_attr);
tcsetattr(fd2, TCSANOW, &init_attr);
close(fd);
close(fd1);
close(fd2);

}
