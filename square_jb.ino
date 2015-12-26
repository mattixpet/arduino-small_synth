// synth, square wave, james bond (sama system og í james bond.ino)
//used  http://www.vibrationdata.com/Resources/piano_keys.jpg
//      http://www.phy.mtu.edu/~suits/notefreqs.html

//globals

//inputs
char notes[13]; //13 notes, from c to C
char noteslen = sizeof(notes)/sizeof(notes[0]);

char arpeggiatorIn = 15; //arpeggiator 3 state switch
char octaveIn = 16; //octave 3 state switch
char tempoIn = 17; //tempo change for arpeggiator
char dirtyIn = 18; //for playdirty ( a different sound )

//outputs
char mainOut = A0;

// periods (in microseconds) of wavelengths corresponding to notes
// per[0] = C2, per[1] = C2#, per[2] = D2, ... , per[38] = C5
short per[39];
short perlen = sizeof(per)/sizeof(per[0]);
short per2[39]; //per shifted 2 halfnotes to the right
short per1[39]; //per shifted 1 halfnotes to the right 
short per3[39];

char readings[13]; //readings from the 13 keys (from 0 - 1023)
char readingslen = sizeof(readings)/sizeof(readings[0]);

char dirty = 0; //dirty = 1, play dirty sound else play regular
char arpeggiator = 1; //arpeggiator = 1, normal, =0, down, = 2, up
char octave = 13; //if octave is 0 then lowest octave is
                 //played, = 13 then middle, = 26 then highest
float tempo = 120.0; //default
int eigth = ((int) ((60/tempo)*1000))/2; //eigth part note for arpegg

//s = sharp (#)
enum {
    c2 = 0, c2s = 1, d2 = 2, d2s = 3, e2 = 4, f2 = 5, f2s = 6,
    g2 = 7, g2s = 8, a2 = 9, a2s = 10, b2=11, c3=12, c4=13, c4s=14, 
    d4=15, d4s=16, e4=17, f4=18, f4s=19, g4=20,g4s=21,a4=22,a4s=23,
    b4=24,c5=25,/*cuz c5 is repeated*/c5s=27,d5=28,d5s=29,e5=30,
    f5=31,f5s=32,g5=33,g5s=34,a5=35,a5s=36,b5=37,c6=38
};

//http://www.phy.mtu.edu/~suits/NoteFreqCalcs.html
//fn = f0*(a)^n
//returns period in microseconds
int findPeriod(int distanceFromA4)
{
  return (1/(440*(pow((pow((2),(1.0/12.0))),distanceFromA4))))*1000000;
}

void setup()
{
    //initialize notes
    for (int i = 0; i < noteslen; i++) {
       notes[i] = i+2; //notes[0] = c = bus 2 etc 
    }
    notes[noteslen-1] = 19; //A5=19 instead of 1-13, 2-13+A5
    
    //initialize per[], first 13 are C2-C3, last 26 are C4-C6 (C5 repeated)
    //manual for the first three cuz math.pow doesn't accept -33-31
    per[0] = 15291;
    per[1] = 14430;
    per[2] = 13621;
    int j = -30; //C2 is 33 notes lower than A4
    for (int i = 3; i < 13; i++) {
       per[i] = findPeriod(j);
       j++; 
    }
    j = -9;
    for (int i = 13; i < 26; i++) {
       per[i] = findPeriod(j);
       j++; 
    }
    j = 3;
    for (int i = 26; i < 39; i++) {
       per[i] = findPeriod(j);
       j++; 
    }
    
    //per2 is per shifted 2 to right because for some reason 
    //the notes when played shifted to the left (probably because
    //of the time it took the chip to go through the loop)    
    per2[0] = 13624;
    per2[1] = 12870;
    per2[2] = 12160;
    j = -28; //C2 is 33 notes lower than A4
    for (int i = 3; i < 13; i++) {
       per2[i] = findPeriod(j);
       j++; 
    }
    j = -7;
    for (int i = 13; i < 26; i++) {
       per2[i] = findPeriod(j);
       j++; 
    }
    j = 5;
    for (int i = 26; i < 39; i++) {
       per2[i] = findPeriod(j);
       j++; 
    }
    
    //per 1
    per1[0] = 14430;
    per1[1] = 13621;
    j = -30; //C2 is 33 notes lower than A4
    for (int i = 2; i < 13; i++) {
       per1[i] = findPeriod(j);
       j++; 
    }
    j = -8;
    for (int i = 13; i < 26; i++) {
       per1[i] = findPeriod(j);
       j++; 
    }
    j = 4;
    for (int i = 26; i < 39; i++) {
       per1[i] = findPeriod(j);
       j++; 
    }
    
    //per3
    per3[0] = 12870;
    per3[1] = 12160;
    j = -28; //C2 is 33 notes lower than A4
    for (int i = 2; i < 13; i++) {
       per3[i] = findPeriod(j);
       j++; 
    }
    j = -6;
    for (int i = 13; i < 26; i++) {
       per3[i] = findPeriod(j);
       j++; 
    }
    j = 6;
    for (int i = 26; i < 39; i++) {
       per3[i] = findPeriod(j);
       j++; 
    }
    
    //initialize readings as 0
    for (int i = 0; i < readingslen; i++) {
       readings[i] = 0;
    }
    
    for (int i = 0; i < noteslen; i++) {
       pinMode(notes[i],INPUT);
    }
    pinMode(octaveIn,INPUT);
    pinMode(arpeggiatorIn,INPUT);
    pinMode(tempoIn,INPUT);
    pinMode(dirtyIn,INPUT);
    pinMode(mainOut,OUTPUT);
    
    for (int i = 0; i < noteslen; i++) analogWrite(notes[i],0);
    
    Serial.begin(9600);
        
    delay(50);
}

void playDirty(int note) //bell like
{
   int p = per[note]; //p for period
   
   for (int i = 0; i < 5; i++) {
     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
   }
}

void playDirty1(int note) //bell like
{
   int p = per1[note]; //p for period
   
   for (int i = 0; i < 5; i++) {
     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
   }
}

void playDirty2(int note) //bell like
{
   int p = per2[note]; //p for period
   
   for (int i = 0; i < 5; i++) {
     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
   }
}

void playDirty3(int note) //bell like
{
   int p = per3[note]; //p for period
   
   for (int i = 0; i < 5; i++) {
     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
   }
}

void play(int note)
{
   int p = per[note]; //p for period

     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
}

void play1(int note)
{
   int p = per1[note]; //p for period

     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
}

void play2(int note)
{
   int p = per2[note]; //p for period

     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
}

void play3(int note)
{
   int p = per3[note]; //p for period

     digitalWrite(mainOut,HIGH);  
     delayMicroseconds(p/2.0);
     digitalWrite(mainOut,LOW);
     delayMicroseconds(p/2.0);
}

void initTempo() 
{
   short tem = analogRead(tempoIn); 
   
   if (tem < 650) tempo = 50.0; //min
   else if (tem > 980) tempo = 400.0; //max
   else {
     //980-650 = 330. 330/350 = 0.943.
       tempo = (tem-650.0)/0.943 + 50.0;
   }
   
   int quart = (int) ((60/tempo)*1000); //q = quarterNote [ms]
   eigth = quart/2;
}

void readForArp()
{
      for (char i = 0; i < readingslen; i++) {
         readings[i] = digitalRead(notes[i]); 
      }
}

void playArp(int note)
{
    int p = per[note]; //p for period

    unsigned long timeDiff = 0;       
    unsigned long prevTime = millis();
       
    while (timeDiff < eigth) {

       digitalWrite(mainOut,HIGH);  
       delayMicroseconds(p/2.0);
       digitalWrite(mainOut,LOW);
       delayMicroseconds(p/2.0);
            
       unsigned long afterTime = millis();
       
       timeDiff = afterTime - prevTime;
    }   
    delay(5);
}

void playArpDown()
{
      for (char i = readingslen-1; i >= 0; i--) {
         if (readings[i]) playArp(i+octave);
      }
}

void playArpUp()
{
      for (char i = 0; i < readingslen; i++) {
         if (readings[i]) playArp(i+octave);
      }
}

void loop()
{
    short oct = analogRead(octaveIn);
    if (oct < 745) octave = 0;
    else if (oct < 800) octave = 26;
    else octave = 13;
  
    short arp = analogRead(arpeggiatorIn);
    if (arp < 745) arpeggiator = 0;
    else if (arp < 800) arpeggiator = 2;
    else arpeggiator = 1;
    
    //read the input pins (keys)
    if (arpeggiator == 1) { //regular
    
      dirty = digitalRead(dirtyIn);
      
      if (dirty) {
        if (octave == 13) {
          for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              playDirty2(i+octave);
            }
          }
        }
        else if (octave == 26) {
          for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              playDirty3(i+octave);
            }
          }
        }
        else {
          for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              playDirty1(i+octave);
            }
          }          
        }
      }
      else {
        if (octave == 13) {
          for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              play2(i+octave);
            }
          }
        }
        else if (octave == 26) {
          for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              play3(i+octave);
            }
          }
        }
        else {
           for (char i = 0; i < readingslen; i++) {
            readings[i] = digitalRead(notes[i]);
            //if a key is pressed, play note accordingly
            if (readings[i] == HIGH) {
              play1(i+octave);
            }
          }
        }
      }
    }
    //arpeggiator can't be dirty
    else if (arpeggiator == 0) { //down mode
        initTempo();
        readForArp();
        playArpDown();
    }
    else if (arpeggiator == 2) { //up mode
        initTempo();
        readForArp();
        playArpUp();
    }
}
