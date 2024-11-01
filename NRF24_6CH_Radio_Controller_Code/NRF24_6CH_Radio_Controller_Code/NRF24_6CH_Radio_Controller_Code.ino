/*A basic 6 channel transmitter using the nRF24L01 module.*/
/* Like, share and subscribe, ELECTRONOOBS */
/* http://www.youtube/c/electronoobs */

/* First we include the libraries. Download it from 
   my webpage if you donw have the NRF24 library */
 
#include <SPI.h>
#include <nRF24L01.h>             //Downlaod it here: https://www.electronoobs.com/eng_arduino_NRF24_lib.php
#include <RF24.h> 
#include <LiquidCrystal_I2C.h>             


LiquidCrystal_I2C lcd(0x27, 16, 2);
/*Create a unique pipe out. The receiver has to 
  wear the same unique code*/
  
const uint64_t pipeOut = 0xE8E8F0F0E1LL; //IMPORTANT: The same as in the receiver!!!
int count = 0;
RF24 radio(9,10); // select  CE and CSN  pins

// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
  byte AUX3;
};

MyData data;

// Variables to handle LCD update timing
unsigned long lcdUpdateTime = 0;
const unsigned long lcdUpdateInterval = 100; // Update interval in milliseconds

void resetData() 
{
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.
    
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
  data.AUX3 = 0;
}

void setup()
{
  //Start everything up
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();
  Serial.begin(9600);

  lcd.init();                          // initialize the lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.clear();
  lcd.print("Arduino");
  lcd.setCursor(0, 1);
  lcd.print("Transmitter");
  delay(1200);
  lcd.clear();
  lcd.print("For Arduino");
  lcd.setCursor(0, 1);
  lcd.print("Mini FPV Drone");  
  delay(2200);
  lcd.clear();

  Serial.begin(9600);
}

/**************************************************/

// Returns a corrected value for a joystick position that takes into account
// the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}
int mapJoystickValues1(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 40)+1;
  else
    val = map(val, middle, upper, 40, 80)+1;
  return ( reverse ? 80 - val : val );
}
int mapJoystickValues2(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 169)+1;
  else
    val = map(val, middle, upper, 169, 255)+1;
  return ( reverse ? 255 - val : val );
}
int mapJoystickValues3(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 144)+1;
  else
    val = map(val, middle, upper, 144, 255)+1;
  return ( reverse ? 255 - val : val );
}
void loop()
{
  
  // The calibration numbers used here should be measured 
  // for your joysticks till they send the correct values.
  data.throttle = mapJoystickValues( analogRead(A3), 13, 524, 1000, false );
  data.yaw      = mapJoystickValues1( analogRead(A1), 13, 524, 1015, false );
  data.pitch    = mapJoystickValues2( analogRead(A2), 0, 495, 1020, false );
  data.roll     = mapJoystickValues3( analogRead(A6), 34, 522, 1020, false );
  data.AUX1     = digitalRead(5);
  data.AUX2     = digitalRead(3);
  data.AUX3     = mapJoystickValues( analogRead(A0), 13, 524, 1015, false );
  
  

  radio.write(&data, sizeof(MyData));
  if(count == 200){
    updateLCD();
    count = 0;
  }
  count = count + 1;
}

void updateLCD() {
  

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(data.throttle);

  lcd.setCursor(6, 0);
  lcd.print("Y:");
  lcd.print(data.yaw);

  lcd.setCursor(12, 0);
  lcd.print("A1:");
  lcd.print(data.AUX1);

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(data.pitch);

  lcd.setCursor(6, 1);
  lcd.print("R:");
  lcd.print(data.roll);

  lcd.setCursor(12, 1);
  lcd.print("A2:");
  lcd.print(data.AUX2);

  // Print all values to Serial Monitor
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(" | Yaw: ");
  Serial.print(data.yaw);
  Serial.print(" | Pitch: ");
  Serial.print(data.pitch);
  Serial.print(" | Roll: ");
  Serial.print(data.roll);
  Serial.print(" | AUX1: ");
  Serial.print(data.AUX1);
  Serial.print(" | AUX2: ");
  Serial.print(data.AUX2);
  Serial.print(" | AUX3: ");
  Serial.println(data.AUX3);
  
}