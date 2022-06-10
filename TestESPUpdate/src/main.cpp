#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
 
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include <WiFiClientSecureBearSSL.h>

#include <time.h>

#include <Arduino_JSON.h>
 
#ifndef APSSID
#define APSSID "Wergelt"
#define APPSK  "KrevniMsta2.0"
#endif
 
ESP8266WiFiMulti WiFiMulti;
 
#define FIRMWARE_VERSION "0.2"
 
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}
 
void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}
 
void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}
 
void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}
 
void setup() {
 
  Serial.begin(115200);
  Serial.setDebugOutput(true);
 
  Serial.println();
  Serial.println();
  Serial.println();
 
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
 
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);
 
  Serial.print(F("Firmware version "));
  Serial.println(FIRMWARE_VERSION);
  delay(2000);
}

void processUpdate(){
      // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
 
    WiFiClient client;
    //BearSSL::WiFiClientSecure client;
 
    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
 
    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);
 
    ESPhttpUpdate.rebootOnUpdate(false); // remove automatic update
 
    Serial.println(F("Update start now!"));
 
    // t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://192.168.1.70:3000/firmware/httpUpdateNew.bin");
    // Or:
    //client.addHeader("Authorization","Bearer bearer_token");
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.88.103", 8080, "/v1/device/update", FIRMWARE_VERSION);
 
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        Serial.println(F("Retry in 10secs!"));
        delay(10000); // Wait 10secs
        break;
 
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;
 
      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        delay(1000); // Wait a second and restart
        ESP.restart();
        break;
    }
  }
}

/*
void downloadTestFile(){
  WiFiClient client;
  HTTPClient http;

    String url = String(UPDATE_URL) + response.substring(4);
    //url = http://123.123.123.123/SPIFFS/test2.htm
    String file_name=response.substring(response.lastIndexOf('/'));
    //file_name = test2.htm
    Serial.println(url);
    File f = SPIFFS.open(file_name, "w");
    if (f) {
      http.begin(url);
      http.addHeader(F(""), "");
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          http.writeToStream(&f);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      f.close();
    }
    http.end();
}
*/

String getAccessToken(){
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    HTTPClient https;
      // Your Domain name with URL path or IP address with path
    client->setInsecure();
    if(https.begin(*client, "https://keycloak-dev.jirismolak.com/auth/realms/Demo-Realm/protocol/openid-connect/token")){

      // Specify content-type header
      https.addHeader("Content-Type", "application/x-www-form-urlencoded");

      // Data to send with HTTP POST
      //String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";

      // Send HTTP POST request
      int httpResponseCode = https.POST("client_id=springboot-microservice&client_secret=HTYgC6vHNPGs7Wm6Xf3e1vh8xBMRkCC8&grant_type=password&username=test&password=test");

      if (httpResponseCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpResponseCode);

        // file found at server
        if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
          JSONVar myObject = JSON.parse(payload);
          // JSON.typeof(jsonVar) can be used to get the type of the var
          if (JSON.typeof(myObject) == "undefined") {
            Serial.println("Parsing input failed!");
            https.end();
            return String("");
          }
          Serial.print("JSON object = ");
          Serial.println(myObject);

          // myObject.keys() can be used to get an array of all the keys in the object
          JSONVar keys = myObject.keys();

          for (int i = 0; i < keys.length(); i++) {
            JSONVar value = myObject[keys[i]];
            Serial.print(keys[i]);
            Serial.print(" = ");
            Serial.println(value);
            https.end();
          }
            Serial.println("-------------------------------------------------------------------------------");
            Serial.println(JSON.stringify(myObject["access_token"]));
            return JSON.stringify(myObject["access_token"]);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpResponseCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  return String("");
}
 
void loop() {
  //processUpdate();
  getAccessToken();
  delay(10000);
}