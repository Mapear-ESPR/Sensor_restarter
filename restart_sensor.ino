/*
  This source code is meant to restart a sensor placed at Escola secundaria pinhal do rei,
  when our website: https://sensor-web.vercel.app, can't retrieve data, this has been a 
  frequent problem with the project, so to counter that, I've put together this Arduino Script
  to make the sensor restart when one of the following conditions are met:
  
  - Webpage has no data to serve
  - Auto restart every 7 days to clear cache and to improve the sensor's reliability
  
  
  Last modification: 04/10/2022
  Author: Rúben Costa
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <CronAlarms.h>

// Network settings
 char* ssid = "===YOUR_NETWORK_SSID_HERE===";
 char* password = "===YOUR_NETWORK_PASSWORD_HERE===";

// Endpoint, in production replace this to http://192.168.1.2/restart
const char* endpoint = "===ENDPOINT===";

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

void restartSensor() {
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
  Serial.println("Configuring cron job to auto restart");
  Serial.println("Cronjob Settings: Every Sunday at midnight it should invoke the function restartSensor");
  Cron.create("0 0 * * 0", restartSensor, false);
  Serial.println("Done!");

  
  // Initiating the server and MDNS
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS Responder started!");
  }
  server.enableCORS(true);
  server.on("/", handleRoot);
  server.on("/restart", restartSensor);
  server.onNotFound(handleErrors);
  // Putting the server to listen
  server.begin();
  Serial.println("HTTP Server started!");
}


void loop() {
  server.handleClient();
  MDNS.update();
}
