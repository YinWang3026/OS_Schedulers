#include <iostream> //cout
#include <fstream> //ifstream
#include <string>
#include <vector>
#include <stdio.h>
#include <getopt.h> //Arg parsing
#include <stdlib.h>

using namespace std;

//Globals
int vFlag = 0;
int tFlag = 0;
int eFlag = 0;

vector<int> randvals; //Vector containg random integers
int rsize; //Total random integer count

//Macro definitions
#define vtrace(fmt...)  do { if (vFlag) { printf(fmt); fflush(stdout); } } while(0)
#define ttrace(fmt...)  do { if (tFlag) { printf(fmt); fflush(stdout); } } while(0)
#define etrace(fmt...)  do { if (eFlag) { printf(fmt); fflush(stdout); } } while(0)

//Enum definitions
enum process_state_t{ 
    STATE_CREATED,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED
};

string enum_state_string[] = { //Print the state
    "CREAETD",
    "READY",
    "RUNNING",
    "BLOCKED"
};

enum process_trans_t{
    TRANS_TO_READY, //Run -> Ready, Block -> Ready
    TRANS_TO_RUN, //Ready -> Run
    TRANS_TO_BLOCK, //Running -> Block
    TRANS_TO_PREEMPT //Create -> Ready
};

//Struct definitions
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
    int pid; //process id
    int arrivalTime;
    int totalCpuTime;
    int cpuBurst;
    int ioBurst;
    process_state_t state; //current state
    //To be modified
    int remain_cputime;
    int static_priority;
    int dynamic_priority; //static prio - 1
    int state_ts; 
};

struct event{
    static int count;
    event(){ next=NULL; } //Empty constructor
    event(process* p, int ts, process_trans_t t):
        evtProcess(p), evtTimeStamp(ts), transition(t){
        evtid = count++;
        next = NULL;
    }
    void printEvent(){
        printf("evtID: %d, PID: %d, ts: %d, trans: %d\n",evtid,evtProcess->pid,evtTimeStamp,
        transition);
    }
    void eTraceEvent(){
        cout << evtTimeStamp << ":" << evtProcess->pid << ":" << enum_state_string[evtProcess->state] << " ";
        //etrace("%d:%d:%s ",evtTimeStamp,evtProcess->pid,enum_state_string[evtProcess->state].c_str());
    }
    int evtid; //An id
    process* evtProcess; //The proc associated with this event
    int evtTimeStamp; //Time to fire this event
    process_trans_t transition; //Which state to bring the proc to
    event* next; //Pointer to next
};

int process::count = 0; //Init the static count for pid
int event::count = 0; //Init the static count for evtid

//Class definitions
class Events{
    public:
        Events(){
            head = new event();
        }
        ~Events(){
            delete head;
        }
        //Return the first evt in list
        //Dont forget to free the evt
        event* getEvent(){
            if (head->next == NULL){
                return NULL;
            }
            event* temp = head->next;
            head->next = head->next->next;
            return temp;
        }
        //Add to the end of the list based on its timestamp
        void putEvent(event* evt){
            event* tempEvt = head->next;
            event* prevEvt = head;
            while (tempEvt != NULL){
                if (evt->evtTimeStamp < tempEvt->evtTimeStamp){
                    break;
                }
                prevEvt = prevEvt->next;
                tempEvt = tempEvt->next;
            }
            prevEvt->next = evt;
        }
        //Find evt by ID and remove it
        void rmEvent(event* evt){
            event* temp = head->next;
            event* prev = head;
            while (temp != NULL){
                if (temp->evtid == evt->evtid){
                    prev->next = temp->next;
                    delete temp;
                    break;
                }
                temp = temp->next;
                prev = prev->next;
            }
        }
        void printEvents(){
            event* temp = head->next;
            while (temp != NULL) {
                temp->printEvent();
                temp = temp->next;
            }  
        }

    private:
        event* head; //Dummy head of linked list of evts
        //Time stamp increases down the list
};

class Scheduler {
    public:
        Scheduler(){}
        virtual void add_process(process* p){
            
        }
        virtual process* get_next_process(){
            
        }
        virtual void test_preempty(process *p, int curtime){
            
        }
    private:

        //Run queue how i do this
};

class FCFS : public Scheduler{
    public:
    private:
};

class LCFS : public Scheduler{
    public:
    private:
};

class SRTF : public Scheduler{
    public:
    private:
};

class RR : public Scheduler{
    public:
    private:
};

class PRIO : public Scheduler{
    public:
    private:
};

class PREPRIO : public Scheduler{
    public:
    private:
};

//Function prototypes
void printProcList(vector<process*>&);
void deleteProcList(vector<process*>&);
int myrandom(int); //Generates a random number
void simulation(Events&, Scheduler&);


int main(int argc, char* argv[]) {
    int c;
    int maxPrio = 4; //PRIO, PREPRIO, default = 4
    int quantum = 10000; //RR, PRIO, PREPRIO, default = 100000
    char sched; //Scheduler type
    Scheduler myScheduler; //Scheduler for the simulator
    while ((c = getopt(argc,argv,"vtes:")) != -1 )
    {
        switch(c) {
        case 'v': 
            vFlag = 1;
            break;
        case 't': 
            tFlag = 1;
            break;
        case 'e':
            eFlag = 1;
            break;
        case 's':
            sscanf(optarg, "%c%d:%d",&sched,&quantum,&maxPrio);
            break;
            ///ARGUEMNT CHECKING
        }
    }
    vtrace("vflag = %d  tflag = %d eflag = %d\n",vFlag,tFlag,eFlag);
    vtrace("Scheduler: %c, quantum: %d, maxPrio: %d\n",sched,quantum,maxPrio);

    if ((argc - optind) < 2) { //optind is the index of current argument
        cerr << "Missing input file and rfile\n";
        exit(1);
    }
    char* inputFile = argv[optind];
    char* rFile = argv[optind+1];
    vtrace("Input file: %s, rfile: %s\n",inputFile,rFile);
    
    //Opening random value file
    ifstream rfile(rFile);
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
    ifstream ifile(inputFile);
    if (!ifile) {
        cerr << "Could not open the input file.\n";
        exit(1);
    }
    process* p;
    event* evt;
    int at, tc, cb, io;
    vector<process*> procList; //Vector holding all created procs
    Events evtList; //Linked list holding all events

    while (ifile >> at >> tc >> cb >> io) {
        p = new process(at,tc,cb,io,STATE_CREATED);
        evt = new event(p, at, TRANS_TO_READY);
        //MAX_PRIO????????????????????
        procList.push_back(p);
        evtList.putEvent(evt);
    }
    ifile.close(); //Closing file

    printProcList(procList); //Print the process list
    evtList.printEvents(); //Print the event list
    event* temp; //Checking if getEvent works
    while ((temp = evtList.getEvent())){ //Get event test
        temp->printEvent();
        delete temp;
    }
    evtList.rmEvent(evt); //Remove event test
    evtList.printEvents(); //Print the event list
    
    simulation(evtList, myScheduler);
    deleteProcList(procList); //Deleting allocated memory
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
void printProcList(vector<process*>& v){
    for (int i=0; i<v.size(); i++){
        v[i]->printProcess();
    }
}

//Deleting every allocated procs
void deleteProcList(vector<process*>& v){
    for (int i=0; i<v.size(); i++){
        delete v[i];
    }
}

void simulation(Events& evtList, Scheduler& myScheduler){
    event* evt;
    process* proc;
    process_trans_t evtTrans;
    int currentTime, timeInPrevState;
    while((evt = evtList.getEvent())) {
        //Get the details
        proc = evt->evtProcess;
        evtTrans = evt->transition;
        currentTime = evt->evtTimeStamp;
        timeInPrevState = currentTime - proc->state_ts;
        //Delete the evt
        delete evt;
        evt = nullptr;
        /*
        switch(evtTrans) {  // which state to transition to?
            case TRANS_TO_READY:
                // must come from BLOCKED or from PREEMPTION
                // must add to run queue
                CALL_SCHEDULER = true; // conditional on whether something is run
                break;
            case TRANS_TO_RUN:
                // create event for either preemption or blocking
                break;
            case TRANS_TO_BLOCK:
                //create an event for when process becomes READY again
                CALL_SCHEDULER = true;
                break;
            case TRANS_TO_PREEMPT:
                // add to runqueue (no event is generated)
                CALL_SCHEDULER = true;
                break;
        }
        // remove current event object from Memory

        if(CALL_SCHEDULER) {
            if (get_next_event_time() == CURRENT_TIME)
                continue; //process next event from Event queue
            CALL_SCHEDULER = false; // reset global flag
            if (CURRENT_RUNNING_PROCESS == nullptr) {
                CURRENT_RUNNING_PROCESS = THE_SCHEDULER->get_next_process();
                if (CURRENT_RUNNING_PROCESS == nullptr)
                    continue;
            }
        } */
    }
}
// create event to make this process runnable for same time.

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