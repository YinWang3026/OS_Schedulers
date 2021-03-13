#include <iostream> //cout
#include <fstream> //ifstream
#include <string>
#include <vector>
#include <stdio.h>
#include <getopt.h> //Arg parsing
#include <stdlib.h>

using namespace std;

//Flags
int vFlag = 0;
int tFlag = 0;
int eFlag = 0;

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

string state_string[] = { //Print the state
    "CREATED",
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
    process(int at, int ct, int cb, int io, int p, process_state_t s):
        arrivalTime(at), totalCpuTime(ct), cpuBurst(cb), ioBurst(io),  
        state(s), remain_cputime(ct), static_priority(p), state_ts(at){
            pid = count++;
            dynamic_priority = static_priority - 1;
        }
    void printProcess(){
        printf("PID: %d, AT: %d, TC: %d, CB: %d, IO: %d, State: %s, rem: %d, sPri: %d, dPri: %d, ts: %d\n",
        pid,arrivalTime,totalCpuTime,cpuBurst,ioBurst,state_string[state].c_str(),remain_cputime,
        static_priority,dynamic_priority,state_ts);
    }
    int pid; //process id
    int arrivalTime;
    int totalCpuTime;
    int cpuBurst;
    int ioBurst;
    process_state_t state; //current state
    int remain_cputime; //total cpu subtract what is done
    int static_priority; //myRandom
    int dynamic_priority; //static prio - 1
    int state_ts; //time that proc put into this state
};

struct event{
    static int count;
    event(){ next=NULL; } //Constructor for dummy head
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
        cout << evtTimeStamp << ":" << evtProcess->pid << ":" << state_string[evtProcess->state] << " ";
        //etrace("%d:%d:%s ",evtTimeStamp,evtProcess->pid,state_string[evtProcess->state].c_str());
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
            //Should be empty by the end of simulation, so just get rid of head
            delete head;
        }
        //Return/pops the first evt based on timestamp, and dont forget to delete the evt
        event* getEvent(){
            if (head->next == NULL){
                return NULL;
            }
            event* temp = head->next;
            head->next = head->next->next;
            return temp;
        }
        //Add to the list based on its timestamp
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

//Linked list for processes
struct procNode {
    procNode(process* p, procNode* n = NULL) : proc(p), next(n) {}
    process* proc;
    procNode* next;
};

class Scheduler {
    public:
        Scheduler(int q, int p) : quantum(q), maxprio(p){}
        virtual ~Scheduler(){}

        //Each scheduler implements their own virtual funcs
        virtual void add_process(process*){
            return;
        };
        virtual process* get_next_process(){
            return NULL;
        };
        virtual void test_preempty(process*, int){
            return;
        };

        //Returns name of the scheduler
        virtual string getSchedName(){
            return "Scheduler";
        }
        //Returns the quantum
        int getQuantum(){
            return quantum;
        }
        //Returns the maxprio
        int getMaxprio(){
            return maxprio;
        }
    protected:
        int quantum;
        int maxprio;
};

//Constructor are not inherited
class FCFS : public Scheduler{
    public:
        FCFS(int q, int p) : Scheduler(q, p) {
            head = new procNode(NULL);
        }
        ~FCFS(){
            delete head;
        }
        void add_process(process* p){
            return;
        }
        process* get_next_process(){
            return NULL;
        }
        string getSchedName(){
            return "FCFS";
        }
    private:
        procNode* head;
};
/*
class LCFS : public Scheduler{
    public:
        LCFS(int q, int p) : Scheduler(q, p) {}
        void add_process(process* p){

        }
        process* get_next_process(){

        }
        void test_preempty(process *p, int curtime){
            cerr << "LCFS Does not implement test_preempty.\n";
            exit(1);
        }
    private:
};

class SRTF : public Scheduler{
    public:
        SRTF(int q, int p) : Scheduler(q, p) {}
        void add_process(process* p){

        }
        process* get_next_process(){

        }
        void test_preempty(process *p, int curtime){
            cerr << "SRTF Does not implement test_preempty.\n";
            exit(1);
        }
    private:
};

class RR : public Scheduler{
    public:
        RR(int q, int p) : Scheduler(q, p) {}
        void add_process(process* p){

        }
        process* get_next_process(){

        }
        void test_preempty(process *p, int curtime){
            cerr << "RR Does not implement test_preempty.\n";
            exit(1);
        }
    private:
};

class PRIO : public Scheduler{
    public:
        PRIO(int q, int p) : Scheduler(q, p) {}
        void add_process(process* p){

        }
        process* get_next_process(){

        }
        void test_preempty(process *p, int curtime){
            cerr << "FCFS Does not implement test_preempty.\n";
            exit(1);
        }
    private:
};

class PREPRIO : public Scheduler{
    public:
        PREPRIO(int q, int p) : Scheduler(q, p) {}
        void add_process(process* p){

        }
        process* get_next_process(){
            
        }
        void test_preempty(process *p, int curtime){
        }
    private:
};
*/

//Globals
vector<int> randvals; //Vector containg the random integers

//Function prototypes
void printProcList(vector<process*>&);
void deleteProcList(vector<process*>&);
int myrandom(int); //Generates a random number
void simulation(Events*, Scheduler*);

int main(int argc, char* argv[]) {
    int c;
    Scheduler* myScheduler; //Scheduler for the simulator
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
            char sched; //Scheduler type
            int maxprio; //PRIO, PREPRIO, default = 4
            int quantum; //RR, PRIO, PREPRIO, default = 100000
            sscanf(optarg, "%c%d:%d",&sched,&quantum,&maxprio);
            switch(sched) {
                case 'F': //FCFS
                    myScheduler = new FCFS(10000, 4);
                    break;
                /*case 'L': //LCFS
                    myScheduler = new LCFS(100000, 4);
                    break;
                case 'S': //SRTF
                    myScheduler = new SRTF(10000, 4);
                    break;
                case 'R': //Round Robin
                    if (quantum <= 0) {
                        cerr << "Error: RR - Quantum <= 0.\n";
                        exit(1)
                    }
                    myScheduler = new RR(quantum, 4);
                    break;
                case 'P': //Priority
                    if (quantum <= 0) {
                        cerr << "Error: Priority - Quantum <= 0.\n";
                        exit(1)
                    }
                    if (maxprio <= 0) {
                        cerr << "Error: Priority - Max prio <= 0.\n";
                        exit(1)
                    }
                    myScheduler = new PRIO(quantum,maxprio);
                    break;
                case 'E': //Preempt Priority
                    if (quantum <= 0) {
                        cerr << "Error: Preempt Priority - Quantum <= 0.\n";
                        exit(1)
                    }
                    if (maxprio <= 0) {
                        cerr << "Error: Preempt Priority - Max prio <= 0.\n";
                        exit(1)
                    }
                    myScheduler = new PREPRIO(quantum,maxprio);
                    break;*/
                default:
                    cerr << "Error: Invalid Scheduler Name.\n";
                    exit(1);                   
            }
            break;
        }
    }
    //Debug statements, invoked using -v flag
    vtrace("vflag = %d  tflag = %d eflag = %d\n",vFlag,tFlag,eFlag);
    vtrace("Scheduler: %s, quantum: %d, maxprio: %d\n",myScheduler->getSchedName().c_str(),myScheduler->getQuantum(),myScheduler->getMaxprio());

    if ((argc - optind) < 2) { //optind is the index of current argument
        cerr << "Missing input file and rfile\n";
        exit(1);
    }

    //Gettng file names
    char* inputFile = argv[optind];
    char* randomFile = argv[optind+1];
    vtrace("Input file: %s, rfile: %s\n",inputFile,randomFile);
    
    //Opening random value file
    ifstream rfile(randomFile);
    if (!rfile) {
        cerr << "Could not open the rfile.\n";
        exit(1);
    }
    int r, rsize; //Random int, and total random integer count
    rfile >> rsize; //Reading the size
    while (rfile >> r) {
        randvals.push_back(r); //Populating the random vector
    }
    rfile.close();

    //Opening input file
    ifstream ifile(inputFile);
    if (!ifile) {
        cerr << "Could not open the input file.\n";
        exit(1);
    }
    process* p;
    event* evt;
    int at, tc, cb, io, prio;
    vector<process*> procList; //Vector holding all created procs
    Events* evtList = new Events(); //Linked list holding all events

    while (ifile >> at >> tc >> cb >> io) {
        prio = myrandom(myScheduler->getMaxprio());
        p = new process(at,tc,cb,io, prio, STATE_CREATED);
        evt = new event(p, at, TRANS_TO_READY);
        procList.push_back(p);
        evtList->putEvent(evt);
    }
    ifile.close(); //Closing file

    printProcList(procList); //Print the process list
    evtList->printEvents(); //Print the event list

    //Begin simulation
    simulation(evtList, myScheduler);
    //Clean up
    deleteProcList(procList); //Deleting allocated procs
    delete myScheduler; //Deleting scheduler
    delete evtList; //Deleting evts
}

void simulation(Events* evtList, Scheduler* myScheduler){
    event* evt;
    process* proc;
    process_trans_t evtTransTo;
    int currentTime, timeInPrevState;
    bool call_scheduler = false;
    while((evt = evtList->getEvent())) { //Pop an event from event queue
        //Get the event details
        proc = evt->evtProcess;
        evtTransTo = evt->transition;
        currentTime = evt->evtTimeStamp;
        timeInPrevState = currentTime - proc->state_ts;
        //Remove current event object from memory
        delete evt; evt = nullptr;
        
        switch(evtTransTo) {  // which state to transition to?
            case TRANS_TO_READY:
                // must come from BLOCKED or from PREEMPTION
                // must add to run queue
                call_scheduler = true; // conditional on whether something is run
                break;
            case TRANS_TO_RUN:
                // create event for either preemption or blocking
                break;
            case TRANS_TO_BLOCK:
                //create an event for when process becomes READY again
                call_scheduler = true;
                break;
            case TRANS_TO_PREEMPT:
                // add to runqueue (no event is generated)
                call_scheduler = true;
                break;
        }
        /*
        if(call_scheduler) {
            if (get_next_event_time() == CURRENT_TIME)
                continue; //process next event from Event queue
            call_scheduler = false; // reset global flag
            if (CURRENT_RUNNING_PROCESS == nullptr) {
                CURRENT_RUNNING_PROCESS = THE_SCHEDULER->get_next_process();
                if (CURRENT_RUNNING_PROCESS == nullptr)
                    continue;
            }
        } */
    }
}

//The random function
int myrandom(int burst) {
    static int ofs = 0;
    if (ofs >= randvals.size()) {
        ofs = 0;
    }
    return 1 + (randvals[ofs++] % burst);
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