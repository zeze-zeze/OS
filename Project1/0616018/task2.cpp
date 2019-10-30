#include<iostream>
#include<unistd.h>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;
int main(){
  int status;
  pid_t pid;
  pid = fork();
  for(int i=0; i<11; i++){
    if(pid < 0){
      printf("fail");
    }
    else if(pid == 0){
      printf("process pid %d create\n", getpid());
      pid = fork();
    }
    else{
      int ex = wait(&status);
      if(ex != -1) printf("process pid %d exit\n", ex);
    }
  }
}
