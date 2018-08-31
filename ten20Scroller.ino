#include <Adafruit_NeoPixel.h>

#define DEBUG

#ifdef __AVR__
#include <avr/power.h>
#endif

// time between display updates
#define WAIT 500

// Pixels across 
#define WIDTH 20

// Pixels up/down
#define HEIGHT 6

// Arduino pin for display data
#define PIN 6

// Number of pixels connected
#define TOTALLED 120

// max # of letters to display
#define SENTENCELENGTH 20

// max # pixels wide in letter
#define LETTERWIDTH 5

// character in alphabet
typedef struct {
    uint32_t pixelData;
    byte width;    
    byte special;
    char val; // for debug
} character;

/// CHANGE TO ARRAY
character charA, charB, charC, charD, charE, charH, charL, charO, charR, charT, charN, charM, charAMPER, charSPACE, charCOMMA, charPERIOD, charEXCLAIM, charEOL;
// character alphabet[70]; 
// A-Z 0-26
// a-z 27-52
// 0-9 53-62
// space, period, exclaim, comma, hashtag, dash, colon, slash, at? 63-70

// init the led display driver
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTALLED, PIN, NEO_GRB + NEO_KHZ800);

// default for LED ON / off
uint32_t ON = strip.Color(200, 0, 0);   // pixel on = red (eventually allow words to have different colors)
uint32_t OFF = strip.Color(0, 0, 0);    // pixel off 

// characters converted into on/off matrix
bool ledData[HEIGHT][(SENTENCELENGTH * LETTERWIDTH)];

// current index in ledData going left to right
uint8_t currentIndex; 

// last piece of parsed data in array (array size likely larger than needs to be, so no reason for loop to look at "empty" data)
uint8_t maxIndex; 

uint8_t lettersInSentence; 

// holds characters to show in display
character sentence[SENTENCELENGTH];

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  
  Serial.println("build alphabet");
  buildChars();
  
  Serial.println("load sentence");
  sentence[0] = charSPACE;
  sentence[1] = charH;
  sentence[2] = charSPACE;
  sentence[3] = charE;
  sentence[4] = charSPACE;
  sentence[5] = charL;
  sentence[6] = charSPACE;
  sentence[7] = charL;
  sentence[8] = charSPACE;
  sentence[9] = charO;
  sentence[10] = charEOL;

  lettersInSentence = 11;
  
  Serial.println("wait 5 sec");
  delay(5000);
  
  Serial.println("Parse Sentence");
  parseSentence();

  Serial.println("start strip");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  currentIndex = 0;
  Serial.println("Ready to go");
  Serial.print("WIDTH: ");
  Serial.println(WIDTH);
  Serial.print("HEIGHT: ");
  Serial.println(HEIGHT);

  Serial.print("Total leds: ");
  Serial.println(TOTALLED);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // flash each character in sentence
  Serial.print("loop -> current: ");
  Serial.print(currentIndex);
  //Serial.print(" of ");
  //Serial.println(maxIndex);
  Serial.println("");
  
  // while there's more data to show 
  if (currentIndex <= maxIndex) {
    
    for (int r = 0; r < HEIGHT; r++) 
    {
      int limit = currentIndex + WIDTH;
      
      Serial.println("");
      Serial.print("r: ");
      Serial.print(r);
      Serial.print(" ");
      Serial.print(currentIndex);
      Serial.print(" to ");
      Serial.print(limit);
      
      for (int c = currentIndex; c < limit; c++) 
      {        
        Serial.print(" : ");
        Serial.print(c);
        Serial.print("(");
        if (ledData[r][c]) {
          Serial.print(1);
          strip.setPixelColor(((r * WIDTH) + c) - currentIndex, ON);
        } else {
          Serial.print(0);
          strip.setPixelColor(((r * WIDTH) + c) - currentIndex, OFF);      
        }
        Serial.print(")");           
      }
    }

    if (currentIndex % 2 == 0)
      strip.setPixelColor(0, strip.Color(0, 0, 200));
    else
        strip.setPixelColor(0, strip.Color(0, 200, 0));
        
    strip.show();    
  }
  Serial.println("");

  currentIndex++;
  
  delay(WAIT);
}
/////////////////////////////////////////////////////////////////////

// Take the letters and build he pixel maps
void parseSentence() {
  uint8_t idx = 0;
  uint8_t row = HEIGHT; 
  uint8_t comp = 0;

  Serial.println("..parse");  
  
  // skip the width of output to prepend blanks, so text will scroll in from the right
  maxIndex = WIDTH;

  // got through character by character until hit limit or the EOL 
  while (idx < SENTENCELENGTH && sentence[idx].special != 2) {    

    character cur = sentence[idx];
    /*    
    Serial.println();
    Serial.print(maxIndex);
    Serial.print("....");
    Serial.print(idx);
    Serial.print(":");
    Serial.println(cur.val);
    */
    for (int r = 0; r < HEIGHT; r++) 
    {      
      /*
      Serial.print("r: ");
      Serial.print(r);
      Serial.print(" ");
      */
      for (int c = 0; c < cur.width ; c++) 
      {                
        /*
        Serial.print(" ");
        Serial.print("c: ");
        Serial.print(c);
        Serial.print(" (");
        */
        uint32_t shift1 = (r * cur.width);
        uint32_t shifted = shift1 + c;
        unsigned long offset = (unsigned long)1 << shifted;
        /*
        Serial.print(shifted);
        Serial.print(" << ");
        Serial.print(offset);
        Serial.print(")");
        */
        // shift the number for comparison below to see if pixel is on or off        
        bool isOn = (cur.pixelData & offset) == offset;

        // set LED bit to correspond to the character pixel status
        if (isOn) {
          int col = c + maxIndex;
          /*Serial.print(" on (");
          Serial.print(r);
          Serial.print(":");
          Serial.print(col);
          Serial.print(") ");
          */
          ledData[r][col] = true;
        }
      }      
      // Serial.println("");
    } 
    maxIndex += cur.width;
    idx++;   
  }  

  Serial.print("..max index = ");
  Serial.println(maxIndex);
}

/////////////////////////////////////////////////////////////////////
void buildChars() {
  charA.pixelData = 630678;
  charA.width = 4;
  charA.val = 'A';  
  
  charB.pixelData = 7642407;
  charB.width = 4;
  charB.val = 'B';
  
  charC.pixelData = 14713902;
  charC.width = 4;
  charC.val = 'C';

  charE.pixelData = 988959;
  charE.width = 4;
  charE.val = 'E';

  charH.pixelData = 630681;
  charH.width = 4;
  charH.val = 'H';

  charL.pixelData = 29257;
  charL.width = 3;
  charL.val = 'L';

  charO.pixelData = 432534; 
  charO.width = 4;
  charO.val = 'O';
        
  charSPACE.pixelData = 0;
  charSPACE.width = 1;
  charSPACE.val = '_';        

  charEOL.pixelData = 0;
  charEOL.val = '^';
  charEOL.special = 1; 
}

// code from here:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both#_=_
