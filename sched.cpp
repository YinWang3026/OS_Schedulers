#include <iostream> //cout
#include <fstream> //ifstream
//#include <string>
//#include <vector>
//#include <stdio.h>

using namespace std;

enum process_state_t{ 
    STATE_CREATED,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED
};

enum process_trans_t{
    TRANS_TO_READY, //Run -> Ready, Block -> Ready
    TRANS_TO_RUN, //Ready -> Run
    TRANS_TO_BLOCK, //Running -> Block
    TRANS_TO_PREEMPT //Create -> Ready
};

struct process{
    int arrivalTime;
    int totalCpuTime;
    int cpuBurst;
    int ioBurst;
    process_state_t state;
    int remainCpuTime;
    int staticPriority;
    int stateTimeStamp;
};

struct event{
    process* evtProcess;
    int evtTimeStamp;
    process_trans_t transition;
    event* nextEvt;
};


class Events {
    public:
    private:
        event head;
};

class Processes {
    static int pid;
    public:
    private:
    
};

class Scheduler {
    public:
    private:
};




int main(int argc, char* argv[]) {
    //Opening process file
    ifstream file("../lab2_assign/input0");
    if (!file) {
        cerr << "Could not open the file.\n";
        exit(1);
    }
    int at, tc, cb, io;
    while (file >> at) {
        cout << at << " ";
        cout << endl;
    }
    
    file.close();
}


/*

process_state_t x;
x = STATE_RUNNING;
if (x == STATE_RUNNING) {
    cout << "hello world.\n" << endl;
}

class A {
    public:
        A(int x) : i(x) {}
        void display() const { //Const is a constant method
            return;
        }
    private:
        int i;
};

class B : public A { //Without public keyword, then this is a private inheritance - All public A becomes private in B
    public:
        B(int x, int z) : A(x), b(z) {} //Constructors are not inhertited
    private:
        int b;
};
*/