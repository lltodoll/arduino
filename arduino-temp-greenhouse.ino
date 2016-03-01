/*-----( Import needed libraries )-----*/
#include <RCSwitch.h>
#include <OneWire.h> 
#include <LiquidCrystal_I2C.h>

/*-----( Declare Constants )-----*/
const int DAY_TARGET_TEMP = 17;
const int NIGHT_TARGET_TEMP = 19;
const int DIVIATION_TEMP = 2;
const int DS18S20_Pin = 2;
const int RF433_Pin = 10;
const int SEND_RETRY = 3;
const int LDR_PIN = A0;

/*-----( Declare objects )-----*/
LiquidCrystal_I2C lcd(0x27, 16, 2); 
OneWire ds(DS18S20_Pin);
RCSwitch mySwitch = RCSwitch();

/*-----( Declare Variables )-----*/
int heatingOn = false;
int ldrValue = 0;
int targetTemp = 0;

void setup()
{
    Serial.begin(9600);
  
    // initialize the LCD
    lcd.begin();

    // initilize the rf433 sender
    mySwitch.enableTransmit(RF433_Pin);

    // Turn on the blacklight and print a message.
    lcd.backlight();

    lcd.setCursor(0,0);
    lcd.print("Vorzugsbeet");
    lcd.setCursor(0,1);
    lcd.print("TempSteuerung");
}


void loop()
{
    ldrValue = analogRead(LDR_PIN);

    if(ldrValue > 500)
    {
      // The sun is shining, target temp = 17
      targetTemp = DAY_TARGET_TEMP;
    } else {
      targetTemp = NIGHT_TARGET_TEMP;
    }

    
    float curTemp = getTemp();
    int minTemp = targetTemp - DIVIATION_TEMP;
    int maxTemp = targetTemp + DIVIATION_TEMP;
    
    if (curTemp<=(targetTemp-DIVIATION_TEMP)  && heatingOn == false)
    {
          Serial.println("Please make heating on");

          for(int i = 0; i<SEND_RETRY; i++)
          {
              mySwitch.switchOn('a', 1, 2);
              delay(500);
          }
          
          heatingOn = true;   
    }
    else if (curTemp>=(targetTemp+DIVIATION_TEMP) && heatingOn == true)
    {
          Serial.println("Please make heating off");

          for(int i = 0; i<SEND_RETRY; i++)
          {
              mySwitch.switchOff('a', 1, 2);
              delay(500);
          }

          heatingOn = false;
    } 

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ist : ");
    lcd.print(getTemp());
    lcd.print(" C");

    if(heatingOn == true)
    {
      lcd.print("  +");
    }
    
    lcd.setCursor(0,1);
    lcd.print("Soll: ");

    if(targetTemp == DAY_TARGET_TEMP)
    {
      lcd.print(DAY_TARGET_TEMP);
      lcd.print(",00 C");
      lcd.print("  T");
    } else {
      lcd.print(NIGHT_TARGET_TEMP);
      lcd.print(",00 C");
      lcd.print("  N");
    }

    Serial.print("HeatingON: ");
    Serial.print(heatingOn);
    Serial.println(heatingOn);
    delay(10000);
}

float getTemp(){

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   //no more sensors on chain, reset search
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); // start conversion, with parasite power on at the end

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); // Read Scratchpad

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); //using two's compliment
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}
