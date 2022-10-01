/*
  Codigo fonte para um projeto escolar denominado MAPEAR

  - Este código serve, para reiniciar um sensor ESP8266, quando, o website
    https://sensor-web.vercel.app, não recebe dados, este, tem sido um problema
    recorrente, sendo que não podemos alterar o código fonte do projeto original
  
  Modificado a 01/10/2022
  Por Rúben Costa
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>



// Network settings
 char* ssid = "HOME_NET";
 char* password = "419CB60221";

// Endpoint, on production replace this to http://192.168.1.2/restart
const char* endpoint = "http://mapear.free.beeceptor.com";

// declare the server and putting it to listening on port 80
ESP8266WebServer server(80);

// handleRoot function to handle every api call done that doesn't specify
// a path or a Method


void handleRoot() {
  server.send(
    200,
    "text/plain",
    "Greetings!\n\n\nDone by: https://github.com/rubencosta13"
  );
}

// handleErrors function catches the erros, and nicely, formats them to 
// properly display them to the users
void handleErrors() {
  server.send(
    404,
    "text/plain",
    "Ooops, the route you are trying to access is not valid!"
  );
}

void setup() {
  // Initializing Serial console as 9600 baud rate.
  Serial.begin(9600);

  // Initializing Wifi connection.
  Serial.println("Connecting to WiFi....");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  Serial.println("WiFi connected!");
  Serial.println("Arduino's Ip address is: ");
  Serial.print(WiFi.localIP());
  // Connected to wifi.
  
  // Initiating the server and MDNS
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS Responder started!");
  }
  server.enableCORS(true);
  server.on("/", handleRoot);
  
  server.on("/restart", []() {
    // Starting the HTTP client to perform requests
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, endpoint);
      // Making the post request and storing the response code
      // on a httpResponseCode variable to ensure that the 
      // request was sent successfully
      String httpRequestData = "";
      int httpResponseCode = http.POST(httpRequestData);
      if (httpResponseCode != 200) {
        // Post request wasn't successful
        Serial.println("Something went wrong!");
        String message = "Got an error: " + httpResponseCode;
        server.send(httpResponseCode, "text/plain", message);
        http.end();
      } else {
        // Successful post request
        server.send(200, "text/plain", "Restart Successful");
        http.end();
      }
    }
    
    
  });

  server.onNotFound(handleErrors);

  // Putting the server to listen
  server.begin();
  Serial.println("HTTP Server started!");


 
}


void loop() {
  server.handleClient();
  MDNS.update();
}
