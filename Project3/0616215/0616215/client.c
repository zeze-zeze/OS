#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/sem.h>

#include <sys/wait.h>

#define BUFSIZE 4096
#define BUFNUM 16

#define SEM_ADD 1
#define SEM_WAIT -1

#define SRV_ADDR "127.0.0.1"
#define SRV_PORT 7777

// copy from http://man7.org/linux/man-pages/man2/semctl.2.html
union semun {
    int val; /* Value for SETVAL */
    struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

// struct sembuf {
//     unsigned short sem_num; /* semaphore number */
//     short sem_op; /* semaphore operation */
//     short sem_flg; /* operation flags */
// };

struct Buffer {
    int eof;
    uint8_t ctx[BUFSIZE];
    size_t len;
};

/* Semaphore */
int init_sem(key_t key, int init_val) {

    int _sem_id = semget(IPC_PRIVATE, BUFNUM, IPC_CREAT | 06666);
    if (_sem_id == -1) {
        perror("semget error\n");
        exit(-1);
    }
    union semun sem_union;
    sem_union.val = init_val;
    int i;
    for (i = 0; i < BUFNUM; i++) {
        if (semctl(_sem_id, i, SETVAL, sem_union) == -1) { // set init_val
            perror("semctl error\n");
            exit(-1);
        }
    }

    return _sem_id;
}

/* Writer */
void writer(int);

/* Downloader */
int connect_to_server();
void downloader(int);

/* IPC */
void IPC_init();
void IPC_release();

int shm_id;
int sem_id;

//struct sembuf sem_buf_add, sem_buf_wait;
struct sembuf init_sem_buf(int idx, int op) {
    struct sembuf sem_buf;
    sem_buf.sem_num = idx;
    sem_buf.sem_op = op;
    //sem_buf.sem_flg = SEM_UNDO;
    sem_buf.sem_flg = 0;
    return sem_buf;
}
void sem_signal(int id, int idx) {
    struct sembuf sem_buf = init_sem_buf(idx, 1);
    if (semop(id, &sem_buf, 1) == -1) {
        perror("sem_signal error\n");
        exit(-1);
    }
}
int get_sem_val(int id, int idx) {
    return semctl(id, idx, GETVAL, 0);
}

void sem_wait(int id, int idx) {
    struct sembuf sem_buf = init_sem_buf(idx, -1);
    if (semop(id, &sem_buf, 1) == -1) {
        perror("sem_wait error\n");
        printf("%d\n", errno);
        exit(-1);
    }
}

struct Buffer *buf;

int main(int argc, char **argv) {

    /* DO NOT Modify this function */
    int i, pid, status, target_id, ret = 0;

    if (argc < 2) {
        perror("Please specify target id");
        exit(1);
    }

    target_id = atoi(argv[1]);

    printf("IPC_init...\n");
    IPC_init();

    printf("Starting downloader and writer...\n");
    for (i = 0; i < 2; i++) {

        pid = fork();
        if (pid == -1) {
            perror("fork failed\n");
            exit(1);
        } else if (pid == 0) {
            if (!i)
                downloader(target_id);
            else
                writer(target_id);
        }
    }
    /*
     * Make surce your downloader and writer exit normaly
     * */
    for (i = 0; i < 2; i++) {
        wait(&status);
        if (status != 0)
            ret = 1;
    }
    IPC_release();

    return ret;
}

void downloader(int target_id) {

    int ret, sockfd;
    unsigned local_it = 0;

    sockfd = connect_to_server();

    if (write(sockfd, &target_id, sizeof(int)) == -1) {
        perror("socket write error\n");
        exit(1);
    }
    int ii = 0;
    while (1) {

        while (buf[local_it].len != 0) {
            //printf("stuck\n");
            // wait for write to finish writing
        }
        // printf("download %d %d\n", local_it, ii++);
        ret = read(sockfd, buf[local_it].ctx, BUFSIZE);
        if (ret == -1) {
            perror("socket may be broken");
            exit(1);
        } else if (ret == 0) {
            buf[local_it].eof = 1;
            sem_signal(sem_id, local_it);
            exit(0);
        }
        buf[local_it].len = ret;
        sem_signal(sem_id, local_it);
        local_it = (local_it + 1) % BUFNUM;
    }
}

void writer(int target_id) {

    int local_it = 0, fd, ret;
    char path[256];
    sprintf(path, "./output/%d", target_id);

    fd = open(path, O_CREAT | O_RDWR | O_TRUNC, 0755);
    if (fd == -1) {
        perror("Can not create output file");
        exit(1);
    }
    int ii = 0;
    while (1) {

        // printf("ready to write %d %d\n", local_it, ii++);
        // int lock = get_sem_val(sem_id, local_it);
        // if (lock == 0)
        // printf("locked! %d\n", lock);
        sem_wait(sem_id, local_it);
        // lock = get_sem_val(sem_id, local_it);
        // printf("lock %d\n", lock);

        /* Receive EOF from downloader */
        if (buf[local_it].eof) {
            printf("Receive EOF, exiting writer process\n");
            exit(0);
        }
        ret = write(fd, buf[local_it].ctx, buf[local_it].len);
        if (ret != buf[local_it].len) {
            perror("Output file fd write failed");
            exit(1);
        }
        buf[local_it].len = 0;
        local_it = (local_it + 1) % BUFNUM;
    }
}

void IPC_init() {

    shm_id = shmget(IPC_PRIVATE, sizeof(struct Buffer) * BUFNUM, IPC_CREAT | 0600);
    buf = (struct Buffer *)shmat(shm_id, NULL, 0);

    if (buf == (void *)-1) {
        perror("shmat error");
        exit(1);
    }
    int i;
    for (i = 0; i < BUFNUM; i++) {
        buf[i].len = 0;
    }

    memset(buf, 0, sizeof(struct Buffer) * BUFNUM);

    printf("shm_id: %d\n", shm_id);
    key_t key = 999;
    sem_id = init_sem(key, 0);
}

void IPC_release() {

    if (shmdt(buf) == -1) {
        perror("detach SHM Error");
    }

    shmctl(shm_id, IPC_RMID, 0);

    /* TODO(Optinal): Release semaphore */
}

int connect_to_server() {

    /* DO NOT Modify this function */
    struct sockaddr_in srv;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create socket\n");
        exit(1);
    }
    memset(&srv, 0, sizeof(srv));
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr(SRV_ADDR);
    srv.sin_port = htons(SRV_PORT);

    if (connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) == -1) {
        perror("Failed to connet to Server");
        exit(1);
    }

    return sockfd;
}
