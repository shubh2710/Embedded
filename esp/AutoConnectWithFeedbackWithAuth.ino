#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"
#include <ESP8266mDNS.h>

boolean isDebug=false;
ESP8266WebServer server(80);

String tokenValue[2];

void configModeCallback (WiFiManager *myWiFiManager) {
  printDebugEsp(String("Entered config mode"));
  //printDebugEsp(String(WiFi.softAPIP()));
  //if you used auto generated SSID, print it
  printDebugEsp(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect()) {
    printDebugEsp(String("failed to connect and hit timeout"));
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
    //if you get here you have connected to the WiFi
  printDebugEsp(String("connected to wifi... :)"));
   if (MDNS.begin("myesp0")) {
     printDebugEsp(String("MDNS myesp0 started"));
  }
  server.on("/device/command", handleRoot);  //Associate handler function to path
  server.begin();                           //Start server
  printDebugEsp(String("HTTP server started"));
  MDNS.addService("http", "tcp", 80);
}

void loop() {
    MDNS.update();
    server.handleClient();  
}
void handleRoot() {
    String message = "Number of args received:";
    message += server.args();
    String command;
    String auth;
    for (int i = 0; i < server.args(); i++) {
      message += "Arg " + (String)i + "=> ";
      message += server.argName(i) + ": ";
      message += server.arg(i) + "\n";
      if(server.argName(i)=="command"){
          command=server.arg(i);
      }
      if(server.argName(i)=="auth"){
          auth=server.arg(i);
      }
    }
   printDebugEsp(String("command: "+command));
   printDebugEsp(String("auth: "+auth));
    if(validation(auth)){
      if(command.startsWith("cmd_esp0_")){
        printSerial(command);
        server.send(200, "application/json", "{\"reuqest\":\"OK\",\"code\":\"200\",\"error\":\"\"}");
      }else{
         server.send(400, "application/json", "{\"reuqest\":\"Bad Request\",\"code\":\"400\",\"error\":\"Request is for wrong esp server \"}");
        }
    }else{
      server.send(401, "application/json", "{\"reuqest\":\"Authentication error\",\"code\":\"401\",\"error\":\"auth error \"}");
      }
}
boolean validation(String token){
  HTTPClient http;
   printDebugEsp(String("token: "+token));
  split(token);
  http.begin("http://"+tokenValue[1]+"/v1/device/"+tokenValue[0]);
  int httpCode = http.GET();
  String payload = http.getString();
  printDebugEsp(payload);
  http.end();
  
  if(payload=="OK"){
    return true;
   }else{ return false;}
}
void printDebugEsp(String data){
  if(isDebug){
    Serial.println(data);
   }
}
void split(String str){                                               
  int index=0;
  int count=0;
  while(str.indexOf("@")>0){
    index=str.indexOf("@");
    String wrd=str.substring(0,index);
   tokenValue[count]=wrd;
    count++;
    str=str.substring(index+1,str.length());
  }
  tokenValue[count]=str;
}
void printSerial(String data){
  Serial.println(data);
}
