#include <iostream>
#include <unistd.h>

using namespace std;

int main(void) {
    cout << "Hello Reader!" << getpid() << endl;
}
