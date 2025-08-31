#include <Servo.h>

Servo horizontal; 
Servo vertical;    

const int ldrTopLeft = A2;
const int ldrTopRight = A1;
const int ldrBottomLeft = A0;
const int ldrBottomRight = A3;
const int horizontalPin = 5;
const int verticalPin = 6;

int horizPos = 90;
int vertPos = 90;

const int numReadings = 10;  
int readingsTL[numReadings], readingsTR[numReadings], readingsBL[numReadings], readingsBR[numReadings];
int readIndex = 0;
long totalTL = 0, totalTR = 0, totalBL = 0, totalBR = 0;
int avgTL = 0, avgTR = 0, avgBL = 0, avgBR = 0;
int maxLDR = 0;  

const float scaleFactor = 0.03;  
const int lowLightThreshold = 200;  
const int deadZone = 5; 

void setup() {
  horizontal.attach(horizontalPin);
  vertical.attach(verticalPin);
  horizontal.write(horizPos);
  vertical.write(vertPos);
  Serial.begin(9600);
  Serial.println("Solar Tracker Initialized");

  for (int i = 0; i < numReadings; i++) {
    readingsTL[i] = 0;
    readingsTR[i] = 0;
    readingsBL[i] = 0;
    readingsBR[i] = 0;
  }
}

void loop() {
  totalTL = totalTL - readingsTL[readIndex];
  totalTR = totalTR - readingsTR[readIndex];
  totalBL = totalBL - readingsBL[readIndex];
  totalBR = totalBR - readingsBR[readIndex];

  readingsTL[readIndex] = analogRead(ldrTopLeft);
  readingsTR[readIndex] = analogRead(ldrTopRight);
  readingsBL[readIndex] = analogRead(ldrBottomLeft);
  readingsBR[readIndex] = analogRead(ldrBottomRight);

  totalTL = totalTL + readingsTL[readIndex];
  totalTR = totalTR + readingsTR[readIndex];
  totalBL = totalBL + readingsBL[readIndex];
  totalBR = totalBR + readingsBR[readIndex];

  readIndex = (readIndex + 1) % numReadings;
  avgTL = totalTL / numReadings;
  avgTR = totalTR / numReadings;
  avgBL = totalBL / numReadings;
  avgBR = totalBR / numReadings;
  
  int currentMax = max(avgTL, max(avgTR, max(avgBL, avgBR)));
  if (currentMax > maxLDR) maxLDR = currentMax;

  int avgTop = (avgTL + avgTR) / 2;
  int avgBottom = (avgBL + avgBR) / 2;
  int avgLeft = (avgTL + avgBL) / 2;
  int avgRight = (avgTR + avgBR) / 2;
  int dynamicTolerance = max(20, maxLDR / 50);  
  if (maxLDR > lowLightThreshold && abs(avgTL - avgTR) < dynamicTolerance &&
      abs(avgTL - avgBL) < dynamicTolerance && abs(avgTL - avgBR) < dynamicTolerance) {
    horizPos = 90;
    vertPos = 90;
    horizontal.write(horizPos);
    vertical.write(vertPos);
    Serial.println("Optimal alignment detected - Centered servos");
  } else {
    int horizDiff = avgLeft - avgRight;
    if (abs(horizDiff) > dynamicTolerance) {
      horizPos += int(horizDiff * scaleFactor);
      horizPos = constrain(horizPos, 0, 180);
      if (abs(horizPos - 90) > deadZone) horizontal.write(horizPos);
    }
    
    int vertDiff = avgTop - avgBottom;
    if (abs(vertDiff) > dynamicTolerance) {
      vertPos += int(vertDiff * scaleFactor);
      vertPos = constrain(vertPos, 0, 180);
      if (abs(vertPos - 90) > deadZone) vertical.write(vertPos);
    }

    if (avgTL < lowLightThreshold && avgTR < lowLightThreshold &&
        avgBL < lowLightThreshold && avgBR < lowLightThreshold) {
      horizPos = 90;
      vertPos = 90;
      horizontal.write(horizPos);
      vertical.write(vertPos);
      Serial.println("Low light detected - Resetting to center");
    }
  }
  
  Serial.print("TL: "); Serial.print(avgTL);
  Serial.print(" | TR: "); Serial.print(avgTR);
  Serial.print(" | BL: "); Serial.print(avgBL);
  Serial.print(" | BR: "); Serial.print(avgBR);
  Serial.print(" | Max LDR: "); Serial.print(maxLDR);
  Serial.print(" | Horiz Pos: "); Serial.print(horizPos);
  Serial.print(" | Vert Pos: "); Serial.println(vertPos);

  delay(50); 
}
