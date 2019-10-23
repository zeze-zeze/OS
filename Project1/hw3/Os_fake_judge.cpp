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
using namespace std;

int main (int argc , char **argv){
    int shmid = 0;
    int *shm;
    try{
        key_t key_value =  ftok("/tmp/123.txt", 123);
        cout << key_value << endl;
        shmid = shmget(key_value, sizeof(int), IPC_CREAT|0666);
        shm = (int*)shmat(shmid, NULL, 0);
        const string s = "Paper";
        int ti = 100;
        int score = 0;
        while(ti--){
            *shm = 0;
            cout << "OK" <<endl;
            string opponent_option ;
            cin >> opponent_option;
            cout << s <<endl;
            if(opponent_option == "Scissor")
                score++;
        }
        cerr << score << endl;
        shmdt(shm);
        shmctl(shmid,IPC_RMID,0);
    }
    catch(...){
        cerr << 0 <<endl;
        if(shm != 0){
            shmdt(shm);
            shmctl(shmid,IPC_RMID,0);
        }
    }
}