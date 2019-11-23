// 
// If you get errors during upload like:
//   "avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0xf8"
// In the IDE menubar go to Tools > Processor > "ATMega328P (Old Bootloader)"
//
// If the output of the terminal monitor is all garbled, set it to 57600 baud.
//

// define which arduino pins are used for what
#define MODE_IN              2 // D2
#define MODE_LED             3 // D3

#define PROGRAM_IN           5 // D5
#define PROGRAM_LED          6 // D6
#define PROGRAM_OUT_LEFT     7 // D7
#define PROGRAM_OUT_CENTER  12 // D12

#define CLOCK_IN             8 // D8
#define CLOCK_LED            9 // D9
#define CLOCK_OUT_LEFT      10 // D10
#define CLOCK_OUT_CENTER    13 // D13

#define RESET_BTN_IN        11 // D11

#define RESET_BTN_OUT        4 // D4


// Need a halt read in line.

/* Arduino runs this function once after loading the Nano, or after pressing the HW reset button.
 * Think of this like main() */
void setup() {
  Serial.begin(57600);

  pinMode(MODE_IN, INPUT);
  pinMode(PROGRAM_IN, INPUT);
  pinMode(CLOCK_IN, INPUT);

  pinMode(MODE_LED, OUTPUT);
  pinMode(PROGRAM_LED, OUTPUT);
  pinMode(CLOCK_LED, OUTPUT);

  pinMode(RESET_BTN_IN, INPUT);
  pinMode(RESET_BTN_OUT, OUTPUT);
}

// return the value of the "mode" switch. true = auto, false = manual
bool IsModeSetToAuto() {
  return HIGH == digitalRead(MODE_IN);
}

// return the value of the "program" switch, true = run, false = load
bool IsProgramSetToRun() {
  return HIGH == digitalRead(PROGRAM_IN);
}

// return the value of the "clock" switch, true = auto, false = manual
bool IsClockSetToAuto() {
  return HIGH == digitalRead(CLOCK_IN);
}

// Set the outgoing program line appropriately
void SetProgram(bool isRun) {
  if (isRun) {
    Serial.print("PROGRAM is  run. "); 
  } else {
    Serial.print("PROGRAM is load. ");
  } 

  // TODO: check logic here.
  // digitalWrite(PROGRAM_OUT_LEFT/RIGHT, set to read/write mode, etc., isRun);
  // When button is up, left is connected to right, so set LEFT to 0V, set center to an input/let it float.
  // When button is down, center is connected to right, so set CENTER to 0V, set left to an input/let it float.
}

// Set the outgoing clock line appropriately.
void SetClock(bool isAuto) {
  if(isAuto) {
    Serial.println("CLOCK is auto. ");
  } else Serial.println("CLOCK is  man. ");


  // TODO: IMPLEMENT
  // We need to two clock outputs, the LEFT AND CENTER OUTPUT.
  
  // when button is up, left is connected to right, so set LEFT to 0V. change center to an input/dangle.
  
  // When button is pressed (aka auto clock mode) Left pin is 4.xV, center pin is 0V.
    // when button is depressed, it's connecting center to right (aka GND). so cet CENTER to 0V, set LEFT to an input/dangle.

  // digitalWrite(CLOCK_OUT, isAuto);
}

void SetLEDs() {
  bool isAutoMode = IsModeSetToAuto();
  digitalWrite(MODE_LED, isAutoMode);
  // only light other LEDs if we are not in auto mode!
  digitalWrite(PROGRAM_LED, !isAutoMode && IsProgramSetToRun());
  digitalWrite(CLOCK_LED, !isAutoMode && IsClockSetToAuto());
}


void loop() {
  SetLEDs();

  bool isAutoMode = IsModeSetToAuto();
  if(isAutoMode) {
    Serial.println("In Auto mode");
    // TODO: write the auto mode, basically, need to:
    // Clock == Off/Manual
    // Program into load mode
    // Reset
    // Start clock, wait until read a HALT
    // Clock Off/Manual
    // Program into run mode
    // Master Reset
    // Clock Auto
  } else {
    Serial.print("Manual mode. ");
    SetProgram(IsProgramSetToRun());
    SetClock(IsClockSetToAuto());
  }

  if(digitalRead(RESET_BTN_IN)) {
    Serial.println("  RESET BTN!");
    // TODO: send a rest signal to the computer on reset OUT. line (add tha line)
    // set RESET_OUT to be 5V when resest is pressed.
  } else {
  }
  
  delay(100); // msec TODO: delete me
}
