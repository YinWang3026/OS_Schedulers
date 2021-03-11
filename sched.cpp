#include <iostream> //cout
#include <fstream> //ifstream
#include <string>
#include <vector>
#include <stdio.h>

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
    static int count;
    process(int at, int ct, int cb, int io, process_state_t s):
        arrivalTime(at), totalCpuTime(ct), cpuBurst(cb), ioBurst(io), state(s){
            pid = count++;
        }
    void printProcess(){
        printf("PID: %d, AT: %d, TC: %d, CB: %d, IO: %d\n",pid,arrivalTime,totalCpuTime,
        cpuBurst,ioBurst);
    }
    int arrivalTime;
    int totalCpuTime;
    int cpuBurst;
    int ioBurst;
    process_state_t state;
    int remainCpuTime;
    int staticPriority;
    int stateTimeStamp;
    int pid;
};

struct event{
    static int count;
    event(process* p, int ts, process_trans_t t):
        evtProcess(p), evtTimeStamp(ts), transition(t){
        evtid = count++;
        nextEvt = NULL;
    }
    void printEvent(){
        printf("evtID: %d, PID: %d, ts: %d, trans: %d\n",evtid,evtProcess->pid,evtTimeStamp,
        transition);
    }
    int evtid;
    process* evtProcess;
    int evtTimeStamp;
    process_trans_t transition;
    event* nextEvt;
};

class events {
    public:
        events ();
        ~events ();
        void get_event(){

        }

        void put_event(){

        }

        void rm_event(){

        }

    private:
        event head; //Dummy head of linked list of evts
};


class Scheduler {
    public:
    private:
};


//Globals
int process::count = 0; //Init the static count for PID
int event::count = 0; //Init the static count for evt id

vector<int> randvals; //Vector containg random integers
int rsize; //Total random integer count
int quantum; //RR

vector<process*> procList; //Vector holding all created procs
events evtList;

//Functions
int myrandom(int);
void printProcList(const vector<process*>&);


int main(int argc, char* argv[]) {
    //Opening random value file
    ifstream rfile("../lab2_assign/rfile");
    if (!rfile) {
        cerr << "Could not open the rfile.\n";
        exit(1);
    }
    int r;
    rfile >> rsize; //Reading the size
    while (rfile >> r) {
        randvals.push_back(r); //Populating the random vector
    }
    rfile.close();
    
    //Opening process file
    ifstream ifile("../lab2_assign/input0");
    if (!ifile) {
        cerr << "Could not open the ifile.\n";
        exit(1);
    }
    process* p;
    event* evt;
    int at, tc, cb, io;
    while (ifile >> at >> tc >> cb >> io) {
        p = new process(at,tc,cb,io,STATE_CREATED);
        procList.push_back(p);
    }
    ifile.close();
    printProcList(procList);
}

//The random function
int myrandom(int burst) {
    static int ofs = -1;
    ofs ++;
    if (ofs >= rsize) {
        ofs = 0;
    }
    return 1 + (randvals[ofs] % burst);
}

//Prints the vector of procs
void printProcList(const vector<process*>& v){
    for (int i=0; i<v.size(); i++){
        v[i]->printProcess();
    }
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