#include "Nintendo.h"

#include "PitchToNote.h"

#define NUM_BUTTONS  12

//USER CONFIGURABLE VALUES
const bool SHOULDER_AS_INTESITY = true; //Change to false if you dont want the note intensity to be controlled by the right shoulder trigger

const uint8_t button1; //A
const uint8_t button2; //B
const uint8_t button3; //X
const uint8_t button4; //Y
const uint8_t button5; //Start
const uint8_t button6; //D-pad up
const uint8_t button7; //D-pad down
const uint8_t button8; //D-pad left
const uint8_t button9; //D-pad right
const uint8_t button10; //Z
const uint8_t button11; //L
const uint8_t button12; //R
//                                     A   B   X   Y   S   Dup Ddo Dl  Dr  Z   L   R
const byte notePitches[NUM_BUTTONS] = {C3, D3, E3, F3, G3, A3, B3, C4, D4, E4, F4, G4}; //Pitches sent by each button

const uint8_t control0 = 0; //xAxis mapped to control surface0 (change the 0 after the = to change the surface number) 
const uint8_t control1 = 1; //yAxis mapped to control surface1 (change the 1 after the = to change the surface number) 
const uint8_t control2 = 2; //cxAxis mapped to control surface2 (change the 2 after the = to change the surface number) 
const uint8_t control3 = 3; //cyAxis mapped to control surface3 (change the 3 after the = to change the surface number) 
const uint8_t control4 = 4; //right slider mapped to control surface4 (change the 4 after the = to change the surface number) 
const uint8_t control5 = 5; //left slider mapped to control surface5 (change the 5 after the = to change the surface number) 



//DONT CHANGE THESE UNLESS YOU KNOW WHAT YOU'RE DOING. THE CODE WILL STOP WORKING IF YOU DO IT WRONG
// Define a Gamecube Controller with data line connected to D2 on arduino
CGamecubeController GamecubeController(6);

const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12};

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store

unsigned long previousMillis = 0;        // will store last time controller report was printed
const long interval = 1000;           // interval at which to print controller report (milliseconds)
uint8_t notesTime[NUM_BUTTONS];

uint32_t pressedButtons = 0x00;

uint32_t previousButtons = 0x00;

uint8_t intensity;
uint8_t val_right;
uint8_t val_left;
uint8_t val_x;
uint8_t val_y;
uint8_t val_cx;
uint8_t val_cy;


void setup()
{
    Serial.begin(115200);
}

void loop()
{
  // Try to read the controller data
  if (GamecubeController.read())
  {
    // Print Controller information
    auto status = GamecubeController.getStatus();
    auto report = GamecubeController.getReport();
    unsigned long currentMillis = millis();

    //Read the controller report for the digital buttons and translate them into MIDI notes
    readButtons(report);

    //Read the controller report for the analog inputs and translate it into a MIDI control surface
    readIntensity(report);

    //Send the notes over USB


    //If it has been > interval milliseconds since the controller report was printed to serial, then do so  
    if (currentMillis - previousMillis >= interval) {
      // save the last time report was printed
      previousMillis = currentMillis;
      print_gc_report(report, status);
    }
    
  }
  else
  {
    // Add debounce if reading failed
    Serial.println(F("Error reading Gamecube controller."));
    delay(interval);
  }
  delay(10); // delay a little bit so we dont flood the midi interface 
}


void print_gc_report(Gamecube_Report_t &gc_report, Gamecube_Status_t &gc_status)
{
  // Print device information
  Serial.print(F("Device: "));
  switch (gc_status.device) {
    case NINTENDO_DEVICE_GC_NONE:
      Serial.println(F("No Gamecube Controller found!"));
      break;
    case NINTENDO_DEVICE_GC_WIRED:
      Serial.println(F("Original Nintendo Gamecube Controller"));
      break;

    default:
      Serial.print(F("Unknown "));
      Serial.println(gc_status.device, HEX);
      break;
  }

  // Print rumble state
  Serial.print(F("Rumble "));
  if (gc_status.rumble)
    Serial.println(F("on"));
  else
    Serial.println(F("off"));

  // Prints the raw data from the controller
  Serial.println();
  Serial.println(F("Printing Gamecube controller report:"));
  Serial.print(F("Start:\t"));
  Serial.println(gc_report.start);

  Serial.print(F("Y:\t"));
  Serial.println(gc_report.y);

  Serial.print(F("X:\t"));
  Serial.println(gc_report.x);

  Serial.print(F("B:\t"));
  Serial.println(gc_report.b);

  Serial.print(F("A:\t"));
  Serial.println(gc_report.a);

  Serial.print(F("L:\t"));
  Serial.println(gc_report.l);
  Serial.print(F("R:\t"));
  Serial.println(gc_report.r);
  Serial.print(F("Z:\t"));
  Serial.println(gc_report.z);

  Serial.print(F("Dup:\t"));
  Serial.println(gc_report.dup);
  Serial.print(F("Ddown:\t"));
  Serial.println(gc_report.ddown);
  Serial.print(F("Dright:\t"));
  Serial.println(gc_report.dright);
  Serial.print(F("Dleft:\t"));
  Serial.println(gc_report.dleft);

  Serial.print(F("xAxis:\t"));
  Serial.println(gc_report.xAxis, DEC);
  Serial.print(F("yAxis:\t"));
  Serial.println(gc_report.yAxis, DEC);

  Serial.print(F("cxAxis:\t"));
  Serial.println(gc_report.cxAxis, DEC);
  Serial.print(F("cyAxis:\t"));
  Serial.println(gc_report.cyAxis, DEC);

  Serial.print(F("L:\t"));
  Serial.println(gc_report.left, DEC);
  Serial.print(F("R:\t"));
  Serial.println(gc_report.right, DEC);
  Serial.println();

}

//Reads the gamecube controller report and translates into MIDI notes
void readButtons(Gamecube_Report_t &gc_report)
{
    if (gc_report.a == 1)
    {
      bitWrite(pressedButtons, 0, 1);
    }
    else{
      bitWrite(pressedButtons, 0, 0);
  }
      if (gc_report.b == 1)
    {
      bitWrite(pressedButtons, 1, 1);
    }
    else{
      bitWrite(pressedButtons, 1, 0);
    }
    if (gc_report.x == 1)
    {
      bitWrite(pressedButtons, 2, 1);
    }
    else{
      bitWrite(pressedButtons, 2, 0);
  }
      if (gc_report.y == 1)
    {
      bitWrite(pressedButtons, 3, 1);
    }
    else{
      bitWrite(pressedButtons, 3, 0);
    }
       if (gc_report.start == 1)
    {
      bitWrite(pressedButtons, 4, 1);
    }
    else{
      bitWrite(pressedButtons, 4, 0);
    }
    if (gc_report.dup == 1)
    {
      bitWrite(pressedButtons, 5, 1);
    }
    else{
      bitWrite(pressedButtons, 5, 0);
    }
    if (gc_report.ddown == 1)
    {
      bitWrite(pressedButtons, 6, 1);
    }
    else{
      bitWrite(pressedButtons, 6, 0);
  }
      if (gc_report.dleft == 1)
    {
      bitWrite(pressedButtons, 7, 1);
    }
    else{
      bitWrite(pressedButtons, 7, 0);
    }
    if (gc_report.dright == 1)
    {
      bitWrite(pressedButtons, 8, 1);
    }
    else{
      bitWrite(pressedButtons, 8, 0);
    }
    if (gc_report.z == 1)
    {
      bitWrite(pressedButtons, 9, 1);
    }
    else{
      bitWrite(pressedButtons, 9, 0);
    }
    if (gc_report.l == 1)
    {
      bitWrite(pressedButtons, 10, 1);
    }
    else{
      bitWrite(pressedButtons, 10, 0);
    }
    if (gc_report.r == 1)
    {
      bitWrite(pressedButtons, 11, 1);
    }
    else{
      bitWrite(pressedButtons, 11, 0);
    }



    
}

//Reads gamecube analog values (joysticks + L/R) and translates them  
void readIntensity(Gamecube_Report_t &gc_report)
{
  uint8_t temp;
  temp = map(gc_report.xAxis, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
  if(temp != val_x){ //Checks to see if its different and only updates if it is
     val_x = temp;

  }

  temp = map(gc_report.yAxis, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
    if(temp != val_y){ //Checks to see if its different and only updates if it is
     val_y = temp;

  }

  temp = map(gc_report.cxAxis, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
  if(temp != val_cx){ //Checks to see if its different and only updates if it is
   val_cx = temp;

  }

  temp = map(gc_report.cyAxis, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
  if(temp != val_cy){ //Checks to see if its different and only updates if it is
   val_cy = temp;

  }

  temp = map(gc_report.right, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
  if(temp != val_right){ //Checks to see if its different and only updates if it is
   val_right = temp;

  }

  temp = map(gc_report.left, 0, 256, 0, 127); //takes the value and maps it to a range of 0-127 for MIDI
  if(temp != val_left){ //Checks to see if its different and only updates if it is
   val_left = temp;

  }

  if(SHOULDER_AS_INTESITY == true){
    intensity = 126 - val_right; //Sets the intensity for the digital notes to the right shoulder trigger
  }
  else{
    intensity = 67;
  }
  
  
}
