#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>

#define SEM_KEY (1986)

using namespace std;

class Writer {
    public:
        int semaphoreId;
        int up();
        int down();
        Writer();
        static void log(string message) {
            cout << "Writer: " << getpid() << ": " << message << endl;     
        }
        void write();
};

int Writer::down() {
    struct sembuf ops[1];
    ops[0].sem_num = 0;
    ops[0].sem_op = -1; // P
    ops[0].sem_flg = 0;
    int id = semget(SEM_KEY, 1, 0666);
    if (id < 0) {
        perror("REASON: ");    
    }
    int retval = semop(id, ops, 1);
    log("Down sem_id: " + to_string(this->semaphoreId));
    if (retval == 0) {
        log("Semaphore operation is successful.");
        return(1);
    }
    perror("REASON");
    log("Retval: " + to_string(retval));
    return(0);
}

int Writer::up() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; // V
    sem_b.sem_flg = 0;
    int id = semget(SEM_KEY, 1, 0666);
    if (semop(id, &sem_b, 1) == 0) {
        log("Semaphore operation is successful.");
        return(1);
    }
    return(0);
}

Writer::Writer() {
}

void Writer::write() {
    key_t key = ftok("/Users/irmak/Dropbox/SWE/SWE573/2/p2/code/hede.txt", 'E');
    int shmid = shmget(key, sizeof(int), 0666);
    log(to_string(shmid));
    int *data = (int *)shmat(shmid, (void *)0, 0);
    log("Shared memory region is accessed!");
    log("Value of the shared memory: " + to_string(*data));
    log("Incrementing count variable...");
    *data += 1;
    log("New value of shared memory: " + to_string(*data));
    log("Detaching from shared memory segment...");
    if(shmdt(data) == -1) {
        log("Detachment problems.");
    } else {
        log("Detached!");
    }
     
}

int main(int argc, char* argv[]) {

    Writer writer;
    if(!writer.up()) {
        Writer::log("Down semaphore failed!");
        exit(EXIT_FAILURE);
    }
    Writer::log("In critical region.");
    writer.write();
    if(!writer.down()) {
        Writer::log("Up semaphore failed!");
        exit(EXIT_FAILURE);
    }
    Writer::log("Critical section ended.");
    return 0;
    
}
