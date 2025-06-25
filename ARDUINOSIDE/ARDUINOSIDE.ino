#include <Wire.h> // Keep Wire for other I2C sensors if you add them later
#include <OneWire.h>
#include <DallasTemperature.h>

// Pins
#define DS18B20_PIN 2         // Data pin for DS18B20
#define PRESSURE_SENSOR_PIN A0 // Analog pin for 40kpa pressure sensor (e.g., A0)

// Sensor objects
OneWire oneWire(DS18B20_PIN);
DallasTemperature tempSensor(&oneWire);

// Timing
unsigned long lastPrintTime = 0;

// Variables for sensor readings
int currentSystolicBP = 0;
int currentDiastolicBP = 0;

void setup() {
  Serial.begin(115200); // IMPORTANT: Match this baud rate with your ESP32 sender's Serial2.begin()
  delay(1000);

  tempSensor.begin();
  Serial.println("✅ DS18B20 ready");

  Serial.println("✅ 40kpa Pressure Sensor (Analog) ready");

  // randomSeed(analogRead(A1)); // No longer needed if we're not generating pure random values
}

void loop() {
  unsigned long now = millis();

  // Send data to ESP32 every 1 second
  if (now - lastPrintTime > 1000) {
    lastPrintTime = now;

    // --- Read DS18B20 Temperature ---
    tempSensor.requestTemperatures();
    float tempC = tempSensor.getTempCByIndex(0);
    if (tempC == -127.00) { // DallasTemperature returns -127.00 if sensor not found or error
      Serial.println("Error: Could not read temperature sensor!");
      tempC = 0.0; // Default to 0 or some error value
    }

    // --- Read 40kpa Pressure Sensor (Analog) ---
    int analogPressure = analogRead(PRESSURE_SENSOR_PIN); // Read raw analog value (0-4095 for ESP32, 0-1023 for Uno)

    // --- CALIBRATION IS CRUCIAL HERE ---
    // The 'map' function below is a simple linear conversion.
    // YOU MUST CALIBRATE YOUR SENSOR to determine the relationship
    // between its raw analog output and actual pressure (e.g., mmHg).
    // This typically involves:
    // 1. Applying known pressures to the sensor.
    // 2. Recording the corresponding analog readings.
    // 3. Deriving a formula (linear, polynomial, lookup table)
    //    to convert analogRead to actual pressure.

    // Example mapping:
    // Assuming your 40kpa sensor, when connected to a cuff,
    // outputs 0 for 0mmHg and 4095 for 200mmHg (this is just an example!)
    // For actual blood pressure, you'd need the whole inflation/deflation cycle.

    // If you're just trying to show *some* value based on the sensor's static output:
    // Let's assume a simplified scenario where the sensor gives a higher
    // analog value for higher "effective" pressure, and you want to
    // map that to a BP range.

    // Determine the min and max raw analog readings you expect from your sensor
    // under "normal" operating conditions for BP, then map them.
    // For a 40kpa sensor (approx 300 mmHg), let's say:
    // Min analog value for the BP range (e.g., when cuff is low pressure)
    // Max analog value for the BP range (e.g., when cuff is high pressure)
    int analogMin = 500;  // Adjust based on your sensor's idle/low pressure reading
    int analogMax = 3500; // Adjust based on your sensor's max expected reading during BP

    // Map the analog value to a representative blood pressure range.
    // THIS IS STILL A SIMPLIFICATION, NOT A MEDICAL MEASUREMENT.
    currentSystolicBP = map(analogPressure, analogMin, analogMax, 90, 160); // Example target range
    currentDiastolicBP = map(analogPressure, analogMin, analogMax, 60, 100); // Example target range

    // Ensure values stay within reasonable bounds (optional, but good practice)
    currentSystolicBP = constrain(currentSystolicBP, 70, 200);
    currentDiastolicBP = constrain(currentDiastolicBP, 40, 120);

    // If analogPressure is outside your calibrated range, map might produce unexpected results.
    // Consider adding checks for analogPressure < analogMin or > analogMax
    // and handle them (e.g., print "Sensor out of range" or clamp values).

    // --- End Pressure Sensor Reading ---

    // Send data to ESP32 (ESP-NOW Sender) via Serial
    // Format: "T:XX.X,BP:YYY/ZZZ"
    Serial.print("T:");
    Serial.print(tempC, 1); // Temperature with 1 decimal place
    Serial.print(",BP:");
    Serial.print(currentSystolicBP);
    Serial.print("/");
    Serial.println(currentDiastolicBP); // Newline at the end
  }
}