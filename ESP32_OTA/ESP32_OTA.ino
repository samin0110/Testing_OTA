#include <WiFi.h>
#include <Update.h>
#include <HTTPClient.h>

#define fota_ssid       "E-ARTKEY_4G"
#define fota_password   "Connect@Eartkey"
#define OTA_URL         "https://raw.githubusercontent.com/TanishKunthe/Testing_OTA/main/ESP32_OTA/ESP32_OTA.bin"
#define VERSION_URL     "https://raw.githubusercontent.com/TanishKunthe/Testing_OTA/main/version.txt"  // Version file URL

#define CURRENT_VERSION "1.0"  // Set the current firmware version

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("Testing the FoTa Github Cloning");

  WiFi.begin(fota_ssid, fota_password);
  Serial.println("Connecting to WiFi...");

  Serial.print("Version : ");   Serial.println(CURRENT_VERSION);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  // Fetch version from server and compare
  String newVersion = fetchVersion();
  if (newVersion != "" && newVersion > CURRENT_VERSION) {
    Serial.println("New Version Available: " + newVersion);
    if (performOTA()) {
      Serial.println("OTA update successful, restarting...");
      esp_restart();
    } else {
      Serial.println("OTA update failed.");
    }
  } else {
    Serial.println("No update required. Already running latest version: " + String(CURRENT_VERSION));
  }
}

String fetchVersion() {
  WiFiClientSecure client;
  client.setInsecure();  // Bypass SSL certificate verification
  HTTPClient http;
  http.begin(client, VERSION_URL);
  int httpCode = http.GET();

  Serial.print("Version Fetch HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    String version = http.getString();
    version.trim();  // Remove any newline characters
    Serial.println("Fetched Version: " + version);
    return version;
  } else {
    Serial.println("Failed to fetch version.");
    return "";
  }
}

bool performOTA() {
  delay(2000);
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, OTA_URL);

  int httpCode = http.GET();
  Serial.print("OTA HTTP Code: ");
  Serial.println(httpCode);

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    if (contentLength > 0) {
      if (!Update.begin(contentLength)) {
        Update.printError(Serial);
        return false;
      }

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

      return true;  // OTA successful
    }
  } else {
    Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }

  return false;  // OTA failed
}

void loop() {
}
