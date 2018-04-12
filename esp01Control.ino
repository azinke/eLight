#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include  <FS.h>

IPAddress local_IP(192,168,4,2);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
    
/* Device */

/* Network info */
const char *ssid = "e-light";
const char *password = "password";

ESP8266WebServer server(80);

uint8_t gpio = 3;
uint8_t state = 0;

String  getContentType(String filename);  //  convert the file  extension to  the MIME  type
bool  handleFileRead(String path);       //  send  the right file  to  the client  (if it  exists)

void setup() {
  pinMode(gpio, OUTPUT);
  /* Set module parameter */
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  
  SPIFFS.begin();

  server.on("/on", [](){
    digitalWrite(gpio, HIGH);
    server.send(200, "text/plain","OK");
  });

  server.on("/off", [](){
    digitalWrite(gpio, LOW);
    server.send(200, "text/plain","OK");
  });

  server.on("/query", [](){
    state = digitalRead(gpio);
    server.send(200, "text/plain", String(state));
  });

  server.onNotFound([](){
        if(!handleFileRead(server.uri())) { server.send(404,  "text/plain", "404: Not Found"); }
  });

  server.begin();
}

void loop() {
  server.handleClient();

}

String getContentType(String filename){ //  convert the file  extension to  the MIME  type
    if(filename.endsWith(".html"))  return  "text/html";
    else if(filename.endsWith(".css")) return  "text/css";
    else if(filename.endsWith(".js"))  return  "application/javascript";
    else if(filename.endsWith(".ico")) return  "image/x-icon";
    else if(filename.endsWith(".png")) return  "image/png";
    return "text/plain";
}

bool handleFileRead(String path){ //  send  the right file  to  the client  (if it  exists)
    if(path.endsWith("/"))  path  +=  "index.html";                 //  If  a folder  is  requested,  send  the index file
    String  contentType = getContentType(path);                     //  Get the MIME  type
    if(SPIFFS.exists(path)) {                                                       //  If  the file  exists
      File  file  = SPIFFS.open(path, "r");                                 //  Open  it
      size_t  sent = server.streamFile(file, contentType); //  And send  it  to  the client
      file.close();
      return  true;
    }
    return  false;
}
