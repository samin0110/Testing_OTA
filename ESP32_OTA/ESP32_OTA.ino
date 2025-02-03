#include <WiFi.h>
#include <Update.h>
#include <HTTPClient.h>

#define fota_ssid       "E-ARTKEY_4G"
#define fota_password   "Connect@Eartkey"
#define OTA_URL "https://raw.githubusercontent.com/TanishKunthe/Testing_OTA/main/ESP32_OTA/ESP32_OTA.bin" // Change to your actual HTTP URL
//https://github.com/TanishKunthe/Testing_OTA/ESP32_OTA.bin

#define ledPin          2

void setup() {
  Serial.begin(115200);
  Serial.println("Testing the FoTa Github Cloning");

  WiFi.begin(fota_ssid, fota_password);
  Serial.println("Connecting to WiFi...");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  // Start OTA
  if (performOTA()) {
    Serial.println("OTA update successful, restarting...");
    esp_restart();
  } else {
    Serial.println("OTA update failed.");
  }
}

bool performOTA() {

  WiFiClientSecure client;
  client.setInsecure();  // Bypass SSL certificate verification
  HTTPClient http;
  http.begin(client, OTA_URL);

  http.begin(client, OTA_URL); // Specify the URL
  int httpCode = http.GET(); // Make the request
  Serial.print("HTTP Code : "); Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    if (contentLength > 0) {
      // Start the OTA process
      if (!Update.begin(contentLength)) {
        Update.printError(Serial);
        return false;
      }

      // Write data to flash
      size_t written = Update.writeStream(http.getStream());
      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully.");
      } else {
        Update.printError(Serial);
        return false;
      }

      if (!Update.end()) {
        Update.printError(Serial);
        return false;
      }

      return true; // OTA successful
    }
  } else {
    Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }

  return false; // OTA failed
}

void loop() {
  digitalWrite(ledPin, true);
  delay(2000);
  digitalWrite(ledPin, false);
}
