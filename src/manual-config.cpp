#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

struct date Date;
struct time Time;

struct motor mWheelFrontLeft = motor(PORT1, ratio18_1, false); // this motor is a struct for testing purposes
motor mWheelFrontRight = motor(PORT2, ratio18_1, false);
motor mWheelBackLeft = motor(PORT3, ratio18_1, false);
motor mWheelBackRight = motor(PORT4, ratio18_1, false);
motor mIntakeLeft = motor(PORT5, ratio36_1, true);
motor mIntakeRight = motor(PORT6, ratio36_1, false);
motor mOutputLower = motor(PORT7, ratio18_1, false);
controller Controller1 = controller(primary);
motor mOutputUpper = motor(PORT13, ratio6_1, false);
optical sOpticalFront = optical(PORT18);
optical sOpticalBack = optical(PORT20);
inertial sInertial = inertial(PORT12);
distance sDistanceLeft = distance(PORT19);
distance sDistanceRight = distance(PORT14);

signature sigBlue = signature (1, -4201, -2979, -3590, 12859, 16865, 14862, 3, 0);
signature sigRed = signature (2, 7493, 9919, 8706, -145, 353, 104, 3, 0);
signature sigGreen = signature (3, -5985, -5703, -5844, -3889, -3501, -3695, 11, 0);
signature sig4 = signature (4, 0, 0, 0, 0, 0, 0, 3, 0);
signature sig5 = signature (5, 0, 0, 0, 0, 0, 0, 3, 0);
signature sig6 = signature (6, 0, 0, 0, 0, 0, 0, 3, 0);
signature sig7 = signature (7, 0, 0, 0, 0, 0, 0, 3, 0);

vision sVisionUpper = vision (PORT17, 50, sigBlue, sigRed, sigGreen, sig4, sig5, sig6, sig7);
vision sVisionLower = vision (PORT10, 50, sigBlue, sigRed, sigGreen, sig4, sig5, sig6, sig7);