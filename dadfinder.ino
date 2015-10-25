uint8_t row = 0;
uint8_t col = 6;
uint8_t LEDsRed[6] = {D7, D6, D5, D4, D3, D2};
uint8_t LEDsGreen[6] = {A0, A1, A2, A3, A5, A6};

#include "pitches.h"

void setup() {
    Spark.subscribe("updateEntering", updateEnteringFunc, MY_DEVICES);
    Spark.subscribe("updateExiting", updateExitingFunc, MY_DEVICES);

    for (uint8_t i=0; i<6; i++){
        pinMode(LEDsRed[i], OUTPUT);
        pinMode(LEDsGreen[i], OUTPUT);
        digitalWrite(LEDsRed[i], HIGH);
        digitalWrite(LEDsGreen[i], HIGH);
    }
    
    // Using PWM on the WKP pin rather than 3.3 v for the LED let's me dim them
    pinMode(WKP, OUTPUT);
    digitalWrite(WKP, HIGH);
     
    playStart();
    delay(500);
    renderLocation();
}

void loop() {
    delay(250);
}

// Called by IFTTT when entering location, data is an int 0-7; 0 is top light 7 is bottom
void updateEnteringFunc(const char *event, const char *data){
    row = 1;
    col = atoi(data);
    renderLocation();
}

// Called by IFTTT when entering location, data is an int 0-7; 0 is top light 7 is bottom
void updateExitingFunc(const char *event, const char *data){
    row = 0;
    col = atoi(data);
    renderLocation();
}

void clearLEDs(){
    // LEDs are common power, turn pin HIGH to turn them off, LOW to light them.
    // RED
    for (uint8_t i=0; i<6; i++){
        digitalWrite(LEDsRed[i], HIGH);
        digitalWrite(LEDsGreen[i], HIGH);
    }
}

void renderLocation(){
    
    clearLEDs();
    
    if (col >=0 && col <6){
        if (row==0){
            digitalWrite(LEDsRed[col], LOW);
            analogWrite(WKP, 100);
        } else {
            digitalWrite(LEDsGreen[col], LOW);
            digitalWrite(WKP, HIGH);
        }
        
        if (col==0 && row ==1 || col==5 && row ==0) {
            // getting home, play long alert
            playLongAlert();
        } else {
            playShortAlert();
        }
    } 
}

void playStart(){
    uint8_t numNotes = 6;
    int melody[] = {NOTE_E5, NOTE_G5, NOTE_E6, NOTE_C6, NOTE_D6, NOTE_G6};
    uint8_t noteDurations[] = { 12,12,12,12,12,8 };
    playTune(numNotes, melody, noteDurations);
}

void playShortAlert(){
    uint8_t numNotes = 4;
    int melody[] = {NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5};
    uint8_t noteDurations[] = { 12, 12, 12, 4 };
    playTune(numNotes, melody, noteDurations);
}

void playLongAlert(){
    uint8_t numNotes = 12;
    int melody[] = {NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C5, NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS2};
    uint8_t noteDurations[] = { 12, 12, 12, 4, 12, 12, 12, 4, 12, 12, 12, 4 };
    playTune(numNotes, melody, noteDurations);
}

void playTune(uint8_t numNotes, int* melody, uint8_t* noteDurations){
  for (int thisNote = 0; thisNote < numNotes; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(A4, melody[thisNote],noteDuration);
    
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(A4);
  }
}