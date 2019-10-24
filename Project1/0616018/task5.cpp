#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <typeinfo>
#include <string.h>
using namespace std;

int main(){
  int shmid;
  char *shmaddr;
  //cin >> key;
  key_t key =  ftok("/tmp/1234.txt", 123);
  cout<<key;
  shmid = shmget(key, sizeof(key_t), IPC_CREAT|0666);
  shmaddr = (char *)shmat(shmid, NULL, 0);
  //strcpy( shmaddr, "Hi, I am child process!\n") ;
  shmdt( shmaddr ) ;
  shmaddr = (char *) shmat(shmid, NULL, 0 ) ;
  printf("%s", shmaddr);
}
