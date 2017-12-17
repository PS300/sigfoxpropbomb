/*
  SigFox First Configuration

  This sketch demonstrates the usage of MKRFox1200 SigFox module.
  Since the board is designed with low power in mind, it depends directly on ArduinoLowPower library

  This example code is in the public domain.
*/

#include <SigFox.h>
#include <ArduinoLowPower.h>
#include <rgb_lcd.h>


rgb_lcd lcd;
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

unsigned long starttime; //via millis bijhouden om met verschil te kunnen werken
int bombState = 0;
int timeSet = 700; //10=1sec
int timeLeft = timeSet;
unsigned long lastSend = millis();

void setup() {
  Serial.begin(9600);
  while (!Serial) {};

  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:
  pinMode(0, INPUT_PULLUP);
  
  if (!SigFox.begin()) {
    Serial.println("Shield error or not present!");
    return;
  }
  // Enable debug led and disable automatic deep sleep
  // Comment this line when shipping your project :)
  SigFox.debug();

  String version = SigFox.SigVersion();
  String ID = SigFox.ID();
  String PAC = SigFox.PAC();

  // Display module informations
  Serial.println("MKRFox1200 Sigfox IoT project bomb");
  Serial.println("SigFox FW version " + version);
  Serial.println("ID  = " + ID);
  Serial.println("PAC = " + PAC);

  Serial.println("");

  Serial.print("Module temperature: ");
  Serial.println(SigFox.internalTemperature());

  delay(100);

  // Send the module to the deepest sleep
  SigFox.end();
  Serial.println("SigFox setup complete!");
  Serial.println("Bomb state: idle");
  lcd.setCursor(0,0);
  lcd.print("status: Idle   ");

  //function for testing Transmission
  //typeString();
  
}

void loop()
{
  switch (bombState) {
    case 0:
      if(digitalRead(0)==0)
      {
        bombState++;
        lcd.setRGB(colorR, colorG, colorB);
        Serial.println("state: armed");
        sendMessage(true);
        lcd.setCursor(0,0);
        lcd.print("status: Armed   ");
        delay(4000);
      }
      break;
    case 1: //state: armed
      if(digitalRead(0)==0)
      {
        bombState++;
        starttime = millis();
        lcd.setCursor(0, 0);
        lcd.clear();
        Serial.println("start running");
        lcd.setCursor(0,0);
        lcd.print("status: Running...");
        lcd.setRGB(0, 255, 0);
        delay(4000);
      }
      break;
    case 2: //state: running
      int now = millis()-starttime;
      timeLeft = timeSet-now/100;
      lcd.setCursor(6,1);
      lcd.print(timeLeft);
      lcd.print(" left   ");
      lcd.setRGB(255, timeLeft/10, 0);
      if (timeLeft == 600) {sendMessage(false);}
      if (timeLeft<1) {explode();} //explode if timer gets to 0
      break;
   }
}

boolean sendMessage(boolean hold) {
  if ((millis()-lastSend)>15000) {
    sendFloat();
    return true;
    } //only send message when last one was more then 15 seconds ago
  else {
    Serial.println("No transmission allowed YET");
    return false;
    if (hold == true) {
      delay (1000);
      sendMessage(true);
    }
  }
}

void typeString() {
  Serial.println("Type the message to be sent");
  while (!Serial.available());

  String message;
  while (Serial.available()) {
    message += (char)Serial.read();
  }

  // Every SigFox packet cannot exceed 12 bytes
  // If the string is longer, only the first 12 bytes will be sent

  if (message.length() > 12) {
    Serial.println("Message too long, only first 12 bytes will be sent");
  }

  Serial.println("Sending " + message);

  // Remove EOL
  message.trim();

  // Example of message that can be sent
  if(message=="float") {
    sendFloat();
    }
  else {sendString(message);}
}

void sendString(String str) {
  // Start the module
  SigFox.begin();
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.print(str);

  int ret = SigFox.endPacket();  // send buffer to SIGFOX network
  if (ret > 0) {
    Serial.println("No transmission");
  } else {
    Serial.println("Transmission ok");
  }

  Serial.println(SigFox.status(SIGFOX));
  Serial.println(SigFox.status(ATMEL));
  SigFox.end();
}

void sendFloat() {
  float lat=7.11533E-10;
  float lng=52.2505E-10;
  byte hours = (int) timeLeft/36000;
  byte minutes = (int) (timeLeft%3600)/600;
  byte seconds = (int) (timeLeft%600)/10;
  byte timeState[4] = {hours, minutes, seconds, bombState};
  
  Serial.println("Float value being sent");
  // Start the module
  SigFox.begin();
  // Wait at least 30mS after first configuration (100mS before)
  delay(100);
  // Clears all pending interrupts
  SigFox.status();
  delay(1);

  SigFox.beginPacket();
  SigFox.write((float)lat);
  SigFox.write((float)lng);
    SigFox.write(timeState,4);

  int ret = SigFox.endPacket();  // send buffer to SIGFOX network
  if (ret > 0) {
    Serial.println("No transmission");
  } else {
    Serial.println("Transmission ok");
  }

  Serial.println(SigFox.status(SIGFOX));
  Serial.println(SigFox.status(ATMEL));
  SigFox.end();
  lastSend = millis(); // remember when last message was sent
}

void explode(void)
{
  //noInterrupts();
  Serial.println("state: exploded");
  sendMessage(true);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bomb exploded!!!");
  for (int i = 0; i<12; i++)
  {
    digitalWrite(BUZZER,HIGH);
    delay(5);
    digitalWrite(BUZZER,LOW);
    delay(10);
  }
  delay(60000);
  //wacht op sleutel
}
