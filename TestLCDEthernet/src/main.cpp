#include <Arduino.h>
#include <Wire.h>                 // LCD
#include <LiquidCrystal_I2C.h>    // LCD
#include <SPI.h>                  // Ethernet
#include <Ethernet.h>             // Ethernet
#include <EthernetUdp.h>
#include <TimeLib.h>

// MAC Address of the Ethernet Shield
//byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
//IPAddress ip(127,0,0,1);         //Fall back IP address

EthernetClient client;           // Ethernet initialization  (port 80 is default for HTTP)

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
    =======================
    create global variables
    =======================
*/

// variable to hold Ethernet shield MAC address
byte mac[] = { 0xAA, 0xBB, 0xCC, 0x00, 0xFE, 0xED };

// define IPAddress object that will contain the NTP server IP address
// We will use an NTP server from https://tf.nist.gov/tf-cgi/servers.cgi
IPAddress timeSrvr(129,6,15,28);

const int timeZone = 2;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
//const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)

//Week Days
String weekDays[7]={"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

// define Ethernet UDP object and local port 8888
EthernetUDP ethernet_UDP;
unsigned int localPort = 8888;

// variable to store previous displayed time
time_t prevDisplay = 0;

// array to hold incoming/outgoing NTP messages
// NTP time message is 48 bytes long
byte messageBuffer[48];

/*
   ===========================================
   NTP code for communicating with NTP server
   ===========================================

   =================================================
   function to request current time from NTP server
   =================================================
*/

/*
   helper function for getTime()
   this function sends a request packet 48 bytes long
*/
void sendRequest(IPAddress &address)
{
  // set all bytes in messageBuffer to 0
  memset(messageBuffer, 0, 48);

  // create the NTP request message

  messageBuffer[0] = 0b11100011;  // LI, Version, Mode
  messageBuffer[1] = 0;           // Stratum, or type of clock
  messageBuffer[2] = 6;           // Polling Interval
  messageBuffer[3] = 0xEC;        // Peer Clock Precision
  // array index 4 to 11 is left unchanged - 8 bytes of zero for Root Delay & Root Dispersion
  messageBuffer[12]  = 49;
  messageBuffer[13]  = 0x4E;
  messageBuffer[14]  = 49;
  messageBuffer[15]  = 52;

  // send messageBuffer to NTP server via UDP at port 123
  ethernet_UDP.beginPacket(address, 123);
  ethernet_UDP.write(messageBuffer, 48);
  ethernet_UDP.endPacket();
}

time_t getTime()
{
  while (ethernet_UDP.parsePacket() > 0) ; // discard packets remaining to be parsed

  Serial.println("Transmit NTP Request message");

  // send packet to request time from NTP server
  sendRequest(timeSrvr);

  // wait for response
  uint32_t beginWait = millis();

  while (millis() - beginWait < 1500) {

    int size = ethernet_UDP.parsePacket();

    if (size >= 48) {
      Serial.println("Receiving NTP Response");

      // read data and save to messageBuffer
      ethernet_UDP.read(messageBuffer, 48);

      // NTP time received will be the seconds elapsed since 1 January 1900
      unsigned long secsSince1900;

      // convert to an unsigned long integer the reference timestamp found at byte 40 to 43
      secsSince1900 =  (unsigned long)messageBuffer[40] << 24;
      secsSince1900 |= (unsigned long)messageBuffer[41] << 16;
      secsSince1900 |= (unsigned long)messageBuffer[42] << 8;
      secsSince1900 |= (unsigned long)messageBuffer[43];

      // returns UTC time
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }

  // error if no response
  Serial.println("Error: No Response.");
  return 0;
}

void printIPAddress(){
    lcd.clear();                           //Clear the LCD
    lcd.setCursor(0,0);                    // Set the cursor on the LCD to Col 1 Row 1
    lcd.print("My IP address: ");          // Print on text on the LCD
    Serial.print("My IP address is: ");    // Print text on the serial monitor
    lcd.setCursor(0,1);                    //set the cursor on the LCD to Col 0 Row 2
    Serial.println(Ethernet.localIP());    // Print the IP address on the Serial monitor
    lcd.print(Ethernet.localIP());         // Print the IP address on the LCD
  }

/*
   ============================================================
   initialize the ff. inside setup():
      1. Arduino Serial interface
      2. Ethernet shield MAC address [part of Ethernet Library]
      3. Ethernet shield IP address [part of Ethernet Library]
      4. setSyncProvider() [part of Time Library]
   ============================================================
*/

void setup() {
  Serial.begin(9600); // Starts the serial communication
  lcd.init();
  lcd.backlight();

  delay(500);
  Serial.println("Sample Program for the Tutorial: Using An Arduino Ethernet Shield To Get Date and Time");

  // get ethernet shield IP via DHCP
  // [part of Ethernet Library]
  while (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP"); // display error
    delay(1000); // retry after 1 sec
  }

  // DHCP assignment successful, display ethernet shield IP to serial
  // [part of Ethernet Library]
  Serial.print("Ethernet Shield IP (DHCP): ");
  Serial.println(Ethernet.localIP());

  // start UDP
  // [part of Ethernet Library]
  ethernet_UDP.begin(localPort);
  Serial.println("Ethernet UDP Start....");

  // pass function getTime() to Time Library to update current time
  // [part of Time Library]
  setSyncProvider(getTime);

  printIPAddress();                  // print your local IP address:
  delay(1000);
}

/*
   =========================================
   helper function for digitalClockDisplay()
   =========================================
*/
void printDigits(int digits) {
  // add colon character and a leading zero if number < 10 
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void printDigitsLCD(int digits) {
  // add colon character and a leading zero if number < 10 
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

/*
   ========================================================================================
   function to display the time in an easy to read format using the Time Library functions
   ========================================================================================
*/
void digitalClockDisplay() {

  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void digitalClockDisplayLCD() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(hour());
  lcd.print(":");
  printDigitsLCD(minute());
  lcd.print(":");
  printDigitsLCD(second());
  lcd.setCursor(0,1);
  printDigitsLCD(day());
  lcd.print(".");
  printDigitsLCD(month());
  lcd.print(".");
  lcd.print(year());
  lcd.print(" - ");
  lcd.print(weekDays[weekday()-1]);
}

void loop() {
  if (timeStatus() != timeNotSet) {   // check if the time is successfully updated
    if (now() != prevDisplay) {       // update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();          // display the current date and time
      digitalClockDisplayLCD();
    }
  }
}

//----------------------------------------------------------------------------------------------------

