/***************************************************
  This is an example sketch for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// LCD INCLUDES
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

// ACCELEROMETER SENSOR INCLUDES
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// For the LCD breakout, you can use any (4 or) 5 pins
#define sclk 4
#define mosi 5
#define cs   6
#define dc   7
#define rst  8  // you can also connect this to the Arduino reset

 //TMP36 Pin Variables Temperature
int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures


// For switching between sensors
unsigned int mode = 0; // Sample window width in mS (50 mS = 20Hz)

// For the Microphone
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
const int sampleWindowV = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
unsigned int sampleV;

boolean running = false;

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// Option 1: use any pins but a little slower
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
float p = 3.1415926;



void setup(void) {
  Serial.begin(9600);
  //Serial.print("hello!");

  // Our supplier changed the 1.8" display slightly after Jan 10, 2012
  // so that the alignment of the TFT had to be shifted by a few pixels
  // this just means the init code is slightly different. Check the
  // color of the tab to see which init code to try. If the display is
  // cut off or has extra 'random' pixels on the top & left, try the
  // other option!
  // If you are seeing red and green color inversion, use Black Tab

  // If your TFT's plastic wrap has a Black Tab, use the following:
  //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  tft.initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab

  //Serial.println("init");

//  uint16_t time = millis();
//  tft.fillScreen(ST7735_BLACK);
//  time = millis() - time;

  //Serial.println(time, DEC);
  //delay(500);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  //testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST7735_WHITE);
  //rotateText();
  tft.setRotation(1); // uncomment and use rotateText() t derive desired orientation
  delay(1000);

  // tft print function!
  //tftPrintTest();
  //delay(4000);

  // a single pixel
//  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
//  delay(500);
//
//  // line draw test
//  testlines(ST7735_YELLOW);
//  delay(500);
//
//  // optimized lines
//  testfastlines(ST7735_RED, ST7735_BLUE);
//  delay(500);
//
//  testdrawrects(ST7735_GREEN);
//  delay(500);
//
//  testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
//  delay(500);
//
//  tft.fillScreen(ST7735_BLACK);
//  testfillcircles(10, ST7735_BLUE);
//  testdrawcircles(10, ST7735_WHITE);
//  delay(500);
//
//  testroundrects();
//  delay(500);
//
//  testtriangles();
//  delay(500);
//
//  mediabuttons();
//  delay(500);

  //Serial.println("done");
  //delay(1000);
  
  // SENSOR TEST - REVISED FOR TFT
  // Serial.begin(9600);
  tft.println("Accelerometer Test"); tft.println("");
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    tft.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL345_RANGE_16_G);
  // displaySetRange(ADXL345_RANGE_8_G);
  // displaySetRange(ADXL345_RANGE_4_G);
  // displaySetRange(ADXL345_RANGE_2_G);
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Display additional settings (outside the scope of sensor_t) */
  displayDataRate();
  displayRange();
  tft.println("");
}

void loop() {

  Serial.print("In the loop and Mode is: ");  
  Serial.println(mode);
  switch (mode) {
    case 0:
      printAccelerometerValues();
      break;
    case 1:
      printTemperatureValues();
      break;
    case 2:
      printMicVoltageValues();
      break;      
  }    

}

void clearTheScreen() {
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0); 
}

void printAccelerometerValues() {
  clearTheScreen();
  int maxScreenLineCount = 16;
  
  for (int lineCounter = 0; lineCounter < maxScreenLineCount; lineCounter++) { 
    // turn the pin on:
    /* Get a new sensor event */ 
    sensors_event_t event; 
    accel.getEvent(&event);
    
    /* Display the results (acceleration is measured in m/s^2) */
    //printMode();
    tft.setTextColor(ST7735_BLUE);
    tft.print("X:"); tft.print(event.acceleration.x); tft.print(" ");
    tft.setTextColor(ST7735_GREEN);
    tft.print("Y:"); tft.print(event.acceleration.y); tft.print(" ");
    tft.setTextColor(ST7735_WHITE);
    tft.print("Z:"); tft.print(event.acceleration.z); tft.print("");tft.println("ms2 ");
    getVolumeVoltage();
    //delay(100);
  }
}

void printTemperatureValues() {
  clearTheScreen();
  int maxScreenLineCount = 16;
  
  for (int lineCounter = 0; lineCounter < maxScreenLineCount; lineCounter++) { 
    //getting the voltage reading from the temperature sensor
     int reading = analogRead(sensorPin);  
     
     // converting that reading to voltage, for 3.3v arduino use 3.3
     float voltage = reading * 5.0;
     voltage /= 1024.0; 
     
     // print out the voltage
     //tft.print(voltage); tft.println(" volts");
     
     // now print out the temperature
     float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                   //to degrees ((voltage - 500mV) times 100)
     //tft.print(temperatureC); tft.println(" degrees C");
     
     //printMode();
     // now convert to Fahrenheit
     float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
     tft.print(temperatureF); tft.println(" degrees F");
     //delay(100);
     getVolumeVoltage();
  }
}

void printMicVoltageValues() {
  clearTheScreen();
  int maxScreenLineCount = 16;
  
  for (int lineCounter = 0; lineCounter < maxScreenLineCount; lineCounter++) { 
     unsigned long startMillis= millis();  // Start of sample window
     unsigned int peakToPeak = 0;   // peak-to-peak level
  
     unsigned int signalMax = 0;
     unsigned int signalMin = 1024;
  
     // collect data for 50 mS
     while (millis() - startMillis < sampleWindow)
     {
        sample = analogRead(1);
        if (sample < 1024)  // toss out spurious readings
        {
           if (sample > signalMax)
           {
              signalMax = sample;  // save just the max levels
           }
           else if (sample < signalMin)
           {
              signalMin = sample;  // save just the min levels
           }
        }
     }
     peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
     double volts = (peakToPeak * 3.3) / 1024;  // convert to volts
     
     //printMode();
     tft.print("Volume voltage is: ");
     tft.println(volts);
     getVolumeVoltage();
  }  
   
}

void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  //tft.setCursor(0, 0);
  //tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(0);
  tft.setTextColor(ST7735_WHITE);
  tft.println("------------------------");
  tft.setTextColor(ST7735_GREEN);
  tft.print  ("Sensor:       "); tft.println(sensor.name);
  tft.print  ("Driver Ver:   "); tft.println(sensor.version);
  tft.print  ("Unique ID:    "); tft.println(sensor.sensor_id);
  tft.print  ("Max Value:    "); tft.print(sensor.max_value); tft.println(" m/s^2");
  tft.print  ("Min Value:    "); tft.print(sensor.min_value); tft.println(" m/s^2");
  tft.print  ("Resolution:   "); tft.print(sensor.resolution); tft.println(" m/s^2");  
  tft.setTextColor(ST7735_WHITE);
  tft.println("------------------------");
  tft.println("");
  delay(3000);
}

void displayDataRate(void)
{
  tft.print  ("Data Rate:    "); 
  
  switch(accel.getDataRate())
  {
    case ADXL345_DATARATE_3200_HZ:
      tft.print  ("3200 "); 
      break;
    case ADXL345_DATARATE_1600_HZ:
      tft.print  ("1600 "); 
      break;
    case ADXL345_DATARATE_800_HZ:
      tft.print  ("800 "); 
      break;
    case ADXL345_DATARATE_400_HZ:
      tft.print  ("400 "); 
      break;
    case ADXL345_DATARATE_200_HZ:
      tft.print  ("200 "); 
      break;
    case ADXL345_DATARATE_100_HZ:
      tft.print  ("100 "); 
      break;
    case ADXL345_DATARATE_50_HZ:
      tft.print  ("50 "); 
      break;
    case ADXL345_DATARATE_25_HZ:
      tft.print  ("25 "); 
      break;
    case ADXL345_DATARATE_12_5_HZ:
      tft.print  ("12.5 "); 
      break;
    case ADXL345_DATARATE_6_25HZ:
      tft.print  ("6.25 "); 
      break;
    case ADXL345_DATARATE_3_13_HZ:
      tft.print  ("3.13 "); 
      break;
    case ADXL345_DATARATE_1_56_HZ:
      tft.print  ("1.56 "); 
      break;
    case ADXL345_DATARATE_0_78_HZ:
      tft.print  ("0.78 "); 
      break;
    case ADXL345_DATARATE_0_39_HZ:
      tft.print  ("0.39 "); 
      break;
    case ADXL345_DATARATE_0_20_HZ:
      tft.print  ("0.20 "); 
      break;
    case ADXL345_DATARATE_0_10_HZ:
      tft.print  ("0.10 "); 
      break;
    default:
      tft.print  ("???? "); 
      break;
  }  
  tft.println(" Hz");  
}

void displayRange(void)
{
  tft.print  ("Range:         +/- "); 
  
  switch(accel.getRange())
  {
    case ADXL345_RANGE_16_G:
      tft.print  ("16 "); 
      break;
    case ADXL345_RANGE_8_G:
      tft.print  ("8 "); 
      break;
    case ADXL345_RANGE_4_G:
      tft.print  ("4 "); 
      break;
    case ADXL345_RANGE_2_G:
      tft.print  ("2 "); 
      break;
    default:
      tft.print  ("?? "); 
      break;
  }  
  tft.println(" g");  
}


// Used to identify orientation values that can be assigned.
void rotateText() {
  for (uint8_t i=0; i<4; i++) {
    //tft.fillScreen(ST7735_BLACK);
    //Serial.println(tft.getRotation(), DEC);

    tft.setCursor(0, 30);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(1);
    tft.print("Rotation value is: ");
    tft.println(tft.getRotation()+1);
  
    tft.setRotation(tft.getRotation()+1);
  }
}

void getVolumeVoltage() {
  Serial.println("Gonna check the volume voltage..");  
  
  if ( running == false ) {
    running = true;
    
    Serial.print("Mode is: ");  
    Serial.println(mode);
  
     unsigned long startMillisV= millis();  // Start of sample window
     unsigned int peakToPeakV = 0;   // peak-to-peak level
    
     unsigned int signalMaxV = 0;
     unsigned int signalMinV = 1024;
    
     // collect data for 50 mS
     while (millis() - startMillisV < sampleWindowV) {   
        sampleV = analogRead(1);
        // toss out spurious readings
        if (sampleV < 1024) {
           if (sampleV > signalMaxV)
           {
              signalMaxV = sampleV;  // save just the max levels
           }
           else if (sampleV < signalMinV)
           {
              signalMinV = sampleV;  // save just the min levels
           }
        }
     }
     peakToPeakV = signalMaxV - signalMinV;  // max - min = peak-peak amplitude
     double voltsV = (peakToPeakV * 3.3) / 1024;  // convert to volts
     Serial.print("I know my volts: "); 
     Serial.println(voltsV);
     
     double switchVolts = 0.50;  // volume level required to switch sensors
     if ( running = true ) {
       if ( voltsV > switchVolts ) {
         Serial.println("voltsV > switchVolts: "); 
         Serial.print("Mode will be increased by 1");
         mode++;
       }
       if ( mode == 3 ) {
         mode = 0;
       }
       running = false;
     }
  }
  else {
     Serial.println("Already running. Sit tight.");
  }  

}

void printMode() {
  tft.print("Mode is: ");
  tft.println(mode);
}

