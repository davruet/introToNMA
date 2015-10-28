/*
 * Captive portal, rudimentary web content management system.
 * 
*/
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>


#define FILE_WRITE "w"
#define DEBUG_SERIAL Serial
#define DNS_PORT 53
#define PASSWORD_ARG "password"
#define SSID_ARG "ssid"
#define SSID_PREFIX "web+site"
#define WATCHDOG_TIME 30000

IPAddress apIP(192, 168, 1, 1);
const char* host = "web";

ESP8266WebServer server(80);
DNSServer dnsServer;

static bool hasFS = false;
File uploadFile;
bool editMode = false;

void returnOK() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "");
}

void returnFail(String msg) {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
  DEBUG_SERIAL.println(msg);
}

bool loadFromSPIFFS(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.html";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html") || path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";

  File dataFile = SPIFFS.open(path.c_str(), "r");
  
  if (!dataFile)
    return false;

  server.sendHeader("Cache-Control", "max-age=120");
  if (server.hasArg("download")) dataType = "application/octet-stream";

  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
    DEBUG_SERIAL.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void handleUpdate(){ 
    HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      DEBUG_SERIAL.setDebugOutput(true);
      WiFiUDP::stopAll();
      DEBUG_SERIAL.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){ //true to set the size to the current progress
        DEBUG_SERIAL.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      DEBUG_SERIAL.setDebugOutput(false);
    }
    yield();
}

void handleFileUpload(){
  if (!requestAccess()) return;
  
  if (server.uri() == "/update"){
    handleUpdate();
  } else if (server.uri() == "/edit"){

    HTTPUpload& upload = server.upload();
    if(upload.status == UPLOAD_FILE_START){
      WiFiClient client = server.client();
      //server.client().stopAllExcept(&client);
      if(SPIFFS.exists((char *)upload.filename.c_str())) SPIFFS.remove((char *)upload.filename.c_str());
      uploadFile = SPIFFS.open(upload.filename.c_str(), FILE_WRITE);
      DEBUG_SERIAL.print("Upload: START, filename: "); DEBUG_SERIAL.println(upload.filename);
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(uploadFile) uploadFile.write(upload.buf, upload.currentSize);
      DEBUG_SERIAL.print("Upload: WRITE, Bytes: "); DEBUG_SERIAL.println(upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END){
      if(uploadFile) uploadFile.close();
      DEBUG_SERIAL.print("Upload: END, Size: "); DEBUG_SERIAL.println(upload.totalSize);
    }
  }
  yield();
  
}



void deleteRecursive(String path){
  Dir dir = SPIFFS.openDir((char *)path.c_str());
  if(!SPIFFS.exists(path)){
    return;
  }

  while(dir.next()) {
    
    SPIFFS.remove(dir.fileName());
    yield();
  }

}

void handleDelete(){
  if (!requestAccess()) return;
  if(server.args() == 0) return returnFail("BAD ARGS");
  String path = server.arg(0);
  if(path == "/" || !SPIFFS.exists((char *)path.c_str())) {
    DEBUG_SERIAL.print("Bad path: ");
    DEBUG_SERIAL.println(path);
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
  returnOK();
}

void handleCreate(){
  if (!requestAccess()) return;
  if(server.args() == 0) return returnFail("BAD ARGS");
  String path = server.arg(0);
  if(path == "/" || SPIFFS.exists((char *)path.c_str())) {
    returnFail("BAD PATH");
    return;
  }

  if(path.indexOf('.') > 0){
    File file = SPIFFS.open((char *)path.c_str(), FILE_WRITE);
    if(file){
      file.write(0);
      file.close();
    }
  } else {
    // SPIFFS.mkdir((char *)path.c_str()); FIXME
  }
  returnOK();
}

void printDirectory() {
  if (!requestAccess()) return;
  if(!server.hasArg("dir")) return returnFail("BAD ARGS");
  String path = server.arg("dir");
  if(path != "/" && !SPIFFS.exists((char *)path.c_str())) return returnFail("BAD PATH");
  Dir dir = SPIFFS.openDir((char *)path.c_str());

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/json", "");
  

  server.sendContent("{\"files\":[");
  int i = 0;
  long usedSize = 0;
  while (dir.next()){
    String fullpath = dir.fileName();
    
    File entry = dir.openFile("r");
    if (!entry)
      break;
    usedSize += entry.size();
    entry.close();
    String output;
    
    if (i > 0)
      output = ',';

    output += "{\"path\":\"";
    output += fullpath;
    output += "\"";
    output += "}";
    server.sendContent(output);
   
    i++;
 }
 server.sendContent("], \"flashSize\":");
 server.sendContent(String(ESP.getFlashChipRealSize() - (ESP.getSketchSize() + ESP.getFreeSketchSpace())));
 server.sendContent(", \"usedSize\":");
 server.sendContent(String(usedSize));
 server.sendContent("}");
}
/** Handle root or redirect to captive portal */
void handleRoot() {
  if (captivePortalRedirect()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  loadFromSPIFFS("/");
}
/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean captivePortalRedirect() {
  
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(host)+".local")) {
    DEBUG_SERIAL.println(server.hostHeader());
    send302();
    return true;
  }
  
  return false;
}

void send302(){
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}


boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

boolean dropRequest(){
  if (server.hostHeader() && (
    server.hostHeader().endsWith("artic.edu")
    || server.hostHeader().endsWith("push.apple.com"))){
    return true;
  }
  return false;
}

void handleNotFound() {
  
  if (server.method() != HTTP_GET){
    server.client().stop();
    return;
  }
  
  if (dropRequest()){
    DEBUG_SERIAL.println("Request dropped.");
    server.sendHeader("Location", "http://192.168.1.5");
    server.send(301, "text-plain", ""); 
    server.client().stop();
    return;
  }
  if (captivePortalRedirect() // If captive portal redirect instead of displaying the error page.
    || hasFS && loadFromSPIFFS(server.uri()) )  {  // or if file exists on SPIFFS
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
  DEBUG_SERIAL.println("Sent 404.");
}


void generate_204(){
  //send302();
  server.send(302, "text/plain", "");
  server.client().stop();
}


void handleAnalogRead(){
  /*if (server.hasArg(SAMPLE_TIME_ARG)){
    set
    String pinStr = server.arg("pin");
  }*/
  String response = "{ \"sensorValue\":";
  response += analogRead(A0);
  response += "}";
  server.send(200, "text/json", response);
}

void handleDigitalWrite(){
  if(!server.hasArg("pin")) return returnFail("Pin not specified.");
  if(!server.hasArg("value")) return returnFail("Value not specified.");
  String pinStr = server.arg("pin");
  String valueStr = server.arg("value");
  int pin = atoi(pinStr.c_str());
  int value = atoi(valueStr.c_str());
  
  if (value == 0) value = LOW;
  else value = HIGH;
  
  if (pin >= 1 && pin <= 15){
    
    pinMode(pin, OUTPUT);
    
    digitalWrite(pin, value);
    server.send(200, "text/plain", "");
  } else {
    server.send(400, "text/plain", "Invalid pin specified.");
  }
  
}
void connect(){
  if(!server.hasArg(PASSWORD_ARG)) return returnFail("NOPW");
  if(!server.hasArg(SSID_ARG)) return returnFail("NOSSID");
  String password = server.arg(PASSWORD_ARG);
  String ssid = server.arg(SSID_ARG);
  WiFi.begin(ssid.c_str(), password.c_str());
}

void handleConnectInfo(){
  int status = WiFi.status();
  String message = "{\"status\":";
  message += status;
  message += "}";
  server.send(200, "text/json", message);
}

void handleSetSSID(){
  if (!requestAccess()) return;
  if(!server.hasArg(SSID_ARG)) return returnFail("NOSSID");
   String ssid = server.arg(SSID_ARG);
   DEBUG_SERIAL.print("Setting SSID to ");  
   DEBUG_SERIAL.println(ssid);
   String message = "{\"status\":\"ok\"}";
   server.send(200, "text/json", message);
   WiFi.softAP(ssid.c_str());
   
}

void gpio0(){
  if (!editMode) DEBUG_SERIAL.println("Enabled edit mode."); // only print this once, mind the bounces
  editMode = true; 
}

bool requestAccess(){
  if (editMode) return true;
  else {
    server.send(403, "text/plain", "Access Denied: updates to server are not enabled. Connect GPIO0 pin to ground to enable.");
    return false;
  }
}

void startAP(){
   String ssid = WiFi.SSID();
   if (!ssid){
      DEBUG_SERIAL.println("SSID not set.  Generating new...");
      ssid = String(SSID_PREFIX);
      ssid += ESP.getChipId(); 
   } else {
      DEBUG_SERIAL.print("Read existing SSID.");
      DEBUG_SERIAL.println(ssid);
   }
   WiFi.softAP(ssid.c_str());
}



void initAP(){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  startAP();

  
  DEBUG_SERIAL.println("Started AP");
 
}
void setup(void){

  pinMode(0, INPUT_PULLUP);
  attachInterrupt(0, gpio0, FALLING);

  if (SPIFFS.begin()){
     DEBUG_SERIAL.println("Mounted FS.");
     //DEBUG_SERIAL.print(SPIFFS.info());
     hasFS = true;
     Dir dir = SPIFFS.openDir("/");
  
    while (dir.next()) {
      DEBUG_SERIAL.print("FILE: ");
      DEBUG_SERIAL.print(dir.fileName());
      File f = dir.openFile("r");
      DEBUG_SERIAL.println(f.size());
      f.close();
    }
  }
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.setDebugOutput(true);
  DEBUG_SERIAL.print("\n");
  
  initAP();
  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  
  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on("/edit", HTTP_POST, [](){ returnOK(); });
  server.on("/analogRead", HTTP_GET, handleAnalogRead);
  server.on("/digitalWrite", HTTP_GET, handleDigitalWrite);
  //server.on("/generate_204", generate_204);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", HTTP_GET, handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/", HTTP_GET, handleRoot);
  server.on("/connect", HTTP_GET, connect); // Connect to an AP;
  server.on("/connectInfo", HTTP_GET, handleConnectInfo); // Get connection info.
  server.on("/setSSID", HTTP_GET, handleSetSSID); // SET SSID
  server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    });
  
  server.onNotFound(handleNotFound);
  server.onFileUpload(handleFileUpload);
 
  server.begin();
  DEBUG_SERIAL.println("HTTP server started");

  ESP.wdtEnable(WATCHDOG_TIME);
}

void loop(void){
  dnsServer.processNextRequest();
  server.handleClient();
  ESP.wdtFeed();
}
