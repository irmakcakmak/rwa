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

class Reader {
    public:
        Reader();
        static void log(string message) {
            cout << "Reader: " << getpid() << ": " << message << endl;     
        }
        void read();
};

Reader::Reader() {
    log("Reader is created.");    
}

void Reader::read() {
    key_t key = ftok("/Users/irmak/Dropbox/SWE/SWE573/2/p2/code/hede.txt", 'E');
    log("Accessing to shared memory segment...");
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *data = (int *)shmat(shmid, (void *)0, 0);
    log("Shared memory region is accessed!");
    log("Value of the shared memory: " + to_string(*data));
    log("Detaching from shared memory segment...");
    if(shmdt(data) == -1) {
        log("Detachment problems.");    
    } else {
        log("Detached!");    
    }

}

int main(void) {
    Reader reader;
    reader.read();
    return 0;
}
