#include "mbed.h"
#include "motordriver.h"
#include "DebounceIn.h"

Motor m(p22, p23, p24, 0);

DigitalIn loopSignal(p7);
DebounceIn button1(p11);
DebounceIn button2(p12);
DebounceIn button3(p13);
DebounceIn button4(p14);
DebounceIn loopButton(p19);

int loopData[5];
int i = 0;

void buttonPress(int button) {
    if (button == 1) {
        m.speed(0.001);
    }
    else if (button == 2) {
        m.speed(0.20);
    }
    else if (button == 3) {
        m.speed(0.45);
    }
    else if (button == 4) {
        m.speed(0.6);
    }
}

void readLoop() {
    printf("%s\r\n", "readLoop started");
    for (i = 0; i < 5; i++) {
        while (1) {
            if (button1 == 1) {
                loopData[i] = 1;
                wait(0.25);
                break;
            }
            
            if (button2 == 1) {
                loopData[i] = 2;
                wait(0.25);
                break;
            }
            if (button3 == 1) {
                loopData[i] = 3;
                wait(0.25);
                break;
            }
            if (button4 == 1) {
                loopData[i] = 4;
                wait(0.25);
                break;
            }            
            /*if (loopButton.read() == 1) {
                readingLoop = false;
                break;
            }*/        
        }
    }
    printf("%s\r\n", "readLoop finished"); 
}

void playLoop() {
    while(1) {
    for (i = 0; i < 5; i++) {
        while(1) {
            if (loopSignal == 1) {
                 printf("%d\r\n", loopData[i]);
                 buttonPress(loopData[i]);
                 wait(0.25);
                 break;
            }
        }
    }
    }
}            

int main() {
    while(1) {
        if (button1 == 1) {
            buttonPress(1);
        }
        if (button2 == 1) {
            buttonPress(2);
        }
        if (button3 == 1) {
            buttonPress(3);
        }
        if (button4 == 1) {
            buttonPress(4);
        }
        if (loopButton == 1) {
            readLoop();
            playLoop();
        }   
    }
}
