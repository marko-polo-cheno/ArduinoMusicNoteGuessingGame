// Mark Chen
// May 27 2019
// This arduino circuit will run a little music game where a frequency is played from 2 octaves of the C Major Scale
// A note will be played, and the user can adjust the dial (potentiometer) to attempt to guess the frequency of the sound being played.
// A 4 digit display will show the frequency number that the user wishes to enter as their guess.
// When the button is pressed, the displayed freqency will be locked and comapred to the correct frequency.
// LEDs will display the accuracy of the player, in a format like a traditional traffic light display - red, amber, and green.

//Pin number initialization for the 4 digit display
int latchPin = 2;
int clockPin = 4;
int dataPin = 3;

//Pin number initialization for the LED display with the shift register
int latchPinLED = 8;
int clockPinLED = 12;
int dataPinLED = 11;

//Pin initialization for using the potentiometer
int potPin = 2; 
int potInPin = 6;

//Pin used to sense if the button is pressed
int buttonPin = 10;

//Initialize the size of an integer array for frequencies of the C major scale
int musicNoteArray[15];

//Set user's input as 0 to begin
int userInputVal = 0;

//Initialize the state of the button pressed as 0
int pressedState = 0;

//The factor that will maintain the continuation of a round until the user presses the button
boolean enteredAnswer = false;

//Pin for outputting frequency to the buzzer
int soundOutPin = 13;


// Not my code
// In this array, we will keep binary patterns to help output the 10 binary digits 0-9
byte numbers[10];

// This function sets up the 2 arrays used in this program for holding useful numerical values 
//(for LED and noise purposes)
void setupNumbers() {
  numbers[0] = B11000000;
  numbers[1] = B11111001;
  numbers[2] = B10100100;
  numbers[3] = B10110000;
  numbers[4] = B10011001;
  numbers[5] = B10010010;
  numbers[6] = B10000010;
  numbers[7] = B11111000;
  numbers[8] = B10000000;
  numbers[9] = B10010000;
  musicNoteArray[0]=262;
  musicNoteArray[1]=294;
  musicNoteArray[2]=330;
  musicNoteArray[3]=349;
  musicNoteArray[4]=392;
  musicNoteArray[5]=440;
  musicNoteArray[6]=494;
  musicNoteArray[7]=523;
  musicNoteArray[8]=587;
  musicNoteArray[9]=659;
  musicNoteArray[10]=698;
  musicNoteArray[11]=784;
  musicNoteArray[12]=880;
  musicNoteArray[13]=988;
  musicNoteArray[14]=1046;
}

void setup() {
  //Sets up the array with the correct useful numbers
  setupNumbers();
  
  //Start Serial for debuging purposes
  Serial.begin(9600);
  
  //Required for truly random values
  randomSeed(analogRead(0));
  
  //Set pins to output to control the 4 digit display
  pinMode(latchPin, OUTPUT);
  pinMode(potInPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  //Set pins to output to control the shift register
  pinMode(latchPinLED, OUTPUT);
  pinMode(clockPinLED, OUTPUT);
  pinMode(dataPinLED, OUTPUT);
  
  //Keep a digital pin on designated for the potentiometer's power
  digitalWrite(potInPin, HIGH);

  //Turn off the LEDs
  digitalWrite(latchPinLED, LOW);
  shiftOut(dataPinLED, clockPinLED, MSBFIRST, B00000000);  
  digitalWrite(latchPinLED, HIGH);  
}

void loop() {
  //Gets a random number form 0 to 14 inclusive for picking a note from the array of frequencies
  int rNum = (int) random(0,15);
  
  //Set that the user has not selected an answer
  enteredAnswer = false;
  
  //Until the user inputs an answer with the button
  while(!enteredAnswer){
    
    //Every 50ms, read in the frequency that the user is choosing
    if(millis()%50<=1){
      userInputVal = analogRead(potPin);
      
      //To see the answers and then accurately get green/orange all the time, press CTRL+SHIFT+M and uncomment the line below for the correct answers
      //Serial.println(musicNoteArray[rNum]); 
    
  }
    
    //Read in the state of the button - if it is pressed or not
    pressedState = digitalRead(buttonPin);

    //If the button is pressed, terminate the while loop
    if(pressedState==HIGH){
      delay(300);
      enteredAnswer = true;
      
    //Otherwise
    } else {
      //Keep displaying the chosen frequency
      writeNumber(userInputVal);
      
      //Play the note that the computer selected
      tone(soundOutPin,musicNoteArray[rNum],999);
    }
  }
  
  
  
  
  //Get the difference between the correct answer and the inputted answer
  int difference = abs(userInputVal-musicNoteArray[rNum]);
  
  //Clears the LEDs
  digitalWrite(latchPinLED, LOW);
  
  //Based on accuracy, display respective LED
  if(difference<25){
    //Green
    shiftOut(dataPinLED, clockPinLED, MSBFIRST, B00001000);
  } else if(difference<50){
    //Yellow
    shiftOut(dataPinLED, clockPinLED, MSBFIRST, B00000100);
  } else if(difference<100){
    //Red, but not the worst
    shiftOut(dataPinLED, clockPinLED, MSBFIRST, B00000010);
  } else {
    //Red
    shiftOut(dataPinLED, clockPinLED, MSBFIRST, B00000001);  
  }
  
  //Turns LEDs back on
  digitalWrite(latchPinLED, HIGH);
      

  
  
}








///////////// Everything under this is not my code.



//This function will output a four digit number to the display
void writeNumber(int thouNum) {
  //for each output digit
  for (int i = 4; i > 0; i--) {
    //isolate the digit value for this column
    int place = thouNum % 10;

    //if it is a leading zero, don't display it,
    //otherwise display the digit in the corresponding place
    if (thouNum == 0) {
      writeDigit(i, B11111111);
    } else {
      writeDigit(i, numbers[place]);
    }
    //setup the number so that the next time
    //through the loop, we investigate the next appropriate
    //digit
    thouNum = thouNum / 10;
    //delay for a short period so the pattern has time
    //to register in the viewers eyes 
    delay(1);
  }
}

//This function writes a single bit pattern
void writeDigit(int pos, byte val) {
  byte outDig = 0;
  if (pos == 1) {
    outDig = B00000001;
  } else if (pos == 2) {
    outDig = B00000010;
  } else if (pos == 3) {
    outDig = B00000100;
  } else if (pos == 4) {
    outDig = B00001000;
  }
  digitalWrite(latchPin, 0);
  //count up on GREEN LEDs
  shiftOut(dataPin, clockPin, outDig);
  //count down on RED LEDs
  shiftOut(dataPin, clockPin, val);
  //return the latch pin high to signal chip that it
  //no longer needs to listen for information
  digitalWrite(latchPin, 1);
}

void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i = 0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
  for (i = 7; i >= 0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1 << i) ) {
      pinState = 1;
    }
    else {
      pinState = 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}


