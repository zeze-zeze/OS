#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int main() {
    string s = "Scissor";
    int key;
    int *shm, shmid;
    cin >> key;
    string temp;
    try {
        shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
        shm = (int *)shmat(shmid, NULL, 0);

        for (int i = 0; i < 100; i++) {
            cin >> temp;
            switch (*shm) {
            case 0:
                s = "Scissor";
                break;
            case 1:
                s = "Stone";
                break;
            case 2:
                s = "Paper";
                break;
            }

            cout << s << endl;
            cin >> temp;
        }
        shmdt(shm);
        shmctl(shmid, IPC_RMID, 0);
    } catch (...) {
        cerr << 0 << endl;
        if (shm != 0) {
            shmdt(shm);
            shmctl(shmid, IPC_RMID, 0);
        }
    }
    return 0;
}