<program> [-v] [-t] [-e][-s<schedspec>] inputfile randfile

The scheduler specification is “–s [ FLS | R<num> | P<num>[:<maxprio>] | E<num>[:<maxprios>] ]”, 
where F=FCFS, L=LCFS, S=SRTF and R10 and P10 are RR and PRIO with quantum 10. (e.g. “./sched –sR10”) and E10 is the preemptive prio scheduler. 
Supporting this parameter is required and the quantum is a positive number. 
In addition the number of priority levels is specified in PRIO and PREPRIO with an optional “:num” addition. E.g. “-sE10:5” implies quantum=10 and numprios=5. 
If the addition is omitted then maxprios=4 by default (lookup : sscanf(optarg, “%d:%d”,&quantum,&maxprio))
The –v -t and -e are optional options for debugging