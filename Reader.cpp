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
    private:
        int executeLoopCount;
    public:
        Reader(int);
        static void log(string message) {
            cout << "Reader: " << getpid() << ": " << message << endl;     
        }
        void read();
        void execute();
};

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

int main(void) {
    Reader reader(1479);
    reader.read();
    reader.execute();
    return 0;
}
