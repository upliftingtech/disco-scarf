// Version 0.1 Putting together the animationData struck and some stubs for functions.
//             Stores a function pointer in the struct.
//         0.2 Lets switch to an integer to identify the animation function in the struct since we can't
//             communicate the function pointer across the radio.
//         0.3 Moving some of the code from main loop to chaseAnimation function
//         0.4 Add in switch statement to pick appropriate animation
//         0.5 Add more animations
//         0.6 Verified to compile with FastLED 3.4.0. Runs. Yay.
//         0.7 Animations fixed back to pte git-hub import
//         0.8 Better comments

#include "FastLED.h"

// FAST LED MACROS

	// The length of the led strip can be set here:
	#define NUM_LEDS 113
	
	// WS2812 strip leds have ony one data pin that doubles as a clock pin.
	// Thus no need to define the clock pin.
	#define DATA_PIN 6
	// #define CLOCK_PIN 13

// MACROS

	// Control the absolute brightness of the strip
	#define FULL_ON 150
	
	// The number of animations to create memory for and loop on
	#define MAX_NUM_ANIMATIONS 20 // 0 - 255
	
	// Drop into the animate function this often
	#define ANIMATE_INTERVAL 10 // milliseconds
	
	// Start a chase animation this often
	#define START_CHASE_INTERVAL 1000 // miliseconds

// GLOBALS
unsigned short blueBreathInterval = 1;
unsigned short blueSweepInterval = 1;

// define the data structure to hold information about one animation
// I'm guessing this would be the place to start making objects if I go to object code
struct animationData 
{ 
  unsigned long startTime;
  unsigned short duration; //0 to 65,535 miliseconds
  unsigned short numFrames; //0 to 65,535
  boolean isRunning;
  uint8_t animationID; // 0 to 255
};

// define an array of animations 
animationData animation[MAX_NUM_ANIMATIONS];

CRGB leds[NUM_LEDS]; // Define the array of leds

// these are all just timers. simplify?
unsigned long currentAnimateTime = 0;
unsigned long previousAnimateTime = 0;
unsigned long currentStartChaseTime = 0;
unsigned long previousStartChaseTime = 0;
unsigned long currentStartBreathTime = 0;
unsigned long previousStartBreathTime = 0;
unsigned long currentStartSweepTime = 0;
unsigned long previousStartSweepTime = 0;

// FUNCTIONS

void chaseAnimation(int frame)
{
  int i;
  //for every led
  for (i=0;i<NUM_LEDS;i++)
  {    
    if (i == frame) leds[i] += CHSV(0, 0, FULL_ON);
    else if (i == frame -1) leds[i] += CHSV(0, 0, FULL_ON / 1.25);
    else if (i == frame -2) leds[i] += CHSV(0, 0, FULL_ON / 1.5);
    else if (i == frame -3) leds[i] += CHSV(0, 0, FULL_ON / 2);
    else if (i == frame -4) leds[i] += CHSV(0, 0, FULL_ON / 3);
  }
}

void blueBreathAnimation(int frame)
{
  int i;
  float y;
  
  // y = sin (x/100) for x of 0 to 314 y is 0 to 1 and back to 0
  y = frame / 100.0;
  y = sin(y);
  y = floor(y * FULL_ON);
  
  //for every led
  for (i=0;i<NUM_LEDS;i++)
  {
    leds[i] += CHSV(160,255,(int)y); //hue 160 is blue
  }
}

void blueSweepAnimation(int frame)
{
  int i;
  
  //for every led
  for (i=0;i<NUM_LEDS;i++)
  {
    leds[i] += CHSV(160, 255, floor(abs(sin((i/30.0) + (frame/100.0)) * FULL_ON / 1.25))); //hue 160 is blue
  }
}


void initializeAnimationData(uint8_t animationNumber)
{
  animation[animationNumber].startTime = 0;
  animation[animationNumber].duration = 0;
  animation[animationNumber].numFrames = 0;
  animation[animationNumber].isRunning = false;
  animation[animationNumber].animationID = 0;
}

int findEmptyAnimatonDataSlot()
{
    int n;
    //find a new slot to put the data in
    for (n=0;n<MAX_NUM_ANIMATIONS;n++)
    {
      if (animation[n].isRunning == false)
      {
        return n;
      }
    }
    //error didn't find empty slot
    return -1;
}

void setup() { 
  uint8_t i; // 0 - 255
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
       
  for (i=0 ; i < MAX_NUM_ANIMATIONS ; i++)
  {
    initializeAnimationData(i);
  }
}

void loop() { 
  currentAnimateTime = millis();
  if (currentAnimateTime - previousAnimateTime > ANIMATE_INTERVAL)
  {
	previousAnimateTime = currentAnimateTime;
  	  
    int i;
    int n;
    //zero strip
    for (i=0;i<NUM_LEDS;i++)
    {
      leds[i] = CRGB::Black;
    }
    
    //for every animation
    for (n=0;n<MAX_NUM_ANIMATIONS;n++)
    {
      //only calculate the LEDs if the animation is actually running
      if (animation[n].isRunning)
      {
        //check to see if the animation has exceeded its duration
        if ((currentAnimateTime - animation[n].startTime) < animation[n].duration)
        {
          // animations should calc one frame adding each led's data to the data already there. strip is set to black earlier
          switch(animation[n].animationID)
          {
            case 1 :
            chaseAnimation(floor(((currentAnimateTime - animation[n].startTime) / (float)animation[n].duration) * animation[n].numFrames)); //pass frame number
            break;
            case 2 :
            blueBreathAnimation(floor(((currentAnimateTime - animation[n].startTime) / (float)animation[n].duration) * animation[n].numFrames)); //pass frame number
            break;
            case 3 :
            blueSweepAnimation(floor(((currentAnimateTime - animation[n].startTime) / (float)animation[n].duration) * animation[n].numFrames)); //pass frame number
            break;

          }  
        }
        else // animation has exceeded it runtime
        {
          //stop the animation
          initializeAnimationData(n);
        }
      }
    } //end of for every animation loop
         
    //display strip
    FastLED.show();

  }
  
 
  currentStartChaseTime = millis();
  if (currentStartChaseTime - previousStartChaseTime > START_CHASE_INTERVAL)
  {
    int n;
    //find a new slot to put the data in
    n = findEmptyAnimatonDataSlot();
    if ( n != -1) // if n == -1 than a slot wasn't found
    {
    //start the chase animation
    
    animation[n].startTime = currentStartChaseTime;
    animation[n].duration = random(1000,7000); // miliseconds
    animation[n].numFrames = NUM_LEDS;
    animation[n].isRunning = true;
    animation[n].animationID = 1;
    } 
    previousStartChaseTime = currentStartChaseTime;
  } 

/* comment out blue breath for now
  currentStartBreathTime = millis();
  if (currentStartBreathTime - previousStartBreathTime > blueBreathInterval)
  {
    int n;
    //find a new slot to put the data in
    n = findEmptyAnimatonDataSlot();
    if ( n != -1) // if n == -1 than a slot wasn't found
    {
    //start the breath animation
    blueBreathInterval = random(4000,5000); // Average adult breath rate is around 1 breath every 3 to 4 seconds.
                                            // But I want it to be super soothing so lengthen a bit.
    
    animation[n].startTime = currentStartBreathTime;
    animation[n].duration = blueBreathInterval; 
    animation[n].numFrames = 314; // it takes 314 frames for the sin function in this animation to cycle around
    animation[n].isRunning = true;
    animation[n].animationID = 2; //2 = starts blue breath
    } 
    previousStartBreathTime = currentStartBreathTime;
  } 
  End blue breath comment out*/
  
  currentStartSweepTime = millis();
  if (currentStartSweepTime - previousStartSweepTime > blueSweepInterval)
  {
	  int n;
	  //find a new slot to put the data in
	  n = findEmptyAnimatonDataSlot();
	  if ( n != -1) // if n == -1 than a slot wasn't found
	  {
		  //start the blue sweep animation
		  blueSweepInterval = random(4000,5000); // Average adult breath rate is around 1 breath every 3 to 4 seconds.
		  // But I want it to be super soothing so lengthen a bit.
		  
		  animation[n].startTime = currentStartSweepTime;
		  animation[n].duration = blueSweepInterval; 
		  animation[n].numFrames = 628; // it takes 628?? frames for the sin function in this animation to cycle around
		  animation[n].isRunning = true;
		  animation[n].animationID = 3; //3 = starts blue sweep
	  } 
	  previousStartSweepTime = currentStartSweepTime;
  } 

    
} // end void loop()
