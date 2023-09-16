#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN (6)    // data pin of neopixel strip
#define SECTION_LEN (1)     // number of LEDs to be grouped as one
#define NUM_SECTIONS (30)   // number of LED groups
#define NUM_PIXELS (30)    // total number of LEDS
#define HEARTBEAT_PIN (13)  // output pin for heartbeat LED
#define TOUCHPIN (19)      // Input pin for determening sequence length


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
unsigned long startTime = millis();
int countInterval = 1000;
int pressCount = 0;
int lastPresscount;
int inputState;

bool heartbeat = false; // the Builtin Led toggle to indicate the program is working
int loop_count = 0; // the amount of lightning flashes that have occured

const int sec_count = 30;  // the maximum "width" of each lightning sequence
const int seq_count = 30;  // the maximum "duration" of each lightning sequence

int delay_quick = random(15, 40);
int delay_long = random(5000, 10000);

void setup()
{
  pinMode(TOUCHPIN, INPUT);
  pinMode(HEARTBEAT_PIN, OUTPUT);
  pixels.begin();
  Serial.begin(9600);
}

void loop()
{
  userInput();
  inputState = lastPresscount;
  if (inputState != 0){
  
  //Serial.println("FLASH!");
  Serial.print("loop: ");
  Serial.println(loop_count);
  loop_count++;
  //  run lightning sequence, then delay for a while before repeating
  lightning();
  delay(delay_quick);
  if (inputState >= 5){
  lightning();
  delay(delay_quick);
  }
  if (inputState >= 10){
  lightning();
  delay(delay_quick);
  }
  inputState = 0;
  }
}

// turn off all LEDs
void all_off()
{
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}

void userInput(){
   
   
   if (digitalRead(TOUCHPIN) == LOW){
    digitalWrite(HEARTBEAT_PIN, HIGH); 
    unsigned long timeNow = millis();
      if (timeNow - startTime > countInterval){
        pressCount++;
        startTime = timeNow;
        Serial.println(pressCount);
      }
    }
   else {
    lastPresscount = pressCount;
    digitalWrite(HEARTBEAT_PIN, LOW);
    //Serial.println(lastPresscount); 
    pressCount = 0;
   }

  
}
// run lightning flash sequence once (call from main loop to repeat)
void lightning()
{
  // randomly select the color of the current lightning sequence
  int r = random(200, 255);
  int g = random(150, 255) - 100;
  int b = random(200, 255);

/*
  int r = random(0, 255);
  int g = random(0, 255);
  int b = random(0, 255);
*/

  // randomly select the order in which the LED sections will flash
  // for the current lightning sequence
  int seq_order[seq_count];
  int seq_max = 0;
  for (int i = 0; i < seq_count; i++)
  {
    seq_order[i] = random(0, sec_count);
    seq_max = max(seq_max, seq_order[i]);
  }
  
  // randomly select the "duration" of the current lightning sequence
  int sections = lastPresscount + 5;
  
  // randomly select the starting location of the current lightning sequence
  int seq_start = random(0, NUM_SECTIONS - seq_max);
  
  // loop through each of the chosen sections
  for (int j = 0; j < sections; j++)
  {
    // loop through each LED of the current section
    for (int k = 0; k < SECTION_LEN; k++)
    {
      // turn on the current LED
      int pix_cur = ((seq_start + seq_order[j]) * SECTION_LEN) + k;
      pixels.setPixelColor(pix_cur, pixels.Color(r, g, b));

      // turn off the LEDs of the previous section
      if (j > 0)
      {
        int pix_prv = ((seq_start + seq_order[j - 1]) * SECTION_LEN) + k;
        pixels.setPixelColor(pix_prv, pixels.Color(0, 0, 0));
      }
    }

    // very short (random) delay so we actually see the lightning
    int delay_quick = random(15, 40);
    delay(delay_quick);
    pixels.show();
  }

  all_off();
}
