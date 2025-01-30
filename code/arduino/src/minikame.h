#ifndef minikame_h
#define minikame_h

#include <Servo.h>
#include <Octosnake.h>

class MiniKame{
public:
    void init();
    void run(float steps, int period);
    void walk(float steps, int period);
    void omniWalk(float steps, int T, bool side, float turn_factor);
    //void backward(float steps, int period);
    void turnL(float steps, int period);
    void turnR(float steps, int period);
    void moonwalkL(float steps, int period);
    void dance(float steps, int period);
    void upDown(float steps, int period);
    void pushUp(float steps, int period);
    void hello();
    void jump();
    void home();
    void zero();
    void frontBack(float steps, int period);
    void spinDance(float steps, int T=1000);

    void setServo(int id, float target);
    void reverseServo(int id);
    float getServo(int id);
    void moveServos(int time, float target[8]);

    bool isMoving = false;
    unsigned long moveStartTime = 0;
    int currentMove = 0;  // 0=none, 1=walk, 3=turnL, 4=turnR, etc.

    void updateMovement();  // Call this frequently to update movement

private:
    Oscillator oscillator[8];
    Servo servo[8];
    int board_pins[8];
    int trim[8];
    bool reverse[8];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[8];
    float _servo_position[8];

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
};

#endif
