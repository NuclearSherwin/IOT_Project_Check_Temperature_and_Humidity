/*
 * For this example you'll need the following library:
 * 1) Adafruit DHT sensor library: https://github.com/adafruit/DHT-sensor-library
 * 
 * 2) Arduino-LiquidCrystal-I2C-library: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
 *
 * 3) Blynk Library: https://github.com/blynkkk/blynk-library
 *
 * Conncetions:
 * D1 -> SCL of I2C LCD 
 * D2 -> SDA of I2C LCD
 * D3 -> Out of DHT11/22
 *
 */

#define BLYNK_PRINT Serial  
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Firebase_ESP_Client.h>

#define BLYNK_TEMPLATE_ID "TMPLnwt3Nh1O"
#define BLYNK_DEVICE_NAME "ASM2IOT"
#define BLYNK_AUTH_TOKEN "WvI6ezlz3VqSeZqqPPc0dKKvS1n-8v5u"



/* 2. Define the API Key */
#define API_KEY "AIzaSyCqLz9R6CjuNGH9PtF48brU6hG4Bb-VM5g"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://asm2-oit-default-rtdb.firebaseio.com/" //<dat
// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth AUTH;
FirebaseConfig config;
bool signupOK = false;

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define digital1  D0
#define digital2  D1

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Henryy";//Enter your WIFI name
char pass[] = "quyhoang25082012";//Enter your WIFI password

int val;
DHT dht;

#define DHTPIN D2          // Digital pin 3
// #define DHTTYPE DHT11
//#define DHTTYPE DHT22     

// DHT dht(DHTPIN, DHTTYPE);


LiquidCrystal_I2C lcd(0x27, 16, 2);

BLYNK_WRITE(V0) {
  digitalWrite(D0, param.asInt());
  val=param.asInt();
  Serial.print(param.asInt());
}


void setup()
{
  Serial.begin(9600);
  // Blynk.begin(auth, ssid, pass);
  pinMode(digital2, OUTPUT);
  dht.setup(DHTPIN);


  // lcd
  Wire.begin(D4,D3); 
  lcd.begin();  
  

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);


  // firebase
   WiFi.begin(ssid, pass);
   Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &AUTH, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &AUTH);
  Firebase.reconnectWiFi(true);

}

void loop()
{
  Blynk.run();
  
  if(val==1){
    Serial.println(val);
    // digitalWrite(digital2,LOW);
    Sensor();
  }
  else if (val==0){
    Serial.print(val);
    // digitalWrite(digital2,HIGH);
    lcd.clear();
  }

  delay(1000);
  float h = dht.getHumidity();

  float t = dht.getTemperature();

  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/humidity",h)){
//      Serial.println("PASSED");
       Serial.print("Humidity: ");
       Serial.println(h);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", t)){
//      Serial.println("PASSED");
       Serial.print("Temperature: ");
       Serial.println(t);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  Serial.println("____________________________");
  
  
}

void Sensor()
{
  float h = dht.getHumidity();
  float t = dht.getTemperature(); // or dht.readTemperature(true) for Fahrenheit
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: " + String(h) + " %");
  Serial.print("\t");
  Serial.println("Temperature: " + String(t) + " C");
  delay(1000);

  
  // Blynk.virtualWrite(V5, h);  //V5 is for Humidity
  // Blynk.virtualWrite(V6, t);  //V6 is for Temperature
  
  // Turn on the blacklight and print a message.
  lcd.display();
  lcd.backlight();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // lcd.print("HI");
  lcd.print("Temp " + String(t) + " C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity " + String(h) + "%");
  delay(800);
  
}