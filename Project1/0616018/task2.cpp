#include<iostream>
#include<unistd.h>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;
int main(){
  int status;
  int mainp = getpid();
  pid_t pid = 0;
  for(int i=0; i<11; i++){
    if(pid < 0){
      printf("fail");
    }
    else if(pid == 0){
      printf("process pid %d create\n", getpid());
      pid = fork();
    }
    else{
      wait(&status);
      printf("process pid %d exit\n", getpid());
      exit(0);
    }
  }
  wait(&status);
  if(pid)printf("process pid %d exit\n", getpid());
}
