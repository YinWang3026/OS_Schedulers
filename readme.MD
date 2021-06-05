# Scheduler
The Schduler of the Operating System is responsible for scheduling processes to run on the CPU. Here, different scheduling policies are simulated by using the Discrete Event Simulation and Objected Oriented Programming. The policies simulated are FCFS (First Come First Served), LCFS (Last Come First Served), SRTF (Shortest Remaining Time First), RR (RoundRobin), PRIO (PriorityScheduler), and PREPRIO (Preemptive PRIO). The program reads the input processes, sets the scheduler, and runs until all procsses are complete. The results and performance of each scheduling policy are measured and displayed.

## Running the Program
1. Compile the cpp file using the Makefile
2. ./sched [-v] [-t] [-e] [-s< policy >] [inputfile] [randfile]
- The -v -t -e are optional flags for more runtime information
- The -s flag specifies the scheduling policy 
  - < FLS | R[num] | P[num:maxprios] | E[num:maxprios] >
  - F=FCFS, L=LCFS, S=SRTF
  - R[num]=Round Robin with num Quantum
  - P[num:maxprio]=Priority with num Quantum, and optional max priority (default=4)
  - E=Preemptive Priority with similar arguments to Priority

## Input Files
- Mandatory inputs
- Each line in the input file represents a process, and each processes is presented by arrival time, total cpu time, CPU burst, and IO burst
- The randfile contains random integers for calculating the CPU and IO bursts

