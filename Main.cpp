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

using namespace std;

int main(void) {
    cout.setf(std::ios::unitbuf);
    key_t key = ftok("/Users/irmak/Dropbox/SWE/SWE573/2/p2/code/hede.txt", 'E');
    cout << "Creating shared memory" << endl;
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    cout << "Initializing shared memory variable to 0" << endl;
    int *data = (int *)shmat(shmid, (void *)0, 0);
    *data = 0;
    cout << "Shared memory initialization is complete..." << endl;

    cout << "Initializing write semaphore...";
    union semun {
        int val;
        struct semid_ds *buf;
        ushort *array;    
    } arg;

    arg.val = 1;

    int semid = semget(SEM_KEY, 1, 0666|IPC_CREAT);
    if(semctl(semid, 0, SETVAL, arg) < 0) {
        cout << "Error initializing writer semaphore." << endl;
        perror("REASON: ");
        exit(EXIT_FAILURE);
    } else {
        cout << "Semaphore initialized: " << SEM_KEY << endl;
    }

    srand(time(NULL));
    for(int i = 0; i < 10; i++) {
        int pid = fork();
        int randomNumber = rand() % 2;
        cout << "Random number:" << randomNumber << endl;
        if (pid == 0) {
            if(randomNumber) {
                execl("reader", NULL);
            } else {
                execl("writer", NULL);
            }
            cout << "Child process." << endl;
        } else {
            cout << "Parent process." << getpid() <<endl;
        }
    }
    for (int i = 0; i < 10; i++) {
        wait(NULL);
    }

    if (shmdt(data) == -1) {
        cout << "Detachment problems." << endl;
    } else {
        cout << "Detached" << endl;
    }

    cout << "Releasing shared memory" << endl;
    shmctl(shmid, IPC_RMID, NULL);
    cout << "Deleting semaphore" << endl;
    semctl(semid, 0, IPC_RMID);
    return 0;
}
