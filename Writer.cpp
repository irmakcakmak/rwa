#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>

using namespace std;

class Writer {
    public:
        Writer();
        void log(string message);
        void write();
};

Writer::Writer() {
    cout << "Writer is created." << endl;    
}

void Writer::log(string message) {
    cout << "Writer:" << getpid() << ": " << message << endl;     
}

void Writer::write() {
    key_t key = ftok("/Users/irmak/Dropbox/SWE/SWE573/2/p2/code/hede.txt", 'E');
    this->log("Accessing to shared memory segment...");
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *data = (int *)shmat(shmid, (void *)0, 0);
    this->log("Shared memory region is accessed!");
    this->log("Value of the shared memory: " + to_string(*data));
    this->log("Incrementing count variable...");
    *data += 1;
    this->log("New value of shared memory: " + to_string(*data));
    this->log("Detaching from shared memory segment...");
    if(shmdt(data) == -1) {
        this->log("Detachment problems.");
    } else {
        this->log("Detached.");
    }
     
}

int main(void) {
    Writer writer;
    writer.write();
    return 0;
    
}
