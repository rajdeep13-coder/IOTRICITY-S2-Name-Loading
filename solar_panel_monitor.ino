#include <Servo.h>

Servo horizontal;  // Horizontal axis servo
Servo vertical;    // Vertical axis servo

// Analog pins for LDRs
const int ldrTopLeft = A2;
const int ldrTopRight = A1;
const int ldrBottomLeft = A0;
const int ldrBottomRight = A3;

// Servo pins
const int horizontalPin = 5;
const int verticalPin = 6;

// Servo positions and sweep variables
int horizPos = 90;
int vertPos = 0;
int sweepStep = 1;
const int sweepRange = 60;  // Sweep ±60° from center
const int sweepDelay = 15;  // Speed of sweep
int horizDirection = 1;     // 1 for forward, -1 for reverse
int vertDirection = 1;

// Smoothing and calibration
const int numReadings = 10;
int readingsTL[numReadings], readingsTR[numReadings], readingsBL[numReadings], readingsBR[numReadings];
int readIndex = 0;
long totalTL = 0, totalTR = 0, totalBL = 0, totalBR = 0;
int avgTL = 0, avgTR = 0, avgBL = 0, avgBR = 0;
int maxLDR = 0;

// Tuning parameters
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
  // Smoothing logic
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

  // Check for optimal alignment (all LDRs high and uniform)
  if (maxLDR > lowLightThreshold && abs(avgTL - avgTR) < dynamicTolerance &&
      abs(avgTL - avgBL) < dynamicTolerance && abs(avgTL - avgBR) < dynamicTolerance) {
    horizPos = 90;
    vertPos = 0;
    horizontal.write(horizPos);
    vertical.write(vertPos);
    Serial.println("Optimal alignment detected - Panel set to flat");
  }
  // Low light condition: Start sweeping
  else if (avgTL < lowLightThreshold && avgTR < lowLightThreshold &&
           avgBL < lowLightThreshold && avgBR < lowLightThreshold) {
    // Sweep horizontal
    horizPos += sweepStep * horizDirection;
    if (horizPos <= 90 - sweepRange || horizPos >= 90 + sweepRange) {
      horizDirection = -horizDirection;  // Reverse direction
    }
    horizPos = constrain(horizPos, 0, 180);
    horizontal.write(horizPos);

    // Sweep vertical
    vertPos += sweepStep * vertDirection;
    if (vertPos <= 0 || vertPos >= 180) {
      vertDirection = -vertDirection;  // Reverse direction
    }
    vertPos = constrain(vertPos, 0, 180);
    vertical.write(vertPos);

    Serial.println("Low light - Sweeping to find light source");
  }
  // Normal tracking mode
  else {
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
  }
	
  int bl;
  int br;
  // Serial monitoring
  Serial.print("TL: "); Serial.print(avgTL);
  Serial.print(" | TR: "); Serial.print(avgTR);
  Serial.print(" | BL: "); Serial.print(bl);
  Serial.print(" | BR: "); Serial.print(br);
  Serial.print(" | Max LDR: "); Serial.print(maxLDR);
  Serial.print(" | Horiz Pos: "); Serial.print(horizPos);
  Serial.print(" | Vert Pos: "); Serial.println(vertPos);

  delay(sweepDelay);
}
