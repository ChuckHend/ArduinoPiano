/* Piano project from Electronics//UWRF PHYS 311 fall 2015
 * Adam Hendel and Mark Christenson
 */
const int recordPin = 16;   //assign pin (A2) to reset recording (Pin 25 on atmega chip)
const int audioPin = 17;    //assign pin (A3) for the audio (Pin 26 on atmega)
const int octave_pin = 18;  //octave multiplier pin (A4) (Pin 27 on atmega)
const int playbackPin = 19; //playback button (A5) (Pin 28 on atmega)

int prevState[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};     //for 13 , all 1/HIGH (off) by default
unsigned long pressStart[13]; //array to hold the time milli() when the one of our switches are pressed
//milli() will record the time passed since power on. we will subract the time the button is released from
//the time it is pressed to record its "down time"

//switches (keys) 1-13, correspond to pins 3-6 and 11-19, respectively on the the Atmega chip
int pinNum[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

//define frequency for each note, in Hz.
int notes[] = {440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880};
int octave_x = 1;    
//multiplier of 1 by default. We change this to 2 and 3 and multiply by the
//notes[] array later

void setup()
{
  //set the pints that are measuring voltage to INPUT, the piezo will be an output.
  pinMode(1, INPUT_PULLUP); //INPUT_PULLUP activates the resistor internal to the ATMEGA328
  pinMode(2, INPUT_PULLUP); //these are pins 1-13 for Arduino IDE
  pinMode(3, INPUT_PULLUP); //but correspond to the physical pins 3-6 and 11-19 on the ATMEGA328
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);

  pinMode(recordPin, INPUT_PULLUP); //recordpin
  pinMode(audioPin, OUTPUT);       //audio pin, physical pin 26 on ATMEGA328
  pinMode(playbackPin, INPUT_PULLUP);//octave_pin, physical pin 27 on ATMEGA328
  pinMode(octave_pin, INPUT_PULLUP);//playback pin, physical pin 28 on ATMEGA328
}

int hearLength = 200; //time in milliseconds that we hear the note each time a button is pressed
                      //holding a switch down will repeat the sound every 300miliseconds with minimal delay
                      //playback excludes the delay between repeated notes
int notePosition = 0; //sets the first note in the array to the first position
int noteList [100];    //creates a blank array to hold 100 notes
unsigned long noteDuration [100]; //blank array to record the durations of the 100 notes
                                  //any variable containing milliseconds works best as an unsigned long
unsigned long downTime =0; //initialize downtime to zero
unsigned long releaseTime = 0; //intialize time switch is released to zero
unsigned long lastNote = millis(); //initialize last note to time when board turned on
unsigned long pauseDuration [100]; //blank array to record the pauses between each note played
 
void loop()
{//**RESET RECORD ARRAY**
  if (digitalRead(recordPin) == LOW){
    lastNote = millis(); //reset time last note played to now
    notePosition = 0;    //resets position in note array to 0
  }
  
  //**OCTAVE SHIFT STATEMENTS***
  if (digitalRead(octave_pin) == LOW){ //if statement, to toggle octave switch
  delay(500); //delay to prevent a rapid toggle when holding switch down
              //the following statements will toggle through factors 1-3, and return a sample
              //of the highest frequency in the octave after each toggle
              //ex. setting when octave_x = 3, our highest note is 880x3 = 2640Hz
  if (octave_x == 1){ 
      octave_x = 2; //sets multiplicative factor of the octave to 2
      tone(audioPin, notes[12]*octave_x,hearLength); //plays a sample of the highest note in octave
      delay(hearLength); //waits for the note to finish playing
  }
  else if (octave_x == 2){
      octave_x = 3;
      tone(audioPin, notes[12]*octave_x,hearLength);
      delay(hearLength);
  }
  else {octave_x = 1;
      tone(audioPin, notes[12]*octave_x,hearLength);
      delay(hearLength);
  }
 }//end octave pin statements

 //***PLAYBACK STATEMENTS***
if (digitalRead(playbackPin) == LOW){ //1=HIGH(not pressed), 0=LOW(pressed)
  for (int i = 0; i < notePosition; i++){       // step through song array, by the length of notes in the song
                                           //we dont want to go through the full 40 positions in the notes array
    delay(pauseDuration[i]);                 //each note is delayed by the amount of time between recorded notes
    tone(audioPin, noteList[i], noteDuration[i]); //play each of the notes stored in the noteList array
    delay(noteDuration[i]);                            // wait for tone to finish (delay here must match the delay in tone function
    } //end playback pin for loop  
}//end playback statements

//***LIVE PLAY & SAVE TO ARRAY STATEMENTS***  
  for(byte i=0; i<13; i++)
  {
     int state = digitalRead(pinNum[i]);
     if(state == LOW && prevState[i] == HIGH)
     {
        // The switch changed to pressed
        pauseDuration[notePosition] = millis()-lastNote;
        prevState[i] = state;
        pressStart[i] = millis();        
        tone(audioPin, notes[i]*octave_x,hearLength);
        delay(hearLength);
        noteList[notePosition] = notes[i]*octave_x;        
     }
     else if(state == LOW && prevState[i] == LOW)
     {
        // The switch is still pressed
       tone(audioPin, notes[i]*octave_x,hearLength);
       delay(hearLength);
     }
     else if(state == HIGH && prevState[i] == LOW)
     {
        // The switch was just released
        lastNote = millis(); //start timer for when last note ended
        releaseTime = millis();
        downTime = releaseTime - pressStart[i];
        noteDuration[notePosition] = downTime;
        prevState[i] = state;
        ++notePosition;
        noTone(audioPin);
     }
     else if(state == HIGH && prevState[i] == HIGH)
     {
        // Switch is not and was not pressed        
        noTone(audioPin);
     }
    }//end "button pressed" for loop
}//end main loop  

