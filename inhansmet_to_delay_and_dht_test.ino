
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <dht11.h>
   

const char* ssid ="AndroidAP8803";//"Lenovo";// 
const char* password ="zzsr2830";//"12345filo";// 
// what digital pin we're connected to
#define DHT11PIN 25
dht11 DHT11;
int humi=0;
int tempC=0;
boolean tempstatus=false;
boolean himstatus=false;
//---------------------ultra-------------------------------
//---------------------ultra-------------------------------
boolean ultraflag=false;
const int trigPin = 2;
const int echoPin = 5;
long duration;
int distance;
//-------------------Gas-Sensor---------------------------
#define gasAnalogPin 26

boolean gasFlag = false;
float sensorVoltage;
float sensorValue;
//------------------------end-of-gas--------------------------------

//-------------------Water-Sensor-------------------------
#define waterAnalogPin 33

boolean waterFlag = false;
int val = 0;
int data = 0;
//---------------------------end-of-water-----------------------------
  //=======================================water thread functions=====================================
#include <pthread.h>
void *dht(void *DHT11PINs) {
   Serial.println();

  int chk = DHT11.read(DHT11PIN);

  Serial.print("Humidity (%): ");
   humi=DHT11.humidity;
  Serial.println(humi);
 tempC=DHT11.temperature;
  Serial.print("Temperature (C): ");
  Serial.println(tempC);
  }
  //===============================water=======================================
void *water(void *waterAnalogPins) {
 val = analogRead(33); //readthesimulationvalueandsendtovariableval
  
  if(val>300){//judgmentvariablevaliswhethermorethan700ornot
    waterFlag =true;
    //digitalWrite(waterLED, HIGH); //variable valgreaterthan300,LEDon
    
    Serial.print("water :");
      Serial.println(val);
    }else{
      waterFlag =false;
      //digitalWrite(waterLED, LOW);
       Serial.print("water :");
      Serial.println(val);
    } 

  } 


//////=====================================gas thread function============================================================

void *gas(void *gasAnalogPins) {
    float sensorValue = analogRead(26);
  
  sensorVoltage = sensorValue/1024*5.0;
 
  if(sensorVoltage >= 350){
    gasFlag =true;
    //digitalWrite(fan, HIGH);
    Serial.print("gas :");
      Serial.println(sensorValue);
    }else{
     gasFlag = false;
      //digitalWrite(fan, LOW);
      Serial.print("gas :");
      Serial.println(sensorValue);
    }
   
  }


//================================================Ultrasonic Sensor threAD function =======================================
void *ultra(void *waterAnalogPins) { 
  
  
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
if(distance>15){ultraflag=true;}else{ultraflag=false;}
    
}


  
void setup() {
   Serial.begin(115200);
  //--------------------------leds////////////////////////
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(21,OUTPUT);
   pinMode(22,OUTPUT);
 //------------------------------------------ultra------------------------------
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
//------------------------------------------end-ultra------------------------------

 // ------------------------We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //-----------------------------------------------------end wifi connection-------
     
}



void loop(){
  pthread_t threads[6];
   int returnValue;
   int returnValue2;
   int returnValue3;
   int returnValue4;
 
  
  returnValue = pthread_create(&threads[1], NULL, water, (void *)waterAnalogPin);
 
      if (returnValue) {
         Serial.println("An error has occurred"); 
                       }
       returnValue2 = pthread_create(&threads[2], NULL, gas, (void *)gasAnalogPin);
 
      if (returnValue2) {
         Serial.println("An error has occurred"); 
                       }
                       returnValue3 = pthread_create(&threads[3], NULL, ultra, (void *)gasAnalogPin);
 
      if (returnValue3) {
         Serial.println("An error has occurred"); 
                       }
                       returnValue4 = pthread_create(&threads[4], NULL, dht, (void *)DHT11PIN);
       if (returnValue4) {
         Serial.println("An error has occurred"); 
                       }
   delay(1000);

 /////////////////////////////////////////iot/////////////////////////////////////////////  
    if(WiFi.status() == WL_CONNECTED){
  HTTPClient http;
//  ///////////////////////////////////////-------send---------------///////////////////////////    
//if(tempC!=NULL){
    http.begin("https://ticket-resrevation-app.web.app/api/v1/DHT11hum/DHT11temp");
    http.addHeader("Content-Type", "application/json");
    
    
    StaticJsonDocument<20> tempDocument1;
    String temp=String(tempC);
    Serial.println("tempsend");
    Serial.println(temp);
    tempDocument1["value"] = temp;
    char buffer1[20];
    serializeJson(tempDocument1, buffer1);
    int sensor1=http.PATCH(buffer1);
Serial.println(sensor1);
http.end();

//else{}
//delay(500);
//-------------------------------------------------humi sensor-------------------------------
//
//if(humi!=NULL){
    http.begin("https://ticket-resrevation-app.web.app/api/v1/DHT11hum/DHT11hum");
    http.addHeader("Content-Type", "application/json");
    
    
    StaticJsonDocument<20> tempDocument5;
    String hum=String(humi);
    Serial.println("humsend");
    Serial.println(hum);
    tempDocument5["value"] = hum;
    char buffer5[20];
    serializeJson(tempDocument5, buffer5);
    int sensor2=http.PATCH(buffer5);
Serial.println(sensor2);
 
    
 
    http.end();
 //----------------------------------ultra----------------------------------

    http.begin("https://ticket-resrevation-app.web.app/api/v1/DHT11hum/HCSR04ultrasonic");
    http.addHeader("Content-Type", "application/json");
    
    
    StaticJsonDocument<20> tempDocument2;
    tempDocument2["status"] = ultraflag;
    Serial.println("ultraflagsend");
    Serial.println(ultraflag);
    
    char buffer2[20];
    serializeJson(tempDocument2, buffer2);
   int sensor3=http.PATCH(buffer2);
Serial.println(sensor3);
    
 
    http.end();
   // delay(500);
//    //}else{}
    //-----------------------------------water---------------------------------

////if(waterFlag==true){
    http.begin("https://ticket-resrevation-app.web.app/api/v1/DHT11hum/WSwater");
    http.addHeader("Content-Type", "application/json");
    
    
    StaticJsonDocument<20> tempDocument3;
    tempDocument3["status"] = waterFlag;
     Serial.println("waterflagsend");
    Serial.println(waterFlag);
    char buffer3[20];
    serializeJson(tempDocument3, buffer3);
   int sensor4=http.PATCH(buffer3);
Serial.println(sensor4);
    
 
    http.end();
    //delay(500);
//   // }else{}
    //----------------------------------gas-------------------------------
// // if(gasFlag==true){  
    http.begin("https://ticket-resrevation-app.web.app/api/v1/DHT11hum/MQ5gas");
    http.addHeader("Content-Type", "application/json");
    
    
    StaticJsonDocument<20> tempDocument4;
    tempDocument4["status"] = gasFlag;
    Serial.println("gasflagsend");
    Serial.println(gasFlag);
    char buffer4[20];
    serializeJson(tempDocument4, buffer4);
    int sensor5=http.PATCH(buffer4);
Serial.println(sensor5);
    
 
   http.end();
   // }else{}
   delay(200);
  //----------------------------------------recive---------------------------------------------
                                           //Make the request
//                                           
//  //HTTPClient http6;
  http.begin("https://ticket-resrevation-app.web.app/api/v1/smrtLed1/smrtLed1");
    int httpCode1 = http.GET();
    if (httpCode1 > 0) { //Check for the returning code
 
        String payload1 = http.getString();
        Serial.println(httpCode1);
        Serial.println(payload1);
        char json1[500];
        payload1.replace(" ","");
        payload1.replace("\n","");
        payload1.trim();
        //payload.remove(0,1);
        payload1.toCharArray(json1,500);
        StaticJsonDocument<200> doc1;
        deserializeJson(doc1,json1);
        bool status1=doc1["status"];
        Serial.println("led1"+String(status1));
        if (status1==true){digitalWrite(16, HIGH); 
//delay(100); 

      }else if(status1==false){
        digitalWrite(16, LOW); 
//delay(100);
}
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
     http.end();
     //================================led2================================================================
 http.begin("https://ticket-resrevation-app.web.app/api/v1/smrtLed1/smrtLed2");
    int httpCode2 = http.GET();                                        //Make the request
 
    if (httpCode2 > 0) { //Check for the returning code
 
        String payload2 = http.getString();
        Serial.println(httpCode2);
        Serial.println(payload2);
        char json2[500];
        payload2.replace(" ","");
        payload2.replace("\n","");
        payload2.trim();
        //payload.remove(0,1);
        payload2.toCharArray(json2,500);
        StaticJsonDocument<200> doc2;
        deserializeJson(doc2,json2);
        bool status2=doc2["status"];
        Serial.println("led2"+String(status2));
        if (status2==true){digitalWrite(17, HIGH); 
//delay(100); 

      }else if(status2==false){
        digitalWrite(17, LOW); 
//delay(100);
}
      }
 
    else {
      Serial.println("Error on HTTP request");
      
    }
     http.end();
//=======================================================led3================================================
http.begin("https://ticket-resrevation-app.web.app/api/v1/smrtLed1/smrtLed3");
    int httpCode3 = http.GET();                                        //Make the request
 
    if (httpCode3 > 0) { //Check for the returning code
 
        String payload3 = http.getString();
        Serial.println(httpCode3);
        Serial.println(payload3);
        char json3[500];
        payload3.replace(" ","");
        payload3.replace("\n","");
        payload3.trim();
        //payload.remove(0,1);
        payload3.toCharArray(json3,500);
        StaticJsonDocument<200> doc3;
        deserializeJson(doc3,json3);
        bool status3=doc3["status"];
        Serial.println("led3"+String(status3));
        if (status3==true){digitalWrite(21, HIGH); 
delay(100); 

      }else if(status3==false){
        digitalWrite(21, LOW); 
delay(100);}
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
     http.end();
    //--------------------------------------------------led4--------------------------------------------
 http.begin("https://ticket-resrevation-app.web.app/api/v1/smrtLed1/smrtLed4");
    int httpCode4 = http.GET();                                        //Make the request
 
    if (httpCode4 > 0) { //Check for the returning code
 
        String payload4 = http.getString();
        Serial.println(httpCode4);
        Serial.println(payload4);
        char json4[500];
        payload4.replace(" ","");
        payload4.replace("\n","");
        payload4.trim();
        //payload.remove(0,1);
        payload4.toCharArray(json4,500);
        StaticJsonDocument<200> doc4;
        deserializeJson(doc4,json4);
        bool status4=doc4["status"];
        Serial.println("led4"+String(status4));
        if (status4==true){digitalWrite(22, HIGH); 
delay(100); 

      }else if(status4==false){
        digitalWrite(22, LOW); 
delay(100);}
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
    http.end();
    }
    else{
    Serial.println("error in connection");
    }
    delay(3000);
     
}
