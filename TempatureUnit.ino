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
#define BUTTONPIN   9

 
RF24 radio(CEPIN, CSNPIN);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C  lcd(I2CLOCATION,2,1,0,4,5,6,7);

int buttonState = 0;
long lastTemperatureCheck = 0;
long lastButtonCheck = 0;
long lastButtonState = 0;

void setup(void)
{
  Serial.begin(9600); 

  lcd.begin (16,2);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(LOW);

  dht.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x4c);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();

  pinMode(BUTTONPIN, INPUT);

  lastTemperatureCheck = millis();
  lastButtonCheck = millis();
}

void handleTemperature() {
  int timeBetween = millis() - lastTemperatureCheck;
  if (timeBetween >= 2000) {
    Serial.println("Checking temperature");
    
    lastTemperatureCheck = millis();
    
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
  
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    
    char outBuffer[15]= "";
    char temperatureBuffer[6] = "";
    char humidityBuffer[6] = "";
    dtostrf(temperature, 5, 2, temperatureBuffer);
    dtostrf(humidity, 5, 2, humidityBuffer);
  
    lcd.home();
    lcd.print(temperatureBuffer);
    lcd.setCursor (0,1);
    lcd.print(humidityBuffer);
  
    sprintf(outBuffer,"|%s|%s|", temperatureBuffer, humidityBuffer);
  
  
    Serial.print("Sending: ");
    Serial.println(outBuffer);
  
    // transmit and increment the counter
    radio.write(outBuffer, strlen(outBuffer));

    Serial.println("Sent");
  }
}

void handleButton() {
  int timeBetween = millis() - lastButtonCheck;
  buttonState = digitalRead(BUTTONPIN);
  
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;
    
    if (buttonState == HIGH) {
      lcd.setBacklight(HIGH);
      Serial.println("Button pressed");
    } else {
      lcd.setBacklight(LOW);
      Serial.println("Button unpressed");
    }
  }
}
 
void loop(void)
{
  handleTemperature();
  handleButton();
  
  delay(100);
}
