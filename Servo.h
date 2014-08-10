#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#ifndef H_SERVO
#define H_SERVO

extern std::string servodWrite;

class Servo
{
public:
    Servo(int servoID, float minPW, float maxPW);
    
    void setVal(float val);
    void turnToAngle(float angle);
    void run();

    float getAngle();
private:
    int mServoID;

    float mMinPW;
    float mMaxPW;
    
    float mVal;

    float mMaxAngle;
    float mLowPW; //The lowest PW that the servo can have
    float mHighPW; //The highest PW that the servo can have
};
#endif
