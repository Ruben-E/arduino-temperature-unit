#include <SPI.h>
#include <RF24.h>
#include <DHT.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2CLOCATION 0x27
#define DHTTYPE     DHT22
#define DHTPIN      2
#define CEPIN       7
#define CSNPIN      8

 
RF24 radio(CEPIN, CSNPIN);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C  lcd(I2CLOCATION,2,1,0,4,5,6,7);
 
void setup(void)
{
  Serial.begin(9600); 

  lcd.begin (16,2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

  dht.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x4c);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();
}
 
void loop(void)
{
  delay(2000);
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // 32 bytes is maximum payload
  char outBuffer[15]= "";
  char temperatureBuffer[6] = "";
  char humidityBuffer[6] = "";
  dtostrf(temperature, 5, 2, temperatureBuffer);
  dtostrf(humidity, 5, 2, humidityBuffer);

  lcd.home();
  lcd.print(temperatureBuffer);
  lcd.setCursor (0,1);
  lcd.print(humidityBuffer);
 

  // pad numbers and convert to string
  //sprintf(outBuffer,"%d.2",temperature);

  sprintf(outBuffer,"|%s|%s|", temperatureBuffer, humidityBuffer);


  Serial.print("Sending: ");
  Serial.println(outBuffer);

  // transmit and increment the counter
  radio.write(outBuffer, strlen(outBuffer));
}
