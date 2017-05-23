// Music 256a / CS 476a | fall 2016
// CCRMA, Stanford University
//
// Author: Romain Michon (rmichonATccrmaDOTstanfordDOTedu)
// Description: Exponential smoothing by a unity-dc-gain one-pole lowpass
// This version a bit different than the one used in previous examples
// since it allows to set s to 0 which is very useful when making polyphonic
// synths.

#ifndef SMOOTH_H_INCLUDED
#define SMOOTH_H_INCLUDED

class Smooth{
private:
    float delay,s;
    
public:
    Smooth():delay(0.0),s(0.0){}
    
    ~Smooth(){}
    
    // set the smoothing (pole)
    void setSmooth(float smooth){
        s = smooth;
    }
    
    // compute one sample
    float tick(float input){
        if(s==0){
            delay = input;
            return input;
        }
        else{
            float currentSample = input*(1.0-s);
            currentSample = currentSample + delay;
            delay = currentSample*s;
            return currentSample;
        }
    }
};

#endif  // SMOOTH_H_INCLUDED
