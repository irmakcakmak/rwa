#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>

#define WRITE_KEY (1986)
#define READ_KEY (1987)

using namespace std;

class Reader {
    private:
        int executeLoopCount;
    public:
        Reader(int);
        int up();
        int down();
        int writerUp();
        int writerDown();
        static void log(string message) {
            cout << "Reader: " << getpid() << ": " << message << endl;     
        }
        void read();
        void execute();
        int increment(int);
};

int Reader::up() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = 0;
    int id = semget(READ_KEY, 1, 0666);
    if (semop(id, &sem_b, 1) == 0) {
        log("Read count mutex up success!");
        return(1);    
    }
    return(0);
}

int Reader::writerUp() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = 0;
    int id = semget(WRITE_KEY, 1, 0666);
    if (semop(id, &sem_b, 1) == 0) {
        log("Reader ups writer mutex.");
        return(1);    
    }
    return(0);
   
}

int Reader::down() {
    struct sembuf sem_b;
    sem_b.sem_num = 1;
    sem_b.sem_op = -1;
    sem_b.sem_flg = 0;
    int id = semget(READ_KEY, 1, 0666);
    if (semop(id, &sem_b, 1) == 0) {
        log("Read count mutex down success.");
        return(1);    
    }
    return(0);
}

int Reader::writerDown() {
    struct sembuf sem_b;
    sem_b.sem_num = 1;
    sem_b.sem_op = -1;
    sem_b.sem_flg = 0;
    int id = semget(WRITE_KEY, 1, 0666);
    if (semop(id, &sem_b, 1) == 0) {
        log("Reader downs writer mutex.");
        return(1);    
    }
    return(0);
}


void Reader::execute() {
    log("Started execution");
    int k = 0;
    for (int i = 0; i < this->executeLoopCount; i++) {
        i++;
    }
    log("Finished execution.");
}

Reader::Reader(int loopCount) {
    this->executeLoopCount = loopCount;
    log("Reader is created.");    
}

void Reader::read() {
    key_t key = ftok("hede.txt", 'E');
    log("Accessing to shared memory segment...");
    int shmid = shmget(key, sizeof(int), 0666);
    int *data = (int *)shmat(shmid, (void *)0, 0);
    log("Shared memory region is accessed!");
    std::this_thread::sleep_for(std::chrono::milliseconds(2731));
    log("Value of the shared memory: " + to_string(*data));
    log("Detaching from shared memory segment...");
    if(shmdt(data) == -1) {
        log("Detachment problems.");    
    } else {
        log("Detached!");    
    }

}

int Reader::increment(int t) {
    key_t key = ftok("readcount.txt", 'E');
    int shmid = shmget(key, sizeof(int), 0666);
    int *data = (int *)shmat(shmid, (void *)0, 0);
    log("Incrementing read count in shm:" + to_string(*data));
    *data += t;
    int incremented = *data;
    log("Changed read count to: " + to_string(*data));
    log("Detaching from shared memory segment...");
    if(shmdt(data) == -1) {
        log("Detachment problems.");   
        perror("REASON: "); 
    } else {
        log("Detached!");    
    }
    return incremented;
}

int main(void) {
    cout.setf(std::ios::unitbuf);
    Reader reader(1479);
    reader.down();
    int readCount = reader.increment(1);
    if (readCount == 1) {
        reader.writerDown();
    }
    Reader::log("Read count in critical: " + to_string(readCount));
    reader.up();
    reader.read();
    reader.down();
    readCount = reader.increment(-1);
    if(readCount == 0) {
        reader.writerUp();
    }
    reader.up();
    reader.execute();
    return 0;
}
