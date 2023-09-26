#include <SPI.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

const char* ssid = "RAND-MIFI";
const char* password = "reddeadred";
String constatus ;
WiFiClient client;

unsigned long myChannelNumebr = 1;
const char * myWriteAPIKey = "EZWWXK6KUUCVDFPB";

//timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);
Adafruit_BMP085 bmp;

//Reading variables
float TempBMP,TempDHT,Pressure,Altitude,Humidity;

void initBMP(){
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
	while (1) {}
  }
}

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
  

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}











void setup() {

  Serial.begin(9600);
  delay(1000);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  print_wakeup_reason();

  dht.begin();
  initBMP();

  // WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  delay(2000);
  ThingSpeak.begin(client);


  ////////////////////FUNCTION CODES///////////////////////

  TempBMP = bmp.readTemperature();
  Serial.print("Temperature_BMP = ");
  Serial.print(TempBMP);
  Serial.println(" *C");

  Pressure = bmp.readPressure();  
  Serial.print("Pressure = ");
  Serial.print(Pressure);
  Serial.println(" Pa");
    
  Serial.print("calc_Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

  Altitude = bmp.readAltitude(101000);
  Serial.print("Real altitude = ");
  Serial.print(Altitude);
  Serial.println(" meters");
  Serial.println();

  //DHT
  TempDHT = dht.readTemperature();
  Serial.print("Temperature_DHT = ");
  Serial.print(TempDHT);
  Serial.println("*C");

  Humidity = dht.readHumidity();  
  Serial.print("Humidity_DHT = ");
  Serial.print(Humidity);
  Serial.println("%");  
  
  //sending data to ThingSpeak

  ThingSpeak.setField(1,TempBMP);
  ThingSpeak.setField(2,TempDHT);
  ThingSpeak.setField(3,Pressure);
  ThingSpeak.setField(4,Humidity);
  int x =ThingSpeak.writeFields(myChannelNumebr, myWriteAPIKey);
  if(x==200){
    Serial.println("Channel Update Successful");
  }
  else{
    Serial.println("Error Updating. Http Error Code : " + String(x));
  }

  Serial.println("----------------------------------------------------------------");

esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +" Seconds");

Serial.println("Initiating Deep Sleep");
delay(1000);
Serial.flush();
esp_deep_sleep_start();

}
  
void loop() {

}




