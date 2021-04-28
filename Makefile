sched: sched.cpp
	g++ -std=c++11 -g -O sched.cpp -o sched

clean:
	rm -rf sched *~
