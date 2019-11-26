// 
// If you get errors during upload like:
//   "avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0xf8"
// In the IDE menubar go to Tools > Processor > "ATMega328P (Old Bootloader)"
//
// If the output of the terminal monitor is all garbled, set it to 57600 baud.
//

// TODO: update the documentation with what I've done, including the ripped out HW buttons for
// Clock, and Program, and use of inverter for former.
//

// define which arduino pins are used for what
#define MODE_IN             13 // D13
#define MODE_LED             3 // D3

#define PROGRAM_IN           5 // D5
#define PROGRAM_LED          6 // D6
#define PROGRAM_OUT          7 // D7

#define CLOCK_IN             8 // D8
#define CLOCK_LED            9 // D9
#define CLOCK_OUT           10 // D10

#define RESET_BTN_IN         2 // D2

#define RESET_BTN_OUT        4 // D4

// TODO: Need a halt read in line. // D12?

volatile bool resetPressed = false;

/* Arduino runs this function once after loading the Nano, or after pressing the HW reset button.
 * Think of this like main() */
void setup() {
  resetPressed = false;
  Serial.begin(57600);

  pinMode(MODE_IN, INPUT);
  pinMode(PROGRAM_IN, INPUT);
  pinMode(CLOCK_IN, INPUT);


  pinMode(MODE_LED, OUTPUT);
  pinMode(PROGRAM_LED, OUTPUT);
  pinMode(CLOCK_LED, OUTPUT);

  pinMode(PROGRAM_OUT, OUTPUT);
  pinMode(CLOCK_OUT, OUTPUT);

  pinMode(RESET_BTN_IN, INPUT);
  pinMode(RESET_BTN_OUT, OUTPUT);

  // TODO: move the pins around so D2 is to reset_in
  // attachInterrupt(digitalPinToInterrupt(2), handleResetButtonEvent, CHANGE);
}

void handleResetButtonEvent() {
  digitalWrite(RESET_BTN_OUT, digitalRead(RESET_BTN_IN) ? HIGH : LOW);  
}

// TODO: also handle the mode switch changing, basically set the mode LED and also trigger a reset?

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
  // TODO: maybe move LED sets here as well, so even in mode=auto we see LEDs cycling...
  if (isRun) {
    Serial.print("PROGRAM is  run. "); 
    digitalWrite(PROGRAM_OUT, LOW);

  } else {
    Serial.print("PROGRAM is load. ");
    digitalWrite(PROGRAM_OUT, HIGH);
  } 
}

// Set the outgoing clock line appropriately.
void SetClock(bool isAuto) {
  if(isAuto) {
    Serial.println("CLOCK is auto. ");
    // Up on the main board, pressing the old clock button puts us in auto clock.
    // In that case, center pin is tied to GND. left is tied to 5V through a resistor.
    // When button is pressed (aka auto clock mode) Left pin goes high, center pin is 0V.
    digitalWrite(CLOCK_OUT, HIGH);
  } else {
    Serial.println("CLOCK is  man. ");
    digitalWrite(CLOCK_OUT, LOW);
  }
}

void SetLEDs() {
  bool isAutoMode = IsModeSetToAuto();
  digitalWrite(MODE_LED, isAutoMode);
  // only light other LEDs if we are not in auto mode!
  digitalWrite(PROGRAM_LED, !isAutoMode && IsProgramSetToRun());
  digitalWrite(CLOCK_LED, !isAutoMode && IsClockSetToAuto());
}


void DoAutoMode() {
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
}

void loop() {
  SetLEDs(); // TODO: remove this

  resetPressed = false;

  bool isAutoMode = IsModeSetToAuto();
  if(isAutoMode) {
    DoAutoMode();
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
