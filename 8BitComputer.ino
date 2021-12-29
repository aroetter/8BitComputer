// Mac <-> Arduino Serial Comms Debugging Info
//
// With the Arduino Nano that is soldered onto the EEPROM Programmer Arduino,
// (the one that came with header pins already soldered on)
// Settings are:
// * Tools > Board > "Arduino Nano"
// * Tools > Processor > "ATMega328P"
// * Tools > Port > make sure the right serial port is selected
// * Tools > Serial Monitor > Set Baud to 57600 baud (to avoid garbled output).
//
// If you get an error:
//   "avrdude: ser_open(): can't open device "/dev/cu.usbserial-AL03KKY3": No such file or directory"
//   In the IDE menubar go to Tools > Port and select the correct port.
// If you get errors during upload like:
//   "avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0xf8"
//   In the IDE menubar go to Tools > Processor > "ATMega328P (Old Bootloader)"
//   Actually, as of 3/16/2020 (Arduino 1.8.12) using old bootloaded causes exactly that problem
//   Swithcing to "ATMega328P" fixed it.
//
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

#define CLR                  4 // D4
#define NOT_CLR              12 // D12


#define HALT_IN             11 // D11

volatile bool resetPressed = false;

/* Arduino runs this function once after loading the Nano, or after pressing the HW reset button.
 * Think of this like main() */
void setup() {
  resetPressed = false;
  Serial.begin(57600);

  pinMode(MODE_IN, INPUT);
  pinMode(PROGRAM_IN, INPUT);
  pinMode(CLOCK_IN, INPUT);
  pinMode(RESET_BTN_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RESET_BTN_IN), handleResetButtonEvent, CHANGE);
  pinMode(HALT_IN, INPUT);

  pinMode(MODE_LED, OUTPUT);
  pinMode(PROGRAM_LED, OUTPUT);
  pinMode(CLOCK_LED, OUTPUT);

  pinMode(PROGRAM_OUT, OUTPUT);
  pinMode(CLOCK_OUT, OUTPUT);
  pinMode(CLR, OUTPUT);
  pinMode(NOT_CLR, OUTPUT);

  SetReset(false); // TOOD: do i need this

}

void handleResetButtonEvent() {
  bool curPress = digitalRead(RESET_BTN_IN);
  if (curPress) resetPressed = true;
  SetReset(curPress);
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

bool IsHalted() {
  return HIGH == digitalRead(HALT_IN);
}

void SetReset(bool flag) {
  digitalWrite(CLR, flag ? HIGH : LOW);
  digitalWrite(NOT_CLR, flag ? LOW : HIGH);
}

// Set the outgoing program line appropriately
void SetProgram(bool isRun) {
  digitalWrite(PROGRAM_LED, isRun ? HIGH : LOW);


  digitalWrite(PROGRAM_OUT, isRun ? LOW : HIGH);
}

// Set the outgoing clock line appropriately.
void SetClock(bool isClockAuto) {
  digitalWrite(CLOCK_LED, isClockAuto ? HIGH : LOW);

  // Up on the main board, pressing the old clock button puts us in auto clock.
  // In that case, center pin is tied to GND. left is tied to 5V through a resistor.
  // When button is pressed (aka auto clock mode) Left pin goes high, center pin is 0V.
  digitalWrite(CLOCK_OUT, isClockAuto ? HIGH : LOW);
}

void MomentarilyDepressReset() {
  SetReset(true);
  delay(50); // TODO is this needed?
  SetReset(false);
}

void DoAutoMode() {
  Serial.println("\n\nTop of DoAutoMode()");

  // 1. Clock == Off/Manual
  SetClock(false);

  // 2. Program into load mode
  if(resetPressed) return;
  SetProgram(false);

  // 3. Reset
  if(resetPressed) return;
  MomentarilyDepressReset();

  // 4. Start clock, wait until read a HALT
  if(resetPressed) return;
  SetClock(true);
  while(!IsHalted() && !resetPressed) delay(100); // wait for load to finish.
  Serial.println("Program loaded!");

  // 5. Clock Off/Manual
  if(resetPressed) return;
  SetClock(false);

  // 6. Program into run mode
  if(resetPressed) return;
  SetProgram(true);

  // 7. Master Reset
  if(resetPressed) return;
  MomentarilyDepressReset();

  // 8. Clock Auto
  if(resetPressed) return;
  SetClock(true);

  // 9. Now just sleep forever. until a reset.
  Serial.println("Just waiting for a reset...");
  while(!resetPressed) {
    delay(100);
  }
  Serial.println("Exiting DoAutoMode()");  
}

void loop() {
  resetPressed = false;

  bool isAutoMode = IsModeSetToAuto();
  digitalWrite(MODE_LED, isAutoMode ? HIGH : LOW);
  
  if(isAutoMode) {
    DoAutoMode();
  } else {
    Serial.println("Manual mode. ");
    SetProgram(IsProgramSetToRun());
    SetClock(IsClockSetToAuto());
  }

  delay(100);
}
