#include <SoftwareSerial.h>
#include <stdlib.h>
#include <String.h>

String reset  = "AT+RST";
String twitterAPIKey = "B2K5PVPQTRMP4563";
String tweetURI = "/apps/thingtweet/1/statuses/update";
String thingSpeak = "api.thingspeak.com";
String urlEncoded = "application/x-www-form-urlencoded";
String cmd;

int sensorPin = 0;
SoftwareSerial ser(2,3); // RX, TX

void setup() {
  // put your setup code here, to run once:
 // enable debug serial
  Serial.begin(9600);
  // enable software serial
  ser.begin(9600);
  //AT+IPR=9600; set baud rate
  
  // reset ESP8266
  ser.println(reset);
}

void loop() {
   // read from tmp36 sensor
   int reading = analogRead(sensorPin);  
   float voltage = reading * 5.0;
   voltage /= 1024.0; 
   //Serial.print(voltage); 
   //Serial.println(" volts");
   float temperatureC = (voltage - 0.5) * 100 ;
   Serial.print(temperatureC); 
   Serial.println(" degrees C");
   if(temperatureC > 29){
    
    //create a POST request header structure
    String payload = postRequest(tweetURI,thingSpeak,urlEncoded,tempAlertTweet(temperatureC));
    
    //TCP connect 
    tcp_connect(cmd);
    
    // send data length 
  if(postStatus(cmd,payload)){
    
      // thingspeak needs at least 15 sec delay between updates
        delay(30000);
      }else{
        ser.println("AT+CIPCLOSE");
        // alert user
       Serial.println("AT+CIPCLOSE");
       }
       }     
  delay(5000);
   
}

 //TCP connection
    bool tcp_connect(String tcp_connect_cmd){
      // TCP connection
      tcp_connect_cmd = "AT+CIPSTART=\"TCP\",\"";
      tcp_connect_cmd += "184.106.153.149"; // api.thingspeak.com
      tcp_connect_cmd += "\",80";
      ser.println(tcp_connect_cmd);
      Serial.println(tcp_connect_cmd);
      //check for connection error
      if(ser.find("Error")||ser.find("closed")){
        Serial.println("AT+CIPSTART error");
        return;
      }
 }

  //POST request structure
 String postRequest(String url, String host, String content_type, String message){
   String postString = "POST " + url + " HTTP/1.1\r\n" +
                    "Host: "+ host + "\r\n" + 
                    "Content-Type: "+ content_type + "\r\n" +
                    "Connection: "+"close\r\n" +
                    "Content-Length: " + message.length()+"\r\n" +
                    "Cache-Control: " +"no-cache\r\n" +
                     + "\r\n" + message;
                     return postString;
                     }

 //tweet message + api key + temperature                    
String tempAlertTweet(float temp){
  //tweet message
  String tempAlert = "api_key="+twitterAPIKey +
                  "&status= It is getting hot... " + 
                  temp +" degrees Celcius ";
                  return tempAlert;
    }

//send payload to server via esp8266    
bool postStatus(String command, String message){
  command = "AT+CIPSEND=";
  command += String(message.length());
  Serial.println(command);
  ser.println(command);
  if(ser.find(">")){
      ser.print(message);
      Serial.print(message);
      return;
      }
}
