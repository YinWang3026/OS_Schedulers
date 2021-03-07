#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

typedef enum { 
    STATE_RUNNING,
    STATE_BLOCKED,
    STATE_READY
} process_state_t;

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




int main () {
    process_state_t x;
    x = STATE_RUNNING;
    if (x == STATE_RUNNING) {
        cout << "hello world.\n" << endl;
    }
}