#include <Wire.h>
#include "DHT.h"

// Define Sensor Pins
#define MQ3_PIN A0      // MQ3 Sensor connected to Analog Pin A0
#define MQ135_PIN A1    // MQ135 Sensor connected to Analog Pin A1
#define DHTPIN D2       // DHT22 Sensor connected to Digital Pin D2

// Define Sensor Types
#define DHTTYPE DHT22

// Initialize DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

// Calibration Constants for MQ3 and MQ135
// *IMPORTANT:* These values are placeholders. You must replace them with your calibrated values.
const float MQ3_VOLTAGE_CLEAN_AIR = 0.4; // Example value in volts
const float MQ3_SLOPE = -0.2;            // Example slope from calibration

const float MQ135_VOLTAGE_CLEAN_AIR = 0.3; // Example value in volts
const float MQ135_SLOPE = -0.1;            // Example slope from calibration

// Conversion Constants from Acetone to Glucose
// *IMPORTANT:* Replace this with your actual conversion formula.
const float ACETONE_TO_GLUCOSE_FACTOR = 0.5; // Example factor

// Averaging Parameters
const int NUM_SAMPLES = 10; // Number of samples for averaging

// Variables to Store Readings
float mq3Readings[NUM_SAMPLES];
float mq135Readings[NUM_SAMPLES];
float acetoneReadings[NUM_SAMPLES];
float glucoseLevels[NUM_SAMPLES];
float averageMQ3 = 0;
float averageMQ135 = 0;
float averageAcetone = 0;
float averageGlucose = 0;

// Variables for Averaging
int sampleIndex = 0;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB
  }
  Serial.println("Sensor Initialization...");

  // Initialize DHT Sensor
  dht.begin();

  // Initialize MQ Sensors
  pinMode(MQ3_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);

  // Initialize Readings Arrays
  for (int i = 0; i < NUM_SAMPLES; i++) {
    mq3Readings[i] = 0;
    mq135Readings[i] = 0;
    acetoneReadings[i] = 0;
    glucoseLevels[i] = 0;
  }

  Serial.println("Initialization Complete.");
}

void loop() {
  // Read Raw Sensor Values
  int mq3Raw = analogRead(MQ3_PIN);
  int mq135Raw = analogRead(MQ135_PIN);
  
  // Convert Raw Values to Voltage (Assuming 3.3V Reference)
  float mq3Voltage = mq3Raw * (3.3 / 4095.0);   // STM32F302R8 ADC is 12-bit: 0-4095
  float mq135Voltage = mq135Raw * (3.3 / 4095.0);
  
  // Convert Voltages to Acetone Concentration (ppm) using Calibration Equations
  float acetoneMQ3 = (mq3Voltage - MQ3_VOLTAGE_CLEAN_AIR) / MQ3_SLOPE;
  float acetoneMQ135 = (mq135Voltage - MQ135_VOLTAGE_CLEAN_AIR) / MQ135_SLOPE;
  
  // Average Acetone from Both Sensors
  float acetoneAverage = (acetoneMQ3 + acetoneMQ135) / 2.0;
  
  // Convert Acetone to Glucose Levels
  float glucoseLevel = acetoneAverage * ACETONE_TO_GLUCOSE_FACTOR; // Example conversion
  
  // Read Humidity and Temperature from DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Store Readings in Arrays for Averaging
  mq3Readings[sampleIndex] = acetoneMQ3;
  mq135Readings[sampleIndex] = acetoneMQ135;
  acetoneReadings[sampleIndex] = acetoneAverage;
  glucoseLevels[sampleIndex] = glucoseLevel;

  // Update Sample Index
  sampleIndex++;
  if (sampleIndex >= NUM_SAMPLES) {
    sampleIndex = 0;
  }

  // Compute Averages
  averageMQ3 = 0;
  averageMQ135 = 0;
  averageAcetone = 0;
  averageGlucose = 0;

  for (int i = 0; i < NUM_SAMPLES; i++) {
    averageMQ3 += mq3Readings[i];
    averageMQ135 += mq135Readings[i];
    averageAcetone += acetoneReadings[i];
    averageGlucose += glucoseLevels[i];
  }

  averageMQ3 /= NUM_SAMPLES;
  averageMQ135 /= NUM_SAMPLES;
  averageAcetone /= NUM_SAMPLES;
  averageGlucose /= NUM_SAMPLES;

  // Display Readings
  Serial.println("======================================");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("Average MQ3 Acetone: ");
  Serial.print(averageMQ3, 2);
  Serial.println(" ppm");

  Serial.print("Average MQ135 Acetone: ");
  Serial.print(averageMQ135, 2);
  Serial.println(" ppm");

  Serial.print("Average Acetone: ");
  Serial.print(averageAcetone, 2);
  Serial.println(" ppm");

  Serial.print("Estimated Glucose Level: ");
  Serial.print(averageGlucose, 2);
  Serial.println(" mg/dL");

  Serial.println("======================================");
  Serial.println();

  // Wait before next reading
  delay(2000); // Adjust delay as needed
}