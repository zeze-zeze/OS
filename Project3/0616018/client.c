#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/sem.h>

#include <sys/wait.h>

#define BUFSIZE 4096
#define BUFNUM  16

#define SRV_ADDR "127.0.0.1"
#define SRV_PORT 7777

/* TODO: Useful structure */
union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
}; 

struct Buffer{
    int eof;
    uint8_t ctx[BUFSIZE];
    size_t len;
    int done;
};

/* Declare functions */
int init_semaphore(int sem_val);
int semaphore_p();
int semaphore_v();

/* Writer */
void writer(int);

/* Downloader */
int connect_to_server();
void downloader(int);

/* IPC */
void IPC_init();
void IPC_release();

/* TODO: Declare some global variable here */
int sem_id, KEY = 487;

int shm_id;
struct Buffer *buf;

int main(int argc, char** argv){
    
    /* DO NOT Modify this function */ 
    int i, pid, status, target_id, ret = 0;
    
    if (argc < 2){
        perror("Please specify target id");
        exit(1);
    }
    
    target_id = atoi(argv[1]);

    printf("IPC_init...\n");
    IPC_init();
    
    printf("Starting downloader and writer...\n");
    for(i=0; i<2; i++){
        
        pid = fork();
        if(pid == -1){
            perror("fork failed\n");
            exit(1);
        }
        else if(pid == 0){
            if(!i)
                downloader(target_id);
            else
                writer(target_id);
        }
    }
    /*
     * Make surce your downloader and writer exit normaly
     * */
    for(i=0;i<2;i++){
        wait(&status);
        if(status != 0) ret = 1;
    }
    IPC_release();

    return ret;
}

void downloader(int target_id){
    

    int ret, sockfd;
    unsigned local_it = 0;

    sockfd = connect_to_server();

    if (write(sockfd, &target_id, sizeof(int)) == -1){
        perror("socket write error\n");
        exit(1);
    }
    init_semaphore(1);
    while ( 1 )
    {
        /* TODO: sync downloader and writer */
        while(buf[local_it].done == 0);
        semaphore_p();

        ret = read(sockfd, buf[local_it].ctx, BUFSIZE);
        if (ret == -1){
            perror("socket may be broken");
            exit(1);
        }
        else if (ret == 0){
            buf[local_it].eof = 1;
            /* TODO: signal writer */
            semaphore_v();
            buf[local_it].done = 0;
            exit(0);
        }
        buf[local_it].len = ret;
        buf[local_it].done = 0;
        local_it = (local_it+1) % BUFNUM; 
        
        /* TODO: sync downloader and writer */
        semaphore_v();
    }
}

void writer(int target_id){
    
    int local_it = 0, fd, ret;
    char path[256];
    sprintf(path, "./output/%d", target_id);

    fd = open(path, O_CREAT|O_RDWR|O_TRUNC, 0755);
    if(fd == -1){
        perror("Can not create output file");
        exit(1);
    }
    init_semaphore(0);
    while(1){

        /* TODO: sync downloader and writer */
        while(buf[local_it].done == 1);
        semaphore_p();

        /* Receive EOF from downloader */
        if (buf[local_it].eof){
            printf("Receive EOF, exiting writer process\n");
            exit(0);
        }
        ret = write(fd, buf[local_it].ctx, buf[local_it].len);
        if(ret != buf[local_it].len){
            perror("Output file fd write failed");
            exit(1);
        }
        buf[local_it].done = 1;
        local_it = (local_it+1) % BUFNUM;

        /* TODO: sync downloader and writer */
        semaphore_v();        
    }
}

void IPC_init(){
    shm_id = shmget(IPC_PRIVATE, sizeof(struct Buffer)*BUFNUM, IPC_CREAT|0600);
    buf = (struct Buffer*)shmat(shm_id, NULL, 0);
    
    if(buf == (void*)-1){
        perror("shmat error");
        exit(1);
    }

    memset(buf, 0, sizeof(struct Buffer)*BUFNUM);
    
    printf("shm_id: %d\n", shm_id);
    
    /* TODO: Create semaphore */
    int i;
    for(i=0; i<BUFNUM; i++) buf[i].done = 1;
    sem_id = semget(KEY, 1, 0666 | IPC_CREAT);
}

void IPC_release(){
    if(shmdt(buf) == -1){
        perror("detach SHM Error");
    }

    shmctl(shm_id, IPC_RMID, 0);
    
    /* TODO(Optinal): Release semaphore */
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
        perror("sem release error");
        exit(1);
    }
}

int connect_to_server(){
    /* DO NOT Modify this function */ 
    struct sockaddr_in srv;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Failed to create socket\n");
        exit(1);
    }
    memset(&srv, 0, sizeof(srv));

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr(SRV_ADDR);
    srv.sin_port = htons(SRV_PORT);

    if (connect(sockfd, (struct sockaddr*)&srv, sizeof(srv)) == -1){
        perror("Failed to connet to Server");
        exit(1);
    }
    
    return sockfd;
}
int init_semaphore(int sem_val){
    union semun sem_union;
    if (sem_val >= 0) {
        sem_union.val = sem_val;
        if (semctl(sem_id, 0, SETVAL, sem_union) == -1) {
            return -1;
        }
    } 
}
int semaphore_p(){
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return -1;
    }
    return 0;
}
int semaphore_v(){
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return -1;
    }
    return 0;
}
