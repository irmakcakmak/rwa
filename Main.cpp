#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>

#define SEM_KEY (1986)
#define READ_KEY (1987)

using namespace std;

int main(void) {
    cout.setf(std::ios::unitbuf);
    /* Create shared memory variable for count  */
    key_t key = ftok("hede.txt", 'E');
    cout << "Creating shared memory" << endl;
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    cout << "Initializing shared memory variable to 0" << endl;
    int *data = (int *)shmat(shmid, (void *)0, 0);
    *data = 0;
    cout << "Shared memory initialization is complete..." << endl;

    /* Create shared memory variable for reader count */
    key_t readKey = ftok("readcount.txt", 'E');
    cout << "Creating shared memory for read count." << endl;
    int readShmid = shmget(readKey, sizeof(int), 0666|IPC_CREAT);
    cout << "Initializing shared memory variable to 0" << endl;
    int *readCount = (int *)shmat(readShmid, (void *)0, 0);
    *readCount = 0;
    cout << "Shared memory for read count initialization is complete..." << endl;

    /* Initialize semaphores  */
    union semun {
        int val;
        struct semid_ds *buf;
        ushort *array;    
    } arg;

    arg.val = 1;

    /* Initialize writer semaphore  */
    int semid = semget(SEM_KEY, 1, 0666|IPC_CREAT);
    if(semctl(semid, 0, SETVAL, arg) < 0) {
        cout << "Error initializing writer semaphore." << endl;
        perror("REASON: ");
        exit(EXIT_FAILURE);
    } else {
        cout << "Writer semaphore initialized: " << SEM_KEY << endl;
    }

    /* Initialize reader semaphore  */
    int readid = semget(READ_KEY, 1, 0666|IPC_CREAT);
    if(semctl(readid, 0, SETVAL, arg) < 0) {
        cout << "Error init read sema." << endl;
        perror("REASON: ");
        exit(EXIT_FAILURE);    
    } else {
        cout << "Read semaphore initialized: " << READ_KEY << endl;    
    }

    /* Fork readers */
    for(int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {    
            std::this_thread::sleep_for(std::chrono::milliseconds(1300));
            execl("reader", NULL);
        } else {
            continue;
        }

    }

    /* Fork writers */
    for(int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {    
            std::this_thread::sleep_for(std::chrono::milliseconds(995));
            execl("writer", NULL);
        } else {
            continue;
        }

    }

    /* Wait for children to die */
    for (int i = 0; i < 10; i++) {
        wait(NULL);
    }

    /* Clean up shared memory and semaphores  */
    if (shmdt(data) == -1) {
        cout << "Detachment problems." << endl;
    } else {
        cout << "Shared memory detached." << endl;
    }
    if (shmdt(readCount) == -1) {
        cout << "Detachment problems." << endl;
    } else {
        cout << "Shared memory detached." << endl;
    }

    cout << "Releasing shared memory..." << endl;
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(readShmid, IPC_RMID, NULL);
    cout << "Shared memory is released." << endl;
    cout << "Deleting semaphores..." << endl;
    semctl(semid, 0, IPC_RMID);
    semctl(readid, 0, IPC_RMID);
    cout << "Semaphores releases." << endl;
    return 0;
}
