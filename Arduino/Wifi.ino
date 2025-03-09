#include <WiFi.h>
const char* ssid = "Temp";
const char* password = "esp32board";
WiFiServer server(80);

//desired temperature
float temperature = 25;

float realTemp = 27;
int recentInstruction = 0;

void ConnectionHandler(WiFiClient client){
  Serial.println("New Client Connected");
  String currentLine = "";
  while (client.connected()){
    if(client.available()){
      char c = client.read();
      if (c == '\n'){
        if(currentLine.length() == 0)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML><html>");
          client.println("<h1>ESP32 Remote Control</h1>");
          client.print("<p>Current temperature: ");
          client.print(temperature);
          client.println("</p>");
          client.println("<form action=\"/set\">");
          client.println("Set temperature: <input type=\"text\" name=\"temp\">");
          client.println("<input type=\"submit\" value=\"Update\">");
          client.println("</form>");
          client.println("</html>");
          break;
        }
        else{
          if(currentLine.startsWith("GET /set?")){
            Serial.println("Request: " + currentLine);

            //"GET /set?temp=VALUE HTTP/1.1" is received
            int startIndex = currentLine.indexOf("p=");
            int endIndex = currentLine.indexOf(" HTTP");
            if(startIndex != -1 && endIndex != -1){
              startIndex += 2;
              String tempStr = currentLine.substring(startIndex, endIndex);
              Serial.println("string to convert to float: |" + tempStr + "|");
              temperature = tempStr.toFloat();
              Serial.print("Temperature updated to: ");
              Serial.println(temperature);
            }
          }
          currentLine="";
        }
      }
      else if(c != '\r'){
        currentLine+=c;
      }
    }
  }
  delay(100);
  client.stop();
  Serial.println("Client disconnected");
}

void onWiFiDisconnected(WiFiEvent_t event, arduino_event_info_t info){
  Serial.print("Disconnected from Wifi, reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Attempting to reconnect");
  //WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
}

void setup(){
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2,HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.onEvent(onWiFiDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  server.begin();

}

void loop(){
  WiFiClient client = server.available();
  if (client){
    ConnectionHandler(client);
  }

  if(realTemp < temperature-0.5 && recentInstruction != 1){
    //do something to increase realTemperature
    Serial.println("raising temperature");
    recentInstruction = 1;
  }
  else if(realTemp > temperature+0.5 && recentInstruction != 2){
    //do something to reduce real temperature
    Serial.println("reducing temperature");
    recentInstruction = 2;
  }
  else if(temperature-0.1<realTemp && realTemp<temperature+0.1 && recentInstruction != 0){
    //turn off
    recentInstruction = 0;
    Serial.println("acceptable temperature, shutting down");
  }

  //for real functionality should be replaced with real temperature readings
  //recent instructions imitate real world changes after a given instruction
  if(recentInstruction == 1){
    realTemp+=0.01;
  }
  else if(recentInstruction == 2){
    realTemp-=0.01;
  }

  //display change on connected device
  if(recentInstruction != 0){
    Serial.println(realTemp);
  }

  delay(200);
}
