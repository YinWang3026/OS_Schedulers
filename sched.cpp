#include <iostream> //cout
#include <fstream> //ifstream
#include <string>
#include <vector>
#include <stdio.h>
#include <getopt.h> //Arg parsing
#include <stdlib.h>
#include <deque>

using namespace std;

//Flags
int vFlag = 0;
int tFlag = 0;
int eFlag = 0;

//Macro definitions
#define vtrace(fmt...)  do { if (vFlag) { printf(fmt); fflush(stdout); } } while(0)
#define etraceEvtEvtList(evt, evtList)  do { if (eFlag){ printf("  AddEvent(%s): %s==> ", evt->getEventInfo().c_str(), \
    evtList->getEventsString().c_str()); fflush(stdout); } } while(0)
#define etraceEvt(evtList)  do { if (eFlag) { printf("%s\n",evtList->getEventsString().c_str()); fflush(stdout); } }  while(0)

//Enum definitions
enum process_state_t{ 
    STATE_CREATED,
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_DONE
};

string state_string[] = { //Print the state
    "CREATED",
    "READY",
    "RUNNG",
    "BLOCK",
    "DONE"
};

enum process_trans_t{
    TRANS_TO_RUN, //Ready -> Run
    TRANS_TO_READY, //Run -> Ready, Block -> Ready
    TRANS_TO_BLOCK, //Running -> Block
    TRANS_TO_PREEMPT, //Create -> Ready
    TRANS_TO_DONE
};

string trans_string[] = { //Print the state
    "RUNNG",
    "READY",
    "BLOCK",
    "PRMPT",
    "DONE",
};

//Struct definitions
struct process{
    static int count;
    process(int at, int ct, int cb, int io, int p, process_state_t s):
        arrivalTime(at), totalCpuTime(ct), cpuBurst(cb), ioBurst(io),  
        static_priority(p), state(s), state_ts(at), remain_cputime(ct){
            pid = count++;
            dynamic_priority = static_priority - 1;
            ft = 0;
            tt = 0;
            it = 0;
            cw = 0;
        }
    void printProcess(){
        printf("PID: %d, AT: %d, TC: %d, CB: %d, IO: %d, State: %s, rem: %d, sPri: %d, dPri: %d, ts: %d\n",
        pid,arrivalTime,totalCpuTime,cpuBurst,ioBurst,state_string[state].c_str(),remain_cputime,
        static_priority,dynamic_priority,state_ts);
    }
    void printResult(){
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",pid, arrivalTime, totalCpuTime, cpuBurst, ioBurst, static_priority,
            ft, tt, it, cw);
    }
    int pid; //process id
    int arrivalTime; //at
    int totalCpuTime; //total time in cpu / running state
    int cpuBurst;
    int ioBurst;
    int static_priority; //myRandom

    process_state_t state; //current state
    int state_ts; //time that proc put into this state

    int remain_cputime; //total cpu subtract what is done
    int dynamic_priority; //static prio - 1

    int ft; //finishing time
    int tt; //turnaround time: at - ft
    int it; //total time in io / block state
    int cw; //total time in wait / ready state
};

struct event{
    static int count;
    //Proc, ts to fire, to what state
    event(process* p, int ts, process_trans_t t):
        evtTimeStamp(ts), evtProcess(p), transition(t){
        evtid = count++;
    }
    string getEventInfo(){
        return to_string(evtTimeStamp) + ":" + to_string(evtProcess->pid) + ":" + trans_string[transition];
        //printf("%d:%d:%s ", evtTimeStamp,evtProcess->pid,trans_string[transition].c_str());
    }
    int evtid; //An id
    int evtTimeStamp; //Time to fire this event
    process* evtProcess; //The proc associated with this event
    process_trans_t transition; //Which state to bring the proc to
};

int process::count = 0; //Init the static count for pid
int event::count = 0; //Init the static count for evtid

//Class definitions
class Events{
    public:
        Events(){}
        //Return/pops the first evt based on timestamp, and dont forget to delete the evt
        event* getEvent(){
            if (evtqueue.empty()){
                return NULL;
            }
            event* temp = evtqueue.front(); //Save the ptr
            evtqueue.pop_front(); //Delete that pt from queue
            return temp;
        }
        //Add to the list based on its timestamp
        void putEvent(event* evt){
            deque<event*>::iterator it = evtqueue.begin();
            for (it = evtqueue.begin(); it < evtqueue.end(); it++){
                if (evt->evtTimeStamp < (*it)->evtTimeStamp){
                    break;
                }
            }
            it = evtqueue.insert(it,evt);
        }
        //Get the next event time
        int getNextEventTime(){
            if (evtqueue.empty()){
                return -1;
            }
            int temp = evtqueue.front()->evtTimeStamp; //Save the ptr
            return temp;
        }
        //Find evt by ID and remove it
        void rmEvent(event* evt){
        }
        string getEventsString(){
            deque<event*>::iterator it;
            string s = "";
            for (it = evtqueue.begin(); it < evtqueue.end(); it++){
                s += (*it)->getEventInfo() + " ";
            }
            return s;
        }
    private:
        deque<event*> evtqueue;
};

class Scheduler {
    public:
        Scheduler(int q, int p) : quantum(q), maxprio(p){}
        virtual ~Scheduler(){}
        //Each scheduler implements their own virtual funcs
        virtual void add_process(process*)=0;
        virtual process* get_next_process()=0;
        virtual void test_preempty(process*, int)=0;
        //Prints the current run queue
        virtual void printQueue()=0;
        //Returns name of the scheduler
        virtual string getSchedName()=0;
        //Returns the queue size
        virtual int getQueueSize()=0;
        //Returns the quantum
        int getQuantum(){ return quantum; }
        //Returns the maxprio
        int getMaxprio(){ return maxprio; }
    protected:
        int quantum;
        int maxprio;
};

//Constructor are not inherited
class FCFS : public Scheduler{
    public:
        FCFS() : Scheduler(10000, 4) {}
        void add_process(process* p){
            //Adds to end of queue
            runqueue.push_back(p);
        }
        process* get_next_process(){
            if (runqueue.empty()){
                return NULL;
            }
            process* res = runqueue.front(); //Get front pt
            runqueue.pop_front(); //Delete that pt from queue
            return res;
        }
        void test_preempty(process*, int){
            return; //FCFS doesn't use this
        }
        void printQueue(){
            deque<process*>::iterator it;
            for (it = runqueue.begin(); it < runqueue.end(); it++){
                cout << (*it)->pid << ":" << (*it)->state_ts << " ";
            }
            cout << '\n';
        }
        string getSchedName(){ return "FCFS"; }
        int getQueueSize(){ return runqueue.size(); }
    private:
        deque<process*> runqueue; //Proc queue
};

class LCFS : public Scheduler{
    public:
        LCFS() : Scheduler(10000, 4) {}
        void add_process(process* p){
            //Adds to front of queue
            runqueue.push_front(p);
        }
        process* get_next_process(){
            if (runqueue.empty()){
                return NULL;
            }
            process* res = runqueue.front(); //Get front pt
            runqueue.pop_front(); //Delete that pt from queue
            return res;
        }
        void test_preempty(process*, int){
            return; //LCFS doesn't use this
        }
        void printQueue(){
            deque<process*>::iterator it;
            for (it = runqueue.begin(); it < runqueue.end(); it++){
                cout << (*it)->pid << ":" << (*it)->state_ts << " ";
            }
            cout << '\n';
        }
        string getSchedName(){ return "LCFS"; }
        int getQueueSize(){ return runqueue.size(); }
    private:
        deque<process*> runqueue; //Proc queue
};

class SRTF : public Scheduler{
    public:
        SRTF() : Scheduler(10000, 4) {}
        void add_process(process* p){
            deque<process*>::iterator it;
            for (it = runqueue.begin(); it < runqueue.end(); it++){
               if (p->remain_cputime < (*it)->remain_cputime){
                    break;
                }
            }
            it = runqueue.insert(it,p);
        }
        process* get_next_process(){
            if (runqueue.empty()){
                return NULL;
            }
            process* res = runqueue.front(); //Get front pt
            runqueue.pop_front(); //Delete that pt from queue
            return res;
        }
        void test_preempty(process*, int){
            return; //SRTF doesn't use this
        }
        void printQueue(){
            deque<process*>::iterator it;
            for (it = runqueue.begin(); it < runqueue.end(); it++){
                cout << (*it)->pid << ":" << (*it)->state_ts << " ";
            }
            cout << '\n';
        }
        string getSchedName(){ return "SRTF"; }
        int getQueueSize(){ return runqueue.size(); }
    private:
        deque<process*> runqueue; //Proc queue
};

class RR : public Scheduler{
    public:
        RR(int q) : Scheduler(q, 4) {}
        void add_process(process* p){
            runqueue.push_back(p);
        }
        process* get_next_process(){
            if (runqueue.empty()){
                return NULL;
            }
            process* res = runqueue.front(); //Get front pt
            runqueue.pop_front(); //Delete that pt from queue
            return res;
        }
        void test_preempty(process*, int){
            return; //SRTF doesn't use this
        }
        void printQueue(){
            deque<process*>::iterator it;
            for (it = runqueue.begin(); it < runqueue.end(); it++){
                cout << (*it)->pid << ":" << (*it)->state_ts << " ";
            }
            cout << '\n';
        }
        string getSchedName(){ return "RR " + to_string(quantum); }
        int getQueueSize(){ return runqueue.size(); }
    private:
        deque<process*> runqueue; //Proc queue

};

/*class PRIO : public Scheduler{
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

//Global var couz im too lazy to make a place to put them
vector<int> randvals; //Vector containg the random integers

//Function prototypes
void printProcList(vector<process*>&);
int myrandom(int); //Generates a random number
void simulation(Events*, Scheduler*, int&);

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
            int maxprio = 4; //PRIO, PREPRIO, default = 4
            int quantum = 10000; //RR, PRIO, PREPRIO, default = 100000
            sscanf(optarg, "%c%d:%d",&sched,&quantum,&maxprio);
            switch(sched) {
                case 'F': //FCFS
                    myScheduler = new FCFS();
                    break;
                case 'L': //LCFS
                    myScheduler = new LCFS();
                    break;
                case 'S': //SRTF
                    myScheduler = new SRTF();
                    break;
                case 'R': //Round Robin
                    if (quantum <= 0) {
                        cerr << "Error: RR - Quantum <= 0.\n";
                        exit(1);
                    }
                    myScheduler = new RR(quantum);
                    break;
                /*case 'P': //Priority
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
    vtrace("Scheduler: %s, quantum: %d, maxprio: %d\n",myScheduler->getSchedName().c_str(),
        myScheduler->getQuantum(),myScheduler->getMaxprio());

    if ((argc - optind) < 2) { //optind is the index of current argument
        cerr << "Missing input file and rfile\n";
        exit(1);
    }

    //Gettng file names
    char* inputFile = argv[optind];
    char* randomFile = argv[optind+1];
    vtrace("Input file: %s\trfile: %s\n",inputFile,randomFile);
    
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
    
    if (vFlag) { 
        printProcList(procList); 
    } //Print the process list

    //Begin simulation
    int totalIoUtil; //Simulation calculates total IO utilization
    simulation(evtList, myScheduler, totalIoUtil);

    //Printing results
    int finishTime = 0; //the largest ft in procs
    double cpuUtil = 0; //Sum up the total cpu time and divide by ft
    double ioUtil = double(totalIoUtil); //Convert to double
    double avgTT = 0; //sum the proc tt and divide by # of procs
    double avgCW = 0; //sum the proc cw and divide by # of procs
    double throughput = 0; //# of procs divide by ft
    double numProcs = procList.size();
    printf("%s\n",myScheduler->getSchedName().c_str());
    for(int i=0; i<procList.size(); i++){
        p = procList[i];
        finishTime = max(finishTime, p->ft);
        cpuUtil += p->totalCpuTime;
        avgTT += p->tt;
        avgCW += p->cw;
        p->printResult();
        delete p; //Pointer to proc is not needed anymore
    }
    finishTime = double(finishTime);
    cpuUtil = (cpuUtil/finishTime)*100;
    ioUtil = (ioUtil/finishTime)*100;
    avgTT /= numProcs;
    avgCW /= numProcs;
    throughput = (numProcs/finishTime)*100;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", finishTime, cpuUtil, ioUtil, avgTT, avgCW, throughput);

    //Clean up
    delete myScheduler; //Deleting scheduler
    delete evtList; //Deleting evts
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

void simulation(Events* evtList, Scheduler* myScheduler, int& totalIoUtil){
    event* evt;
    process* proc;
    process* runningProc;
    process_trans_t evtTransTo;
    int currentTime, timeInPrevState, rcb, rio, ioStart, ioEnd;
    int quantum = myScheduler->getQuantum();
    bool call_scheduler;

    if (eFlag){
        printf("ShowEventQ: %s\n", evtList->getEventsString().c_str());
    }
    runningProc = NULL; //No proc running
    call_scheduler = false; //Not calling the scheduler
    totalIoUtil = 0;
    ioStart = 0; //Start period of current io
    ioEnd = 0; //End period of current io

    while((evt = evtList->getEvent())) { //Pop an event from event queue
        //Get the event details
        proc = evt->evtProcess; //The proc associated with the evt
        evtTransTo = evt->transition; //To which state
        currentTime = evt->evtTimeStamp; //When did this evt fire
        timeInPrevState = currentTime - proc->state_ts; //Time spend it previous state
        //Remove current event object from memory
        delete evt; evt = nullptr;
        
        switch(evtTransTo) {
            //The ready state
            case TRANS_TO_READY:    // must come from BLOCKED or from PREEMPTION
                //timestamp of the evt, pid, how long was prev state: FROM -> TO
                vtrace("%d %d %d: %s -> %s\n", currentTime, proc->pid, timeInPrevState, 
                    state_string[proc->state].c_str(), state_string[STATE_READY].c_str());
                //Update proc
                proc->state = STATE_READY; //Update proc state
                proc->state_ts = currentTime; //Update proc state_ts, aka now
                //Adding proc to run queue
                myScheduler->add_process(proc); 
                call_scheduler = true;
                //runningProc = NULL; //Might have a proc currently running
                break;

            //The running state
            case TRANS_TO_RUN: // create event for either preemption or blocking
                //Generate random cpu brust
                rcb = myrandom(proc->cpuBurst);
                if (rcb > proc->remain_cputime){
                    rcb = proc->remain_cputime; //if rcb > remain cpu, reduce to remain cpu
                }
                //Print info
                vtrace("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", currentTime, proc->pid, timeInPrevState, 
                    state_string[proc->state].c_str(), state_string[STATE_RUNNING].c_str(), 
                    rcb, proc->remain_cputime, proc->dynamic_priority);
                //Update proc
                proc->state = STATE_RUNNING; //Update proc state
                proc->state_ts = currentTime; //Update proc state_ts, aka now
                proc->cw += timeInPrevState; //Time in ready
                if (rcb <= quantum){ //Moving to blocked state
                    proc->remain_cputime -= rcb; //reduce the remaining time
                    if (proc->remain_cputime == 0){
                        //Process complete
                        evt = new event(proc, currentTime+rcb, TRANS_TO_DONE);
                    } else{
                        //fire new event at curr time + rcb
                        evt = new event(proc, currentTime+rcb, TRANS_TO_BLOCK);
                    }
                    etraceEvtEvtList(evt,evtList); //Print queue before
                    //Update evtList
                    evtList->putEvent(evt); //Add evt to qeueue
                    etraceEvt(evtList); //Print queue after
                    
                } else { // Greater than quantum, get preempted

                }
                
                break;
            //The block state
            case TRANS_TO_BLOCK:
                //Generate random io brust
                rio = myrandom(proc->ioBurst);
                //Print info
                vtrace("%d %d %d: %s -> %s ib=%d rem=%d\n", currentTime, proc->pid, timeInPrevState, 
                    state_string[proc->state].c_str(), state_string[STATE_BLOCKED].c_str(), 
                    rio, proc->remain_cputime);
                //Update proc
                proc->state = STATE_BLOCKED; //Update proc state
                proc->state_ts = currentTime; //Update proc state_ts, aka now
                proc->it += rio; //Time in IO
                //Calculate the total IO Utilization
                if (currentTime > ioEnd){ //No overlap
                    totalIoUtil += rio;
                    ioStart = currentTime;
                    ioEnd = currentTime + rio;
                } else if (currentTime <= ioEnd && ((currentTime+rio) > ioEnd)){ //Overlap
                    totalIoUtil += ((currentTime + rio) - ioEnd); //Need to take out overlap
                    ioEnd = currentTime + rio;
                    ioStart = currentTime;
                } //else, total overlap, just ignore
                //fire new event at curr time + rio
                evt = new event(proc, currentTime+rio, TRANS_TO_READY);
                etraceEvtEvtList(evt,evtList); //Print queue before
                //Update evtList
                evtList->putEvent(evt); //Add evt to qeueue
                etraceEvt(evtList); //Print queue after
                //Update dynamic prio
                proc->dynamic_priority = proc->static_priority - 1; //When finishing I/O
                //Proc is blocked, nothing running call scheduler
                call_scheduler = true;
                runningProc = NULL;
                break;
            //The preempt state, prob not used in FLS
            case TRANS_TO_PREEMPT:
                // add to runqueue (no event is generated)
                myScheduler->add_process(proc); //Adding proc to run queue

                call_scheduler = true;
                runningProc = NULL;

                break;

            case TRANS_TO_DONE:
                vtrace("%d %d %d: %s -> %s rem=%d prio=%d\n", currentTime, proc->pid, timeInPrevState, 
                    state_string[proc->state].c_str(), state_string[STATE_DONE].c_str(), 
                    proc->remain_cputime, proc->dynamic_priority);

                proc->state = STATE_DONE; //Update proc state
                proc->state_ts = currentTime; //Update proc state_ts, aka now
                proc->ft = currentTime; //Finishing time
                proc->tt = currentTime - proc->arrivalTime; //Turn around time
                //Proc is finished, nothing running call scheudler
                call_scheduler = true;
                runningProc = NULL;
                break;
        }
        if(call_scheduler) {
            if (evtList->getNextEventTime() == currentTime)
                continue; //Have to handle more events at the same time stamp
            
            call_scheduler = false; // reset global flag
            if (runningProc == NULL) { //Currently not running a proc

                if (tFlag) { //Print the run queue
                    printf("SCHED (%d): ", myScheduler->getQueueSize());
                    myScheduler->printQueue();
                }

                runningProc = myScheduler->get_next_process(); //Give me a proc to run
                if (runningProc == NULL){
                     //No proc to run
                    continue;
                }else{
                    //There is a proc to run, make an event
                    //Creating new event ready->run
                    evt = new event(runningProc, currentTime, TRANS_TO_RUN);
                    etraceEvtEvtList(evt,evtList); //Print queue before
                    //Update evtList
                    evtList->putEvent(evt); //Add evt to qeueue
                    etraceEvt(evtList); //Print queue after
                }
            }
        } 
    }
}

