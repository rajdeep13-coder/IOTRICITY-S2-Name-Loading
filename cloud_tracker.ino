//Enhanced version using WiFi-enabled adruino. Sends LDR + sensor data to FastAPI cloud ML API, receives predicted angles, and moves servos accordingly. Includes sweeping fallback when cloud is unavailable.
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

// ---------------- WiFi Config ----------------
const char* ssid = "XXX";
const char* password = "XXX";

// Cloud FastAPI endpoint
const char* serverName = "http://aot.edu.in/predict";   // FastAPI endpoint

// ---------------- Servo Setup ----------------
Servo horizontal;  
Servo vertical;    
const int horizontalPin = 5;
const int verticalPin = 6;

// ---------------- LDR Pins -------------------
const int ldrTopLeft = A2;
const int ldrTopRight = A1;
const int ldrBottomLeft = A0;
const int ldrBottomRight = A3;

// ---------------- Servo Angles ----------------
int horizPos = 90;
int vertPos = 90;

// ---------------- Tuning ----------------
const int lowLightThreshold = 200;
const int sweepDelay = 20;

void setup() {
  Serial.begin(115200);

  // Attach servos
  horizontal.attach(horizontalPin);
  vertical.attach(verticalPin);
  horizontal.write(horizPos);
  vertical.write(vertPos);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi Connected!");
}

void loop() {
  // ---------------- Read Sensors ----------------
  int tl = analogRead(ldrTopLeft);
  int tr = analogRead(ldrTopRight);
  int bl = analogRead(ldrBottomLeft);
  int br = analogRead(ldrBottomRight);

  int avgLDR = (tl + tr + bl + br) / 4;

  // ---------------- Check Light ----------------
  if (avgLDR < lowLightThreshold) {
    // Low light fallback: sweeping mode
    horizPos = (horizPos + 2) % 180;
    vertPos = (vertPos + 1) % 180;
    horizontal.write(horizPos);
    vertical.write(vertPos);
    Serial.println(" Low light detected - sweeping mode");
  } 
  else {
    // ---------------- Send Data to Cloud ----------------
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/json");

      // Create JSON payload
      String jsonPayload = "{";
      jsonPayload += "\"ldr_tl\":" + String(tl) + ",";
      jsonPayload += "\"ldr_tr\":" + String(tr) + ",";
      jsonPayload += "\"ldr_bl\":" + String(bl) + ",";
      jsonPayload += "\"ldr_br\":" + String(br) + ",";
      jsonPayload += "\"horiz_pos\":" + String(horizPos) + ",";
      jsonPayload += "\"vert_pos\":" + String(vertPos) + ",";
      jsonPayload += "\"sun_az\":180,";   // placeholder, can calculate if needed
      jsonPayload += "\"sun_el\":45";     // placeholder, can calculate if needed
      jsonPayload += "}";

      int httpResponseCode = http.POST(jsonPayload);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(" Cloud Response: " + response);

        // ---------------- Parse JSON ----------------
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, response);
        if (!error) {
          float pred_horiz = doc["pred_horiz"];
          float pred_vert = doc["pred_vert"];

          // ---------------- Move Servos ----------------
          horizPos = constrain((int)pred_horiz, 0, 180);
          vertPos = constrain((int)pred_vert, 0, 180);

          horizontal.write(horizPos);
          vertical.write(vertPos);

          Serial.print("➡ Servo moved to H:");
          Serial.print(horizPos);
          Serial.print(" V:");
          Serial.println(vertPos);
        }
      } else {
        Serial.println(" Error in POST request, fallback to sweeping");
      }
      http.end();
    }
  }

  delay(sweepDelay);
}
