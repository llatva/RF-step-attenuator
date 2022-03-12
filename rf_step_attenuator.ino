
/* OH3HZB RF attenuator control board based on *duino pro mini & SSD1306 OLED display */
/* 2016  Lasse Latva OH3HZB */

/* This is based on a library for our Monochrome OLEDs based on SSD1306 
drivers  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
Scrolling code contributed by Michael Gregg
BSD license, check license.txt for more information
All text above must be included in any redistribution */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// If using software SPI (the default case):
#define OLED_MOSI   6 
#define OLED_CLK    7
#define OLED_DC     4
#define OLED_CS     8 
#define OLED_RESET  5

#define ROTSW 10
#define ROTA 11
#define ROTB 12

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 64 
#define LOGO16_GLCD_WIDTH  128 

unsigned long currentTime;
unsigned long loopTime;
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;
boolean switch_prev=false;
boolean switch_now=false;
int atten=20;
boolean drawn=false;
unsigned int step=2;
String minmaxstr="";

void setup()   {                
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);

  // Clear the buffer.
  display.clearDisplay();
  delay(50);

  display.setCursor(0,0);
  display.setTextColor(BLACK,WHITE);
  display.setTextSize(2);
  display.println(" INIT...");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("");  
  display.println(" 0..4 GHz AT90-1107");
  display.println(" digital attenuator"); 
  display.println("");  
  display.setTextSize(1);
  display.println(" OH3HZB 2016"); 
  display.println(" fw: 2016-07-19");

  // set 6 lowest pins in port A to output mode:
  DDRC = DDRC | B00111111;

  pinMode(ROTA, INPUT); // ROTARY ENCODER p1
  pinMode(ROTB, INPUT); // ROTARY ENCODER p2
  pinMode(ROTSW, INPUT_PULLUP); // ROTARY's SWITCH
  display.display();
  delay(3000);  
  display.clearDisplay();

   currentTime = millis();
   loopTime = currentTime; 
}


void loop() {

  currentTime = millis();
  if(currentTime >= (loopTime + 3)){
    // 3ms since last check of encoder
    encoder_A = digitalRead(ROTA);    // Read encoder pins
    encoder_B = digitalRead(ROTB);   
    switch_now = digitalRead(ROTSW); // and the switch
    if((!encoder_A) && (encoder_A_prev)){
      // A has gone from high to low 
      if(encoder_B) {
        // B is high so clockwise
        // increase the attenuation
        if(atten+step < 64) {
           atten = atten+step; 
           drawn=false;
        }
        else {
          atten = 63;
          drawn=false; 
        }
      }   
      else {
        // B is low so counter-clockwise
        // decrease attenuation
        if(atten-step >= 0) {
           atten=atten-step; 
           drawn=false;
        }
        else {
           atten=0;
           drawn=false;
        }
       }
      }
      
      if(atten<0) {
        atten=0;
        drawn=false;
      }
    // check the rotary encoder's switch:
    if((!switch_now) && (switch_prev)){
       if(step == 1) {
          step=2; 
       }
       else if(step == 2) {
          step=6; 
       }
       else if(step == 6) {
          step=12; 
       }
       else if(step == 12) {
          step=1; 
       }
       drawn=false;
    }
    
    encoder_A_prev = encoder_A;     // Store value of A for next time     
    switch_prev = switch_now;
    loopTime = currentTime;  // Updates loopTime
  }  

// AT90-1107 MODULE CONNECTOR PINOUT:
/*
1 NC
2 C1
3 NC (VEE)
4 NC (GND
5 NC (GND)


*/
  


// AT90-1107 TRUTH TABLE:
/*
C16 C8 C4 C2 C1 C0. Attenuation
0 0 0 0 0 0 Loss, Reference
0 0 0 0 0 1 0.5 dB
0 0 0 0 1 0 1.0 dB
0 0 0 1 0 0 2.0 dB
0 0 1 0 0 0 4.0 dB
0 1 0 0 0 0 8.0 dB
1 0 0 0 0 0 16.0 dB
1 1 1 1 1 1 31.5 dB 
*/
  
  if(drawn==false) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("AT90-1107 attenuator");
    display.println("DC..4GHz OH3HZB 2016"); // empty line
    display.println(""); // empty line
  // display.setTextColor(BLACK, WHITE); // 'inverted' text
    display.setTextSize(3);
    if(atten<20) {
    display.print(" ");
    }
    if(atten>0) {
      display.print("-");
    }
    Serial.print("A=");
    if(atten == 0) {
      display.print((float) atten,1); 
      Serial.print((float) atten);      
    }
    else {
      display.print((float) atten/2,1); 
      Serial.print((float) atten/2);
    }
    display.println("dB");  
    Serial.println("dB");   
    display.setTextSize(1);
    
    // format MIN/MAX string to be displayed on the lowest line:
    if(atten==0) {
      display.setTextColor(BLACK, WHITE);
      minmaxstr = " REF ";
    } 
    else if(atten==1) {
      display.setTextColor(BLACK, WHITE);
      minmaxstr = " MIN ";
    } 
    else if(atten==63) {
      display.setTextColor(BLACK, WHITE);
      minmaxstr = " MAX "; 
    }
    else {
      minmaxstr = "     ";
    }
    display.println("");  
    display.print(minmaxstr);
    display.setTextColor(WHITE);
    display.print("    ");
    if(step==1) {
     display.println("STEP: 0.5 dB");  
    } else if(step==2) {
     display.println("STEP: 1 dB");  
    } else if(step==6) {
     display.println("STEP: 3 dB");  
    } else if(step==12) {        
     display.println("STEP: 6 dB");  
    }
    display.display();
    delay(50);
    
    // write the binary value to pins:
    PORTC = atten;
    
    drawn=true;
  }
  
}

