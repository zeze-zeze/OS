//#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;
#define print_create(pid) printf("process pid %d create\n", pid);
#define print_exit(pid) printf("process pid %d exit\n", pid);

int main() {
    // main process
    int n = 0;
    print_create(getpid());
    while (n < 10) {
        int pid = fork();
        if (pid != 0) {
            waitpid(pid, nullptr, 0);
            break;
        } else {
            print_create(getpid());
            n += 1;
        }
    }
    print_exit(getpid());

    return 0;
}
