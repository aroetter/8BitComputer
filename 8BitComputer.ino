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
  pinMode(CLOCK_OUT_LEFT, OUTPUT);

  // TODO fix PROGRAM_OUT_LEFT/CENTER
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

// TODO confirm if both these helper methods work!!!!
// Set the outgoing program line appropriately
void SetProgram(bool isRun) {
  if (isRun) {
    Serial.print("PROGRAM is  run. "); 
  } else {
    Serial.print("PROGRAM is load. ");
  } 

  // TODO test this!
  if (isRun) {
    // Up on the main board, pressing the old LOAD button puts us in run mode.
    // In that case, center pin is tied to ground, left floats.
    pinMode(PROGRAM_OUT_CENTER, OUTPUT);
    digitalWrite(PROGRAM_OUT_CENTER, LOW);
    pinMode(PROGRAM_OUT_LEFT, INPUT);
  } else {
    // Up on the main board, releasing the old LOAD button puts us in load mode.
    // In that case, left pin is tied to ground, center floats.
    pinMode(PROGRAM_OUT_LEFT, OUTPUT);
    digitalWrite(PROGRAM_OUT_LEFT, LOW);
    pinMode(PROGRAM_OUT_CENTER, INPUT);
    
  }
}

// Set the outgoing clock line appropriately.
void SetClock(bool isAuto) {
  if(isAuto) {
    Serial.println("CLOCK is auto. ");
  } else Serial.println("CLOCK is  man. ");

  // TODO test this!
  if (isAuto) {
    // Up on the main board, pressing the old clock button puts us in auto clock.
    // In that case, center pin is tied to GND. left floats, is tied to 5V through a resistor.
    // When button is pressed (aka auto clock mode) Left pin is 4.xV, center pin is 0V.
    digitalWrite(CLOCK_OUT_LEFT, HIGH);
    
  } else {
    // TODO fix comments, rename
    // Up on the main board, releasing the old clock button puts us in manual clock.
    // In that case, left pin is tied to GND. center floats.
    digitalWrite(CLOCK_OUT_LEFT, LOW);    
  }
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
    digitalWrite(RESET_BTN_OUT, HIGH);
  } else {
    digitalWrite(RESET_BTN_OUT, LOW);
  }
  
  delay(100); // msec TODO: delete me
}
