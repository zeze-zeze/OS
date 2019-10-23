#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
int main(int argc ,char* argv[]){
    int pidx=0,pidy=0,pidz=0;
    printf("create main process %d\n",getpid());
    printf("process %d create process %d\n",getpid(),pidx=fork());
    printf("process %d create process %d\n",getpid(),pidy=fork());
    printf("process %d create process %d\n",getpid(),pidz=fork());
    /*
        your code
    */
    printf("process %d exit its child process %d %d %d\n",getpid(),pidx,pidy,pidz);
}