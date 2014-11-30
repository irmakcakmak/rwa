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
    key_t key = ftok("/Users/irmak/Dropbox/SWE/SWE573/2/p2/code/hede.txt", 'E');
    cout << key << endl;
    cout << "Creating shared memory" << endl;
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    cout << shmid << endl;
    cout << "Initializing shared memory variable to 0" << endl;
    int *data = (int *)shmat(shmid, (void *)0, 0);
    *data = 0;
    cout << "Initialization is complete..." << endl;
    cout << "Detaching" << endl;

    union semun {
        int val;
        struct semid_ds *buf;
        ushort *array;    
    } arg;

    arg.val = 0;

    int semid = semget(SEM_KEY, 1, 0666|IPC_CREAT);
    cout << "semid: " << semid << endl;
    if(semctl(semid, 0, SETVAL, arg) < 0) {
        cout << "Cannot set semaphore value." << endl;
    } else {
        cout << "Semaphore initialized: " << SEM_KEY << endl;
    }

    for(int i = 0; i < 10; i++) {
        int pid = fork();
        int status = 0;
        if (pid == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1231));
            srand(time(0));
            int randomNumber = rand() % 2;
            cout << randomNumber << endl;
            if(randomNumber) {
                execl("reader", NULL);
            } else {
                execl("writer", NULL);
            }
            cout << "Child process." << endl;
        } else {
            wait(&status);
            cout << "Parent process." << getpid() <<endl;
        }
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
