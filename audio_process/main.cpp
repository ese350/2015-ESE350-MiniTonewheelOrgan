#include "mbed.h"
#include "DebounceIn.h"
#include "FastAnalogIn.h"

Ticker sample;
Ticker playBackCreator;
//Ticker harmonicCreator1;
//Ticker harmonicCreator2;
Timer loopTimer;
int loopData[5];

FastAnalogIn ain(p15);
AnalogOut aout(p18);
FastAnalogIn ain2(p16);
FastAnalogIn ain3(p17);

DigitalOut motorSpeedSig(p30);
//DigitalIn arpegSwitch(p24);
DebounceIn buttonPress(p11);
DebounceIn loopButton(p27);

volatile int sampleCounter = 0;
int fundCounter = 0;
//int harmonic1Counter = 0;
//int harmonic2Counter = 0;
int playbackCounter = 0;

int i = 0;
unsigned short x;
unsigned short y;
unsigned short z;
const int bufferLength = 2000;

volatile bool sampleFinished = false;
volatile bool playBackCreated = false;
volatile bool playbackRunning = false;
volatile bool readingLoop = false;

unsigned short sampleValues[bufferLength];
unsigned short harmonic1Values[bufferLength];
unsigned short harmonic2Values[bufferLength];
unsigned short playBack[bufferLength];
unsigned short playBackNew[bufferLength];


/*void AInToBuffer() {
    x = ain.read_u16();
    if(sampleCounter < bufferLength) {
        sampleValues[sampleCounter] = x;
    }
    if(sampleCounter < 2* bufferLength) {
        harmonic1Values[sampleCounter] = x;
    }
    if(sampleCounter < 3*bufferLength) {
        harmonic2Values[sampleCounter] = x;
    }
    sampleCounter++;
    if (sampleCounter >= 3*bufferLength) {sampleFinished = true; return;}
    //printf("%d\r\n", sampleCounter);
}*/

void AInToBuffer() {
    //x = ain.read_u16();
    y = ain2.read_u16();
    z = ain3.read_u16();
    if (sampleCounter < bufferLength) {
        //sampleValues[sampleCounter] = x;
        harmonic1Values[sampleCounter] = y;
        harmonic2Values[sampleCounter] = z;
    }
    sampleCounter++;
    //printf("%d\r\n", sampleCounter);
    if (sampleCounter >= bufferLength) {sampleFinished = true; return;}
}

void fundamental() {
    if(fundCounter >= bufferLength) {playBackCreated = true; return;}
    playBack[fundCounter] = (//((int) sampleValues[fundCounter]) + 
                            ((int) harmonic1Values[fundCounter]) +
                            ((int) harmonic2Values[fundCounter]))/2;
    //playBack[fundCounter] = sampleValues[fundCounter];
    fundCounter++;   
}
/*void harmonic1() {
    harmonic1Counter++;
    if (harmonic1Counter >= 2* bufferLength) {
        harmonic1Counter = 0;
    }
}*/

/*void harmonic2() {
    harmonic2Counter++;
    if (harmonic2Counter >= 3*bufferLength) {
        harmonic2Counter = 0;
    }    
}*/

void playBackFunc() {
    aout.write_u16(playBack[playbackCounter]);
    
    playbackCounter++;
    if (playbackCounter >= bufferLength) {
        playbackCounter = 0;
    }
}

void resample() {
    wait(0.02);
    sample.attach(&AInToBuffer, 0.00003333333); //30khz sample freq

    while(!sampleFinished);
    printf("%s\r\n", "sample finished");
    sample.detach(); 


    playBackCreator.attach(&fundamental, 0.00003333333);  // 30khz
    //harmonicCreator1.attach(&harmonic1, 0.0000166666666); // 60khz
    //harmonicCreator2.attach(&harmonic2, 0.000011111111);  // 90khz
    while(!playBackCreated);
    playBackCreator.detach();

    //harmonicCreator1.detach();
    //harmonicCreator2.detach();
        
    sample.attach(&playBackFunc, 0.000011111111); //30khz playback freq
    playbackRunning = true;   
}

void stopPlayback() {
    sample.detach();
    sampleFinished = false;
    playBackCreated = false;
    playbackRunning = false;
    sampleCounter = 0;
    fundCounter = 0;
    //harmonic1Counter = 0;
    //harmonic2Counter = 0;
    playbackCounter = 0;
}


void readLoop() {
    printf("%s\r\n", "readLoop started");
    printf("%s\r\n", "timer started");
    loopTimer.reset();
    loopTimer.start();
    readingLoop = true;
    for (i = 0; i < 5; i++) {
        while (1) {
            if (buttonPress == 1) {
                loopData[i] = loopTimer.read_ms();
                printf("%s\r\n", "button pressed");
                wait(0.25);
                break;
            }
            /*if (loopButton.read() == 1) {
                readingLoop = false;
                break;
            }*/        
        }
        if (readingLoop == false) break;
    }
    printf("%s\r\n", "readLoop finished");
    loopTimer.stop();
    readingLoop = false;
}

void playLoop() {

    while(1){
        loopTimer.reset();
        loopTimer.start();
    for (i = 0; i < 5; i++) {
        motorSpeedSig = 1;
        wait(0.01);
        motorSpeedSig = 0;
        while(loopTimer.read_ms() != loopData[i]);       
        resample();
        wait(0.25);
        stopPlayback();
    }
    }
}

int main(void) {
        motorSpeedSig = 0;
        while (1) {
            //if(arpegSwitch = 1) {arpegOn = true;}
            //else if (arpegSwitch = 0) {arpegOn = false;}
            
            if (buttonPress == 1 && !playbackRunning) {
                printf("%d\r\n", 0);
                resample();
                printf("%d\r\n", 1);
            }
            
            if(playbackRunning && buttonPress == 0) {
                printf("%d\r\n", 2);
                stopPlayback();
                printf("%d\r\n", 3);
            }
            
            if(loopButton == 1 && !readingLoop) {
                readLoop();
                playLoop();
            }
            

        }

}
