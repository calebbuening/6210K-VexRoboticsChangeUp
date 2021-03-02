/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Practically all the members of 6210K                      */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "miscmethods.h"

using namespace vex;

// A global instance of competition
competition Competition;

// Other global variables
char mode = 'N';
bool disableIntakes = false;
// these are global variables
float ForwardDistance;
float TurnDistance;
float StrafeDistance;
float ForwardVelocity;
float TurnVelocity;
float StrafeVelocity;

void pre_auton(void) {
  vexcodeInit();

  // Print options
  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("V-auton B-none");
  Controller1.Screen.setCursor(2, 1);
  Controller1.Screen.print("<-L1 ^-L2 >-L3");
  Controller1.Screen.setCursor(3, 1);
  Controller1.Screen.print("Y-R1 X-R2 A-R3");

  disableIntakes = true;

  // Get input
  while (mode == 'N') {
    if (Controller1.ButtonDown.pressing())
      mode = 'V';
    if (Controller1.ButtonB.pressing())
      break;

    if (Controller1.ButtonY.pressing())
      mode = 'Y';
    if (Controller1.ButtonA.pressing())
      mode = 'A';
    if (Controller1.ButtonX.pressing())
      mode = 'X';

    if (Controller1.ButtonLeft.pressing())
      mode = '<';
    if (Controller1.ButtonRight.pressing())
      mode = '>';
    if (Controller1.ButtonUp.pressing())
      mode = '^';
    if (Controller1.ButtonR1.pressing()) {
      mode = 'S';
    }
  }
  Controller1.Screen.clearScreen();

  // Calibrate
  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("Calibrate?");
  Controller1.Screen.setCursor(2, 1);
  Controller1.Screen.print("Press A");
  waitUntil(Controller1.ButtonA.pressing());

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.Screen.print("Calibrating...");
  sInertial.calibrate();
  waitUntil(!sInertial.isCalibrating());

  Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1, 1);
  Controller1.rumble("..");
  Controller1.Screen.print("DONE");
  wait(500, msec);
  Controller1.Screen.clearScreen();
  disableIntakes = false;

  // Activiate the opticals
  sOpticalFront.setLight(ledState::on);
  sOpticalBack.setLight(ledState::on);
  sOpticalFront.setLightPower(100, pct);
  sOpticalBack.setLightPower(100, pct);
}

// Experimental reusable auton methods

void scoreFirstCornerGoal(int dir) {
  int leftVelocity;
  int rightVelocity;
  if (dir == LEFT) {
    leftVelocity = 0;
    rightVelocity = -40;
  } else {
    leftVelocity = 40;
    rightVelocity = 0;
  }

  // Get to the goal
  mWheelFrontLeft.spin(fwd, leftVelocity, pct);
  mWheelFrontRight.spin(fwd, rightVelocity, pct);
  mWheelBackLeft.spin(fwd, leftVelocity, pct);
  mWheelBackRight.spin(fwd, rightVelocity, pct);
  vexDelay(300);
  mWheelFrontLeft.spin(fwd, 0, pct);
  mWheelFrontRight.spin(fwd, 0, pct);
  mWheelBackLeft.spin(fwd, 0, pct);
  mWheelBackRight.spin(fwd, 0, pct);

  // Ensure we are in the goal
  driveForward(100, 1000);

  // Outtake the preload
  output(100, 400);

  // Make sure we are out of the goal
  driveForward(-20, 500);
}

// Used for turning when we don't need accuracy
void turnFast(double angle){
  double error = fabs(fabs(angle) - fabs(sInertial.rotation(deg)));
  int rightX;
  while(error > 10){
    error = fabs(fabs(angle) - fabs(sInertial.rotation(deg)));
    if(error < 40)
      rightX = (2 * error) + 20;
    else
      rightX = 90;
    mWheelFrontLeft.spin(fwd, rightX, pct);
    mWheelFrontRight.spin(fwd, rightX, pct);
    mWheelBackLeft.spin(fwd, rightX, pct);
    mWheelBackRight.spin(fwd, rightX, pct);
  }
  driveForward(0, 0);
}

// Used for turning with accuracy
void turnTo(double angle){
  while(fabs(fabs(angle) - fabs(sInertial.rotation(deg))) > 2 || fabs(TurnVelocity) > 10) // uses global variable TurnVelocity
  {
    // Calculate error
    double error = angle - sInertial.rotation(deg);
    if(error > 90) error = 90;   // cap positive motor power at +90
    if(error < -90) error = -90; // cap negative motor power at -90
    mWheelFrontLeft.spin(fwd, error, pct);
    mWheelFrontRight.spin(fwd, error, pct);
    mWheelBackLeft.spin(fwd, error, pct);
    mWheelBackRight.spin(fwd, error, pct);
    wait(5, msec);
  }
  driveForward(0, 0);
}

// Self explanatory
void driveViaDistanceGyro(double dist, double a){
  // reset all motor encoders to zero
  // 10000 units is equal to 56" of travel
  mWheelFrontLeft.resetRotation();
  mWheelBackLeft.resetRotation();
  mWheelFrontRight.resetRotation();
  mWheelBackRight.resetRotation();
  int d = 0;
  if(d < dist){
    while (d < dist){
      int leftY = 80;
      int rightX = (a - sInertial.rotation(deg)) * 3;
      int leftX = 0;
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
      wait(5, msec);
      d = mWheelFrontLeft.rotation(rotationUnits::raw) - mWheelFrontRight.rotation(rotationUnits::raw) + mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
    }
  }else{
    while (d > dist){
      int leftY = -80;
      int rightX = (a - sInertial.rotation(deg)) * 3;
      int leftX = 0;
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
      wait(5, msec);
      d = mWheelFrontLeft.rotation(rotationUnits::raw) - mWheelFrontRight.rotation(rotationUnits::raw) + mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
    }
  }
  driveForward(0, 0);
}

// Using lower camera on the red sig
void driveViaDistanceGyroCamera(double dist, double a){
  // reset all motor encoders to zero
  // 10000 units is equal to 56" of travel
  mWheelFrontLeft.resetRotation();
  mWheelBackLeft.resetRotation();
  mWheelFrontRight.resetRotation();
  mWheelBackRight.resetRotation();
  int d = 0;

  double leftY;
  double leftX;
  double rightX;

    leftY = 80;
    while (d < dist){
      rightX = (a - sInertial.rotation(deg)) * 3;
      leftX = 0;
      sVisionLower.takeSnapshot(sigRed);
      // Width > 40
      if (sVisionLower.objectCount > 0 && sVisionLower.largestObject.width > 40) {
        leftX = (sVisionLower.largestObject.centerX - 180) * .8;
      }
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
      wait(10, msec);
      d = mWheelFrontLeft.rotation(rotationUnits::raw) - mWheelFrontRight.rotation(rotationUnits::raw) + mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
    }
  driveForward(0, 0);
}

// Self explanatory
void strafeViaDistanceGyro(double dist, double a){
  // reset all motor encoders to zero
  // 10000 units is equal to 56" of travel
  mWheelFrontLeft.resetRotation();
  mWheelBackLeft.resetRotation();
  mWheelFrontRight.resetRotation();
  mWheelBackRight.resetRotation();
  int d = 0;

  d = mWheelFrontLeft.rotation(rotationUnits::raw) + mWheelFrontRight.rotation(rotationUnits::raw) - mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);

  if(d < dist){
    while (d < dist){
      int leftY = 0;
      int rightX = (a - sInertial.rotation(deg)) * 3;
      int leftX = 50;
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
      wait(5, msec);
      d = mWheelFrontLeft.rotation(rotationUnits::raw) + mWheelFrontRight.rotation(rotationUnits::raw) - mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
    }
  }else{
    while (d > dist){
      int leftY = -80;
      int rightX = (a - sInertial.rotation(deg)) * 3;
      int leftX = 0;
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
      wait(5, msec);
      d = mWheelFrontLeft.rotation(rotationUnits::raw) - mWheelFrontRight.rotation(rotationUnits::raw) + mWheelBackLeft.rotation(rotationUnits::raw) - mWheelBackRight.rotation(rotationUnits::raw);
    }
  }
  driveForward(0, 0);
}

// Using the signature passed into the method and upper camera
void driveViaTimeGyroCamera(double timeInMS, double a, signature sig){
  // This method drives according to time and corrects with camera (strafe) and gyro (angle)
  // The loop breaks if the robot runs into an obstacle
  double startTime = Brain.timer(msec);
  int leftX;
  int leftY;
  int rightX;
  while (Brain.timer(msec) - startTime < timeInMS){
    sVisionUpper.takeSnapshot(sig);
    if (sVisionUpper.objectCount > 0 && sVisionUpper.largestObject.width > 40) {
      leftX = (sVisionUpper.largestObject.centerX - 180) * .6; // used to be 0.8
    } else {
      leftX = 0;
    }
    leftY = 50;
    rightX = (a - sInertial.rotation(deg)) * 2; // used to be 3
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    if (Brain.timer(msec) - startTime > 400) {
      if (ForwardVelocity < 20) {
        break;
      }
    }
    // this line gives the robot 200ms to speed up (assuming it started at rest)
    // ForwardVelocity is the sum of the 4 wheels, so leftY would make ForwardVelocity = 320
    // ForwardVelocity will go towards zero when the robot runs into something and stops
    wait(5, msec);
  }
  driveForward(0, 0);
}

// Self explanatory
void driveViaTimeGyro(double timeInMS, double a){
  // This method drives according to time and corrects with camera (strafe) and gyro (angle)
  // The loop breaks if the robot runs into an obstacle
  double startTime = Brain.timer(msec);
  int leftX;
  int leftY;
  int rightX;
  while (Brain.timer(msec) - startTime < timeInMS){
    leftX = 0;
    leftY = 50;
    rightX = (a - sInertial.rotation(deg)) * 2; // used to be 3
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    if (Brain.timer(msec) - startTime > 400) {
      if (ForwardVelocity < 20) {
        break;
      }
    }
    // this line gives the robot 200ms to speed up (assuming it started at rest)
    // ForwardVelocity is the sum of the 4 wheels, so leftY would make ForwardVelocity = 320
    // ForwardVelocity will go towards zero when the robot runs into something and stops
    wait(5, msec);
  }
  driveForward(0, 0);
}

void driveBackwardsViaTimeGyro(double timeInMS, double a){
  // This method drives according to time and corrects with camera (strafe) and gyro (angle)
  // The loop breaks if the robot runs into an obstacle
  double startTime = Brain.timer(msec);
  int leftX;
  int leftY;
  int rightX;
  while (Brain.timer(msec) - startTime < timeInMS){
    leftX = 0;
    leftY = -50;
    rightX = (a - sInertial.rotation(deg)) * 2; // used to be 3
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    if (Brain.timer(msec) - startTime > 400) {
      if (ForwardVelocity < 20) {
        break;
      }
    }
    // this line gives the robot 200ms to speed up (assuming it started at rest)
    // ForwardVelocity is the sum of the 4 wheels, so leftY would make ForwardVelocity = 320
    // ForwardVelocity will go towards zero when the robot runs into something and stops
    wait(5, msec);
  }
  driveForward(0, 0);
}

void alignToGoal(double a){
  double startTime = Brain.timer(msec);
  // Needs tuning:  the accuracy (2 degrees) the velocity indicator (40 units) and gyro multiplier (x 5)
  while(fabs(fabs(a) - fabs(sInertial.rotation(deg))) > 2 || fabs(TurnVelocity) > 40) // uses global variable TurnVelocity
  {
    // Calculate error
    double error = (a - sInertial.rotation(deg)) * 5;
    if(error > 90) error = 90;   // cap positive motor power at +90
    if(error < -90) error = -90; // cap negative motor power at -90
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, error, pct);  // when left and right are same polarity
    mWheelBackRight.spin(fwd, error, pct); // the robot rotates (turns)
    wait(5, msec);
    if(Brain.timer(msec) - startTime > 1000) break;
  }
  driveForward(0, 0);
}

void strafeUntilGreen(int speed, double a){
  int leftX;
  int leftY;
  int rightX;
  leftX = speed;
  leftY = 0;
  sVisionUpper.takeSnapshot(sigGreen);
  // looks 80 pixels in advance to accomidate for overshoot
  while(sVisionUpper.objectCount == 0 || fabs(sVisionUpper.largestObject.centerX - 180) > 60 || sVisionUpper.largestObject.width < 40){
    wait(10, msec);
    sVisionUpper.takeSnapshot(sigGreen);
    rightX = (a - sInertial.rotation(deg)) * 2;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);

  }
  mWheelBackLeft.spin(fwd, 0, pct);
  mWheelFrontLeft.spin(fwd, 0, pct);
  mWheelBackRight.spin(fwd, 0, pct);
  mWheelFrontRight.spin(fwd, 0, pct);
  //setStopping(brake);
}
void strafeUntilRed(int speed, double a){
  int leftX;
  int leftY;
  int rightX;
  leftX = speed;
  leftY = 0;
  sVisionLower.takeSnapshot(sigRed);
  // looks 80 pixels in advance to accomidate for overshoot
  while(sVisionLower.objectCount == 0 || fabs(sVisionLower.largestObject.centerX - 180) > 60 || sVisionLower.largestObject.width < 40){
    wait(10, msec);
    sVisionLower.takeSnapshot(sigRed);
    rightX = (a - sInertial.rotation(deg)) * 2;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);

  }
  mWheelBackLeft.spin(fwd, 0, pct);
  mWheelFrontLeft.spin(fwd, 0, pct);
  mWheelBackRight.spin(fwd, 0, pct);
  mWheelFrontRight.spin(fwd, 0, pct);
  //setStopping(brake);
}

void ejectBalls(){
  mOutputLower.startSpinFor(-8, rotationUnits::rev, 100, velocityUnits::pct);
  mOutputUpper.spinFor(-8, rotationUnits::rev, 100, velocityUnits::pct);
}

void waitForRed(){
  double startTime = Brain.timer(msec);
  while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
    wait(10, msec);
    sVisionUpper.takeSnapshot(sigRed);
  }
}

void autonomous(void) {
  // output(100, 5000); // Spit the ball into the goal
  // intake(-75, 1000); // Flick the intakes outward
  // driveForward(-50, 1000); // Back out and away from the goal

  mWheelFrontLeft.spin(fwd);
  mWheelFrontRight.spin(fwd);
  mWheelBackLeft.spin(fwd);
  mWheelBackRight.spin(fwd);
  mIntakeLeft.spin(fwd);
  mIntakeRight.spin(fwd);
  mOutputLower.spin(fwd);
  mOutputUpper.spin(fwd);

  mWheelFrontLeft.spin(fwd, 0, pct);
  mWheelFrontRight.spin(fwd, 0, pct);
  mWheelBackLeft.spin(fwd, 0, pct);
  mWheelBackRight.spin(fwd, 0, pct);
  mIntakeLeft.spin(fwd, 0, pct);
  mIntakeRight.spin(fwd, 0, pct);
  mOutputLower.spin(fwd, 0, pct);
  mOutputUpper.spin(fwd, 0, pct);
  mOutputLower.setBrake(hold);
  mOutputUpper.setBrake(hold);

  int leftX;
  int leftY;
  int rightX;
  double startTime;

  //        XXXXXXXXXXXXXX
  //      XXXXXXXXXXXXXXXXXXXX
  //    XXXXXXXXXXXXXXXXXXXXXXXX
  //    XXXXXXX          XXXXXXX
  //    XXXXXXX          XXXXXXX
  //    XXXXXXX          XXXXXXX
  //    XXXXXXXXXXXXXXXXXXXXXXXX
  //    XXXXXXXXXXXXXXXXXXXXXXXX
  //    XXXXXXXXXXXXXXXXXXXXXXXX
  //    XXXXXXX          XXXXXXX
  //    XXXXXXX          XXXXXXX    
  //    XXXXXXX          XXXXXXX
  //    XXXXXXX          XXXXXXX
  //    XXXXXXX          XXXXXXX

  // STATE AUTONOMOUS
  if (mode == 'V') {
    sInertial.setRotation(-57, deg);

    // PART 1 - Deploy Camera and Hood and flick ball into goal
    mOutputUpper.spin(fwd, 100, pct);
    wait(300, msec);
    mOutputUpper.spin(fwd, 0, pct);


  ///////////////////////////////////////////////////////


    // PART 2 - Get ball 1,
    mOutputLower.spin(fwd, 100, pct);
    driveViaDistanceGyro(6000, -57);

    // Step 2 - Deploy Arms
    intakeIn();
    wait(1000, msec);
    intakeOff();

    // Prepare for strafe
    turnTo(-180);
    intakeIn();

    // Strafe until we see the goal
    strafeUntilGreen(50, -180);

    // open arms
    intakeOpenAuton();
    wait(100, msec);

    // drive into goal
    driveViaTimeGyroCamera(1000, -180, sigGreen);
    alignToGoal(-180);
    intakeOff();

    //Score and descore
    intakeIn();
    wait(400, msec);
    mOutputUpper.spin(fwd, 100, pct);
    mOutputLower.spin(fwd, 100, pct);
    waitForRed();
    mOutputUpper.spin(fwd, 0, pct);
    intakeOpenAuton();


  ///////////////////////////////////////////////////////////
    // PART 3 - Back up and eject blue
    driveViaDistanceGyro(-1900, -180);
    wait(100, msec);

    
    turnFast(-225); // turn quickly to eject blue

    ejectBalls();

    turnTo(-270);
    strafeViaDistanceGyro(400,-270);
    turnTo(-360); // face center goal

    // Drive towards center goal and scoop red ball along the way
    mOutputLower.spin(fwd,100, pct);
    driveViaTimeGyroCamera(2000, -360, sigBlue);
    // Strike center goal and align
    alignToGoal(-360);
    // Descore first blue from center goal
    intakeIn();
    // Eject red and score into center goal
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,70, pct);
    // Watch for red ball going into goal
    waitForRed();
    // Stop upper output scoring wheel
    mOutputUpper.spin(fwd,0, pct);
    intakeOpenAuton();
    wait(400, msec);
    // Intake second blue
    intakeIn();
    mOutputLower.spin(fwd,80, pct);
    wait(800, msec);
    
    intakeOpenAuton();
    
    // Back away from center goal
    driveViaDistanceGyro(-1700, -360);
    // Turn towards goal 1 to eject balls
    turnFast(-205);
    ejectBalls();
    // Close intake arms
    mIntakeLeft.startSpinFor(-3, rotationUnits::rev, 90, velocityUnits::pct);
    mIntakeRight.startSpinFor(-3, rotationUnits::rev, 90, velocityUnits::pct);

    /////////////////////////////////////////////////////////////////////////
    // PART 4 - goal 4
    // Get red ball
    turnTo(-135); // did trig to find this
    intakeOpenAuton();
    mOutputLower.spin(fwd,80, pct);
    driveViaDistanceGyroCamera(5300, -135);
    intakeIn();

    // Go to goal
    turnTo(-135); //did trig to find this
    driveViaTimeGyroCamera(4000, -135, sigGreen);
    alignToGoal(-135);

    // Goal 4 score red and descore blue
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,80, pct);
    waitForRed();
    mOutputUpper.spin(fwd,0, pct);
    


  ///////////////////////////////////////////////////////////////////////////////////////////////////////


    // Part 5 - Back out of the goal

    driveViaDistanceGyro(-4500, -146.31);
    intakeIn();

    // Turn to face the wall
    turnFast(-270);
    // Eject Blue
    ejectBalls();
    wait(800, msec);
    // Turn to red
    turnTo(-90);
    // Get the red ball
    intakeOpenWall();
    mOutputLower.spin(fwd,80, pct);
    driveViaTimeGyro(3000, -90);
    intakeIn();

    // Scoot back
    driveViaDistanceGyro(-2000, -90);
    

    // Strafe to goal
    strafeUntilGreen(50, -90);

    // prep for the goal
    intakeOpenAuton(); // open arms
    wait(100, msec);

    // drive into goal
    driveViaTimeGyroCamera(2000, -90, sigGreen);
    alignToGoal(-90);
    intakeOff();

    //Score and descore
    intakeIn();
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,100, pct);
    waitForRed();
    mOutputUpper.spin(fwd,0, pct);
  /*

  ////////////////////////////////////////////////////////////////////////


    // PART 6 - Back up and eject blue
    driveViaDistanceGyro(-1700, -90);

    intakeIn();

    // Turn and ditch the blue ball
    turnFast(-135);
    mOutputLower.startSpinFor(10, rotationUnits::rev, 90, velocityUnits::pct);
    mOutputUpper.spinFor(10, rotationUnits::rev, 90, velocityUnits::pct);

    // Turn back to face the wall
    turnTo(-90);

    // Strafe to the next red ball
    strafeUntilRed(50, -90;)
    
    // Get the red ball
    intakeOpenWall();
    mOutputLower.spin(fwd, 80, pct);
    driveViaTimeGyro(3000, -90);
    intakeIn();

    // Scoot back
    driveViaDistanceGyro(-2000, -90);

    // Rotate toward goal 6
    turnTo(-33.69);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Part 7 - Goals 6 and 7
    // Score goal 6
    intakeIn();
    driveViaTimeGyroCamera(5000, -33.69, sigGreen);
    alignToGoal(-33.69);
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,100, pct);
    startTime = Brain.timer(msec);
    while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
      wait(10, msec);
      sVisionUpper.takeSnapshot(sigRed);
    }
    mOutputUpper.spin(fwd,0, pct)
    intakeOff();
    // Back up, turn and eject blue
    driveViaDistanceGyro(-3000, -33.69);
    turnTo(-180);
    mOutputLower.startSpinFor(10, rotationUnits::rev, 90, velocityUnits::pct);
    mOutputUpper.spinFor(10, rotationUnits::rev, 90, velocityUnits::pct);
    // Get Red
    strafeViaDistanceGyro(700, -180);
    turnTo(-270);
    mOutputLower.spin(fwd, 100, pct);
    driveViaDistanceGyroCamera(4000, -270);
    intakeIn();
    turnTo(-360);

    strafeUntilGreen(50, -360;

    intakeOpenAuton();
    wait(100, msec);

    // drive into goal
    driveViaTimeGyroCamera(1000, -360, sigGreen);
    alignToGoal(-360);
    intakeOff();
    intakeIn();
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,100, pct);
    startTime = Brain.timer(msec);
    while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
      wait(10, msec);
      sVisionUpper.takeSnapshot(sigRed);
    }
    mOutputUpper.spin(fwd,0, pct);
    mOutputLower.spin(fwd,80, pct);
    intakeOff();
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Goal 8 and Experimental: knock blue ball into center, score corner

    driveBackwardsViaTimeGyro(5000, -360);
    driveViaDistanceGyro(1000, -360);
    turnTo(-396.87); // did trig to find this
    intakeOpenAuton();
    mOutputLower.spin(fwd, 80, pct);
    driveViaDistanceGyro(10300, -396.87);
    intakeIn();
    turnTo(-416.31); //did trig to find this
    driveViaTimeGyroCamera(4000, -416.31, sigGreen);
    alignToGoal(-416.31);

    // Score goal 8
    mOutputUpper.spin(fwd,100, pct);
    mOutputLower.spin(fwd,100, pct);
    startTime = Brain.timer(msec);
    while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
      wait(10, msec);
      sVisionUpper.takeSnapshot(sigRed);
    }
    mOutputUpper.spin(fwd,0, pct);
    mOutputLower.spin(fwd,0, pct);
    intakeOff();
    // Back out
    driveViaDistanceGyro(-6000, -416.31);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // PART 8 - Goal nine followed by CELEBRATION!
    //turnTo(-540);

    // Get the red ball
    intakeOpenWall();
    mOutputLower.spin(fwd,100, pct);
    driveViaTimeGyro(3000, -540);
    intakeIn();


    // Scoot back
    driveViaDistanceGyro(-2000, -540);

    // Strafe to goal
    strafeUntilGreen(50, -540);

     // prep for the goal
     intakeOpenAuton(); // open arms
     wait(100, msec);

     // drive Into Goal
     driveViaTimeGyroCamera(1000, -540, sigGreen);
     alignToGoal(-540);
     intakeOff();

     //Score and descore
     intakeIn();
     mOutputUpper.spin(fwd,100, pct);
     mOutputLower.spin(fwd,100, pct);
     startTime = Brain.timer(msec);
     while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
       wait(10, msec);
       sVisionUpper.takeSnapshot(sigRed);
     }
     mOutputUpper.spin(fwd,0, pct);
     mOutputLower.spin(fwd,0, pct);
     intakeOff();
     intakeOpenWall();
     driveViaDistanceGyro(-1700, -540);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // PART 9 - Experimental: Go back to the first flick-in goal and score 1 red + descore 2 blue
     strafeUntilRed(50, -540);
     mOutputLower.spin(fwd, 100, pct);
     driveViaTimeGyro(3000, -540);
     intakeIn();
     wait(200);// This may not be needed
     driveViaDistanceGyro(-4500, -540);
     turnTo(-585);
     driveViaTimeGyroCamera(5000, -585, sigGreen);
     mOutputUpper.spin(fwd, 100, pct);
     startTime = Brain.timer(msec);
     while(sVisionUpper.largestObject.width < 100 && Brain.timer(msec) - startTime < 1000){
       wait(10, msec);
       sVisionUpper.takeSnapshot(sigRed);
     }
     mOutputLower.spin(fwd, 0, pct);
     mOutputUpper.spin(fwd, 0, pct);
     intakeOpenAuton();
     wait(400, msec);
     intakeIn();
     driveViaDistanceGyro(-4000, -585);


 // DONE!!!!!!!!!!!!!!!!!!!!!!!!!!!

  */
  }

  // Right 1
  if (mode == 'Y') {
    // Get to the goal
    mWheelFrontLeft.spin(fwd, 40, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 40, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    vexDelay(300);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Ensure we are at the goal
    driveForward(100, 1000);

    // Outtake the preload
    output(100, 400); // 500 > 300 timems

    // Drive in reverse to make sure we aren't touching anything in the goal
    driveForward(-20, 500);
  }

  // Left 1
  if (mode == '<') {
    // Get to the goal
    // mWheelFrontLeft.spin(fwd, 0, pct);
    // mWheelFrontRight.spin(fwd, -40, pct);
    // mWheelBackLeft.spin(fwd, 0, pct);
    // mWheelBackRight.spin(fwd, -40, pct);
    // vexDelay(300);
    // mWheelFrontLeft.spin(fwd, 0, pct);
    // mWheelFrontRight.spin(fwd, 0, pct);
    // mWheelBackLeft.spin(fwd, 0, pct);
    // mWheelBackRight.spin(fwd, 0, pct);

    // // Ensure we are in the goal
    // driveForward(100, 1000);

    // // Outtake the preload
    // output(100, 400); //500 > 300 > 400timems

    // // Drive in reverse
    // driveForward(-20, 500);

    // Deploy
    mOutputUpper.spin(fwd, 100, pct);
    wait(300, msec);
    mOutputUpper.spin(fwd, 0, pct);

    intakeIn();

    // Wait to fully deplay
    wait(750, msec);

    // Get into the goal
    mWheelFrontLeft.spin(fwd, 20, pct);
    mWheelFrontRight.spin(fwd, -40, pct);
    mWheelBackLeft.spin(fwd, 20, pct);
    mWheelBackRight.spin(fwd, -40, pct);
    vexDelay(500);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Score preload, score the red ball off the ground, and descore the blues
    output(100, 2000);

    // Fully deploy
    driveForward(-75, 500);

    intakeIn();
    wait(500, msec);

    driveForward(100, 1000);

    output(100, 2000);
    intakeOff();
    intakeOpen();
    wait(100, msec);
    intakeIn();
    prepOutput(100, 400);
    driveForward(-100, 500);
    intakeOff();
  }

  // Right 2
  if (mode == 'X') {
    // Get to the goal
    mWheelFrontLeft.spin(fwd, 40, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 40, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    vexDelay(300);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    // Outtake the preload
    output(100, 400); // 500 > 300 timems
    // Drive in reverse
    driveForward(-20, 200);
    // Turn 45 deg ccw
    sInertial.resetRotation();
    while (sInertial.rotation(deg) < 40) {
      mWheelBackRight.spin(fwd, 40, pct);
      mWheelFrontRight.spin(fwd, 40, pct);
    }
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelFrontLeft.spin(fwd, 0, pct);

    intakeIn();

    // Line track until we reach the perpendicular line
    while (sOpticalFront.getRgb().clear < OPTICAL_THRESHOLD) {
      // Calculate new moving average for the front optical
      // Assign movement variables
      leftX = 25;
      leftY;
      rightX = 0;
      if (sOpticalBack.getRgb().clear > OPTICAL_THRESHOLD)
        leftY = 30;
      else
        leftY = -10;
      // Assign wheel speeds
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Drive to the left a bit
    leftX = -25;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    vexDelay(300); // 500
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    intakeOpen();
    vexDelay(500);

    // Thrust into the goal
    driveForward(100, 1000);

    // Turn intake off

    // Spit out the ball we have
    output(100, 600); // 500 > 300 timems

    mWheelFrontLeft.spin(fwd, -50, pct);
    mWheelFrontRight.spin(fwd, 50, pct);
    mWheelBackLeft.spin(fwd, -50, pct);
    mWheelBackRight.spin(fwd, 50, pct);

    vexDelay(1000);

    intakeOff();

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
  }

  // Left 2
  if (mode == '^') {
    // Get to the goal
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, -40, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, -40, pct);
    vexDelay(300);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    // Outtake the preload
    output(100, 400); // 500 > 300 timems
    // Drive in reverse
    driveForward(-20, 200);
    // Turn 45 deg ccw
    sInertial.resetRotation();
    while (sInertial.rotation(deg) > -40) {
      mWheelBackLeft.spin(fwd, -40, pct);
      mWheelFrontLeft.spin(fwd, -40, pct);
    }
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelFrontLeft.spin(fwd, 0, pct);

    intakeIn();

    // Line track until we reach the perpendicular line
    while (sOpticalFront.getRgb().clear < OPTICAL_THRESHOLD) {
      // Calculate new moving average for the front optical
      // Assign movement variables
      leftX = -25;
      leftY;
      rightX = 0;
      if (sOpticalBack.getRgb().clear > OPTICAL_THRESHOLD)
        leftY = 30;
      else
        leftY = -10;
      // Assign wheel speeds
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Drive to the left a bit
    leftX = -25;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    vexDelay(300); // 500
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    intakeOpen();
    vexDelay(500);

    // Thrust into the goal
    driveForward(100, 1000);

    // Turn intake off

    // Spit out the ball we have
    output(100, 600); // 500 > 300 timems

    mWheelFrontLeft.spin(fwd, -50, pct);
    mWheelFrontRight.spin(fwd, 50, pct);
    mWheelBackLeft.spin(fwd, -50, pct);
    mWheelBackRight.spin(fwd, 50, pct);

    vexDelay(1000);

    intakeOff();

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
  }

  // Right 3
  if (mode == 'A') {
    // Get to the goal
    mWheelFrontLeft.spin(fwd, 40, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 40, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    vexDelay(300);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    // Outtake the preload
    output(100, 400); // 500 > 300 timems
    // Drive in reverse
    driveForward(-20, 200);
    // Turn 45 deg ccw
    sInertial.resetRotation();
    while (sInertial.rotation(deg) < 40) {
      mWheelBackRight.spin(fwd, 40, pct);
      mWheelFrontRight.spin(fwd, 40, pct);
    }
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelFrontLeft.spin(fwd, 0, pct);

    intakeIn();

    // Line track until we reach the perpendicular line
    while (sOpticalFront.getRgb().clear < OPTICAL_THRESHOLD) {
      // Calculate new moving average for the front optical
      // Assign movement variables
      leftX = 25;
      leftY;
      rightX = 0;
      if (sOpticalBack.getRgb().clear > OPTICAL_THRESHOLD)
        leftY = 30;
      else
        leftY = -10;
      // Assign wheel speeds
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Drive to the left a bit
    leftX = -25;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    vexDelay(300); // 500
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Thrust into the goal
    driveForward(100, 1000);

    // Turn intake off

    // Spit out the ball we have
    output(100, 600); // 500 > 300 timems

    mWheelFrontLeft.spin(fwd, -50, pct);
    mWheelFrontRight.spin(fwd, 50, pct);
    mWheelBackLeft.spin(fwd, -50, pct);
    mWheelBackRight.spin(fwd, 50, pct);

    vexDelay(1000);

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Line track until we reach the perpendicular line
    leftX = 100;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);

    waitUntil(sDistanceRight.objectDistance(distanceUnits::in) < 4);

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    driveForward(100, 1000);

    output(100, 1000);

    intakeOpen();

    driveForward(-100, 500);

    intakeOff();
  }

  // Left 3
  if (mode == '>') {
    // Get to the goal
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, -40, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, -40, pct);
    vexDelay(300);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Outtake the preload
    output(100, 400); // 500 > 300 timems

    // Drive in reverse
    driveForward(-20, 200);

    // Turn 45 deg ccw
    sInertial.resetRotation();
    while (sInertial.rotation(deg) > -40) {
      mWheelBackLeft.spin(fwd, -40, pct);
      mWheelFrontLeft.spin(fwd, -40, pct);
    }
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelFrontLeft.spin(fwd, 0, pct);

    // Deploy the intakes
    intakeIn();

    // Line track until we reach the perpendicular line
    while (sOpticalFront.getRgb().clear < OPTICAL_THRESHOLD) {
      // Calculate new moving average for the front optical
      // Assign movement variables
      leftX = -25;
      leftY;
      rightX = 0;
      if (sOpticalBack.getRgb().clear > OPTICAL_THRESHOLD)
        leftY = 30;
      else
        leftY = -10;
      // Assign wheel speeds
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Open the intakes
    intakeOpen();

    // Drive to the left a bit to align with the goal
    leftX = -25;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    vexDelay(300); // 500
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Thrust into the goal
    driveForward(100, 1000);

    // Spit out the ball we have
    output(100, 600); // 500 > 300 timems

    // Back up
    mWheelFrontLeft.spin(fwd, -50, pct);
    mWheelFrontRight.spin(fwd, 50, pct);
    mWheelBackLeft.spin(fwd, -50, pct);
    mWheelBackRight.spin(fwd, 50, pct);

    vexDelay(1000);

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Line track until we reach the wall
    leftX = -100;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);

    waitUntil(sDistanceLeft.objectDistance(distanceUnits::in) < 4);

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Open the intakes
    intakeOpen();

    // Gun it into the goal
    driveForward(100, 1000);

    // Spit the ball out
    output(100, 1000);

    // Get away from the goal
    driveForward(-100, 500);

    // Relax the intakes
    intakeOff();
  }

  // Online tournament 15 seconds
  if (mode == 'S') {
    // Get to the goal
    sInertial.setRotation(-107, deg);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, -40, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, -40, pct);
    vexDelay(500);
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);
    // Score the preload
    output(100, 500); // 500 > 300 timems
    // Drive in reverse
    driveForward(-20, 200);
    // Turn 45 deg ccw
    while (sInertial.rotation(deg) > -175) {
      mWheelFrontLeft.spin(fwd, -20, pct);
      mWheelBackLeft.spin(fwd, -20, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    intakeIn();

    // Line track until we reach the perpendicular line
    while (sOpticalFront.getRgb().clear < OPTICAL_THRESHOLD) {
      // Calculate new moving average for the front optical
      // Assign movement variables
      leftX = -25;
      leftY;
      rightX = 0;
      if (sOpticalBack.getRgb().clear > OPTICAL_THRESHOLD)
        leftY = 30;
      else
        leftY = -10;
      // Assign wheel speeds
      mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
      mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
      mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
      mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    intakeOpen();

    // Drive to the left a bit
    leftX = -25;
    leftY = 0;
    rightX = 0;
    mWheelFrontLeft.spin(fwd, rightX + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, rightX - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, rightX + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, rightX - leftY - leftX, pct);
    vexDelay(200); // 500 > 300
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    // Thrust into the goal
    driveForward(100, 1000);

    // Spit out the ball we have
    output(100, 600); // 500 > 300 timems

    // Reverse into the center goal

    double startTime = Brain.timer(msec);
    while (Brain.timer(msec) - startTime < 2000) {
      int ySpeed;
      int rotSpeed;
      ySpeed = -75;
      rotSpeed = -(sInertial.rotation() + 180) * 2;
      mWheelFrontLeft.spin(fwd, rotSpeed + ySpeed, pct);
      mWheelFrontRight.spin(fwd, rotSpeed - ySpeed, pct);
      mWheelBackLeft.spin(fwd, rotSpeed + ySpeed, pct);
      mWheelBackRight.spin(fwd, rotSpeed - ySpeed, pct);
    }
    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    intakeOff();

    // Turn towards
    mWheelFrontRight.spin(fwd, -50, pct);
    mWheelBackRight.spin(fwd, -50, pct);
    waitUntil(sInertial.rotation() > -225);

    // Gun it towards the wall
    mWheelFrontLeft.spin(fwd, -100, pct);
    mWheelFrontRight.spin(fwd, -100, pct);
    mWheelBackLeft.spin(fwd, 100, pct);
    mWheelBackRight.spin(fwd, 100, pct);

    intakeOpen();

    vexDelay(2000);

    mWheelFrontLeft.spin(fwd, 0, pct);
    mWheelFrontRight.spin(fwd, 0, pct);
    mWheelBackLeft.spin(fwd, 0, pct);
    mWheelBackRight.spin(fwd, 0, pct);

    output(100, 1000);

    intakeOpen();

    driveForward(-100, 500);

    intakeOff();
  }
}

void usercontrol(void) {

  // Set everything into motion
  mWheelFrontLeft.spin(fwd);
  mWheelFrontRight.spin(fwd);
  mWheelBackLeft.spin(fwd);
  mWheelBackRight.spin(fwd);
  mIntakeLeft.spin(fwd);
  mIntakeRight.spin(fwd);
  mOutputLower.spin(fwd);
  mOutputUpper.spin(fwd);

  mIntakeLeft.setStopping(hold);
  mIntakeRight.setStopping(hold);

  // Variables local to usercontrol
  int intakePhase = 0;
  int leftX;
  int leftY;
  int rightX;

  while (true) {

    // Drivetrain
    // /////////////////////////////////////////////////////////////////////////

    // Controller joystick values
    leftX = Controller1.Axis4.position();
    leftY = Controller1.Axis3.position();
    rightX = Controller1.Axis1.position();

    // // Moving average replacement for the code above
    // leftX = (leftX * 0.9) + (Controller1.Axis4.position() * 0.1);
    // leftY = (leftY * 0.9) + (Controller1.Axis3.position() * 0.1);
    // rightX = (rightX * 0.9) + (Controller1.Axis1.position() * 0.1);


    // // Zero the values (Likely not needed if we use the cubing function)
    // if (leftX < 20 && leftX > -20)
    //   leftX = 0;
    // if (leftY < 20 && leftY > -20)
    //   leftY = 0;
    // if (rightX < 20 && rightX > -20)
    //   rightX = 0;

    // Bend values from a linear to a cubic function
    leftX /= 100; // Shift value to below one
    leftX = leftX * leftX * leftX; // Cube the value
    leftX *= 100; // Shift back to 100

    leftY /= 100;
    leftY = leftY * leftY * leftY;
    leftY *= 100;

    rightX /= 100;
    rightX = rightX * rightX * rightX;
    rightX *= 100;

    // // Perfect extreme code
    // if(leftX > 70)
    //   leftX = 100;
    // if(leftX < -70)
    //   leftX = -100;
    
    // if(leftY > 70)
    //   leftY = 100;
    // if(leftY < -70)
    //   leftY = -100;

    // if(rightX > 70)
    //   rightX = 100;
    // if(rightX < -70)
    //   rightX = -100;

    // Assign wheel speeds
    mWheelFrontLeft.spin(fwd, (rightX * 1.5) + leftY + leftX, pct);
    mWheelFrontRight.spin(fwd, (rightX * 1.5) - leftY + leftX, pct);
    mWheelBackLeft.spin(fwd, (rightX * 1.5) + leftY - leftX, pct);
    mWheelBackRight.spin(fwd, (rightX * 1.5) - leftY - leftX, pct);

    // Intake
    // ////////////////////////////////////////////////////////////////////////////////////////////////////

    if (!disableIntakes) {
      // ButtonR2 > Begin opening the intakes
      if (Controller1.ButtonR2.pressing() && intakePhase == 0) {
        intakePhase = 1;
      }

      // ButtonR1 > Spin intakes inward
      if (Controller1.ButtonR1.pressing()) {
        intakePhase = 0;
        mIntakeLeft.spin(fwd, -100, pct);
        mIntakeRight.spin(fwd, -100, pct);
        mIntakeLeft.setMaxTorque(100, pct);
        mIntakeRight.setMaxTorque(100, pct);
        mIntakeLeft.setStopping(coast);
        mIntakeRight.setStopping(coast);
      }

      // If absolutly nothing is happening, stop
      if (!Controller1.ButtonR1.pressing() &&
          !Controller1.ButtonR2.pressing() && intakePhase == 0) {
        mIntakeLeft.spin(fwd, 0, pct);
        mIntakeRight.spin(fwd, 0, pct);
      }

      // Phase 1: Spin to the hardstops
      if (intakePhase == 1) {
        mIntakeLeft.spin(fwd, 100, pct);
        mIntakeRight.spin(fwd, 100, pct);
      }

      // Both arms hit > Phase 2
      if (mIntakeLeft.torque(Nm) > TORQUE_THRESHOLD &&
          mIntakeRight.torque() > TORQUE_THRESHOLD && intakePhase == 1) {
        intakePhase = 2;
      }

      // Phase 2: Continue to press against the hard stops, but relax the torque a bit
      if (intakePhase == 2) {
        mIntakeLeft.spin(fwd, 100, pct);
        mIntakeRight.spin(fwd, 100, pct);
        mIntakeLeft.setMaxTorque(40, pct);
        mIntakeRight.setMaxTorque(40, pct);
      }
    }

    // Output
    // ///////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Spin in
    if (Controller1.ButtonL1.pressing()) {
      mOutputLower.spin(fwd, 100, pct);
      mOutputUpper.spin(fwd, 100, pct);
      // Spin out
    } else if (Controller1.ButtonL2.pressing()) {
      mOutputLower.spin(fwd, -100, pct);
      mOutputUpper.spin(fwd, -100, pct);
      // Stop
    } else {
      mOutputLower.spin(fwd, 0, pct);
      mOutputUpper.spin(fwd, 0, pct);
    }

    vexDelay(5);
  }
}

int computeMotorParameters() {
  wait(100, msec);
  // Local variables
  // float FrontLeftDistance;
  // float FrontRightDistance;
  // float BackLeftDistance;
  // float BackRightDistance;
  float FrontLeftVelocity;
  float FrontRightVelocity;
  float BackLeftVelocity;
  float BackRightVelocity;
while (1 > 0) {
    // Compute distance each wheel has traveled
    // May not need this code in the multitask (instead put the code in the
    // method that uses it)
    float FrontLeftDistance = mWheelFrontLeft.rotation(rotationUnits::raw);
    float FrontRightDistance = mWheelFrontRight.rotation(rotationUnits::raw);
    float BackLeftDistance = mWheelBackLeft.rotation(rotationUnits::raw);
    float BackRightDistance = mWheelBackRight.rotation(rotationUnits::raw);
    ForwardDistance = FrontLeftDistance - FrontRightDistance + BackLeftDistance - BackRightDistance;
    TurnDistance =    FrontLeftDistance + FrontRightDistance + BackLeftDistance + BackRightDistance;
    StrafeDistance =  FrontLeftDistance - FrontRightDistance - BackLeftDistance - BackRightDistance;

    // Compute velocity of each wheel
    // This code is useful for determining if Robot has hit the goal and stopped
    // moving Also useful to calculate the turn velocity to detect the end of a
    // gyro turn
    FrontLeftVelocity = mWheelFrontLeft.velocity(pct);
    FrontRightVelocity = mWheelFrontRight.velocity(pct);
    BackLeftVelocity = mWheelBackLeft.velocity(pct);
    BackRightVelocity = mWheelBackRight.velocity(pct);
    ForwardVelocity = BackLeftVelocity - BackRightVelocity;
    TurnVelocity = FrontLeftVelocity + FrontRightVelocity + BackLeftVelocity + BackRightVelocity;
    StrafeVelocity = FrontLeftVelocity + FrontRightVelocity - BackLeftVelocity - BackRightVelocity;
    
    wait(5, msec);
  }
} // end of computerMotorParameters

int printCameraObjects() {
  wait(100, msec);
  while (1 > 0) {
    Controller1.Screen.clearScreen();

    Controller1.Screen.setCursor(1, 1);
    Controller1.Screen.print("GY"); // print Gyro Angle
    Controller1.Screen.setCursor(1, 5);
    Controller1.Screen.print(sInertial.rotation(deg));

    Controller1.Screen.setCursor(1, 11);
    Controller1.Screen.print("FV"); // Print Forward Velocity
    Controller1.Screen.setCursor(1, 15);
    Controller1.Screen.print(ForwardVelocity);

    Controller1.Screen.setCursor(2, 1);
    Controller1.Screen.print(" "); // Empty
    Controller1.Screen.setCursor(2, 5);
    Controller1.Screen.print(" ");

    Controller1.Screen.setCursor(2, 11);
    Controller1.Screen.print("TV"); // Print Turn Velocity
    Controller1.Screen.setCursor(2, 15);
    Controller1.Screen.print(TurnVelocity);

    Controller1.Screen.setCursor(3, 1);
    Controller1.Screen.print("VUX "); // Vision Upper X-Axis
    Controller1.Screen.setCursor(3, 5);
    Controller1.Screen.print(sVisionUpper.largestObject.centerX - 180);

    Controller1.Screen.setCursor(3, 11);
    Controller1.Screen.print("VLX"); // Vision Lower X-Axis
    Controller1.Screen.setCursor(3, 15);
    Controller1.Screen.print(sVisionLower.largestObject.centerX - 180);

    wait(250, msec);
  }
} // end of printCameraObjects

int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  task taskPrintCameraObjects(printCameraObjects);
  task taskComputeMotorParameters(computeMotorParameters);
  
  Competition.test_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
