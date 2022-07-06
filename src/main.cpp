
#include <Arduino.h>
#
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <Arduino_JSON.h>
#include "HttpClient.h"
#include <Wire.h>
#include <credentials.h>
#include <Adafruit_MLX90614.h>
//#include <ArduinoJSON.h>
#include <Weather.cpp>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"



#define SENSOR_PIN  21 

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  mlx.begin();
  Serial.print(mlx.readObjectTempC());
  Firebase.begin(&config, &auth);
  //Firebase.reconnectWiFi(true);
    
  }
String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}"; 

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}


int count = 1;
String value = "";
String name = "Temperature";
String jsonBuffer;
FirebaseJson &weather = fbdo.to<FirebaseJson>();
void loop(){
 if (Firebase.ready() && (millis() - sendDataPrevMillis > 900000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    value = name + count;
   FirebaseJson weather;
   WiFiClient client;
      if(WiFi.status()== WL_CONNECTED){
      String location = "http://api.openweathermap.org/data/2.5/weather?lat=-39.34&lon=174.278&appid=";
      String path = location + WeatherAPI + "&units=metric";
      String serverPath = path;
    // HTTPClient http;
    jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    JSONVar myObject = JSON.parse(jsonBuffer);
    //Serial.println(myObject["weather"][0]["id"]);
    //Serial.println(myObject["weather"]);
    int id = myObject["weather"][0]["id"];
    long time = myObject["dt"];
    double ground = mlx.readObjectTempC();
    double Temp = myObject["main"]["temp"];
    double Feels_Like = myObject["main"]["feels_like"];
    double Temp_Max = myObject["main"]["temp_max"];
    double Temp_Min = myObject["main"]["temp_min"];
    int Pressure = myObject["main"]["pressure"];
    double Humidity = myObject["main"]["humidity"];
    int Sea_Level = myObject["main"]["sea_level"];
    double Grnd_level = myObject["main"]["grnd_level"];
    int Visibility = myObject["visibility"];
    double Wind_Speed = myObject["wind"]["speed"];
    int Wind_Deg = myObject["wind"]["deg"];
    double Wind_Gust = myObject["wind"]["gust"];
    int Clouds = myObject["clouds"]["all"];
    int Sunrise = myObject["sys"]["sunrise"];
    int Sunset = myObject["sys"]["sunset"];
    weather.add("Time",time);
    weather.add("Ground",ground);
    weather.add("Temp",Temp);
    weather.add("Feels_Like");
    weather.add("Temp_Max",Temp_Max);
    weather.add("Temp_Min",Temp_Min);
    weather.add("Pressure",Pressure);
    weather.add("Humidity",Humidity);
    weather.add("Sea_Level",Sea_Level);
    weather.add("Ground_Level",Grnd_level);
    weather.add("Visibility",Visibility);
    weather.add("Wind_Speed",Wind_Speed);
    weather.add("Wind_Deg",Wind_Deg);
    weather.add("Wind_Gust",Wind_Gust);
    weather.add("Clouds",Clouds);
    weather.add("Sunrise",Sunrise);
    weather.add("Sunset",Sunset);
    Serial.println("count");
    //Firebase.RTDB.pushJSON(&fbdo, value, &weather);
  
    }
    count++;
      }
  }  
