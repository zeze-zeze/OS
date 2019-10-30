#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
int cn=0;
int fork_tree(){
  pid_t pid;
  int status;
  pid=fork();
  cn++;
  if(pid<0){
    cout<<"Fork Failed!"<<endl;
    return 1;
    }
  else if(pid>0){
    waitpid(pid,&status,0);
    cout<<"process pid "<<getpid()<<" exit"<<endl;
    exit(0);
  }
  else{
    cout<<"process pid "<<getpid()<<" create"<<endl;
    if(cn<10){
      fork_tree();
    }
    else{
      cout<<"process pid "<<getpid()<<" exit"<<endl;
      exit(0);    
    }       
  }
  return 0; 
}

int main(){
  cout<<"process pid "<<getpid()<<" create"<<endl;
  fork_tree();
}
