#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    int pidx = 0, pidy = 0, pidz = 0;
    printf("create main process %d\n", getpid());
    pidx = fork();
    printf("process %d create process %d\n", getpid(), pidx);

    pidy = fork();
    if (pidy == 0)
        pidx = 0;
    printf("process %d create process %d\n", getpid(), pidy);

    pidz = fork();
    if (pidz == 0)
        pidx = pidy = 0;

    printf("process %d create process %d\n", getpid(), pidz);

    /*
    // Using this method SHOULDN't work, however it works.... weird

    waitpid(pidx, nullptr, 0);
    waitpid(pidy, nullptr, 0);
    waitpid(pidz, nullptr, 0);
    */

    int wpid = 1, status;
    while ((wpid = wait(&status)) > 0) {
    }

    printf("process %d exit its child process %d %d %d\n", getpid(), pidx, pidy, pidz);
}