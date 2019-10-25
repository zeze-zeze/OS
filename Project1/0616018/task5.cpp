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
string fist[3] = {"Paper", "Scissor", "Stone"};

int main(){
  int *shmaddr;
  string OK;
  cin >> key;
  int shmid = shmget(key, sizeof(key_t), IPC_CREAT|0666);
  for(int i=0; i<100; i++){
    cin >> OK;
    shmaddr = (int *) shmat(shmid, NULL, 0 );
    cout << fist[(shmaddr + 1) % 3] << endl;
  }
}
