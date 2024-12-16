#define WINDOW_SIZE 100  // The number of readings in the sliding window

int sensorValues[4][WINDOW_SIZE];  // Array to store readings for 4 sensors
int currentIndex = 0;  // The current index for storing readings
long sum[4];  // Sums for each sensor
long sumOfSquares[4];  // Sum of squares for each sensor
double average[4];  // Averages for each sensor
double variance[4];  // Variance for each sensor

const int R1 = 1000;  // Resistance of R1 in ohms
const float V_REF = 5.0;  // Reference voltage of the Arduino

void setup() {
  Serial.begin(9600); // Set serial communication speed
  // Initialize arrays to 0
  for (int i = 0; i < 4; i++) {
    sum[i] = 0;
    sumOfSquares[i] = 0;
    average[i] = 0;
    variance[i] = 0;
    for (int j = 0; j < WINDOW_SIZE; j++) {
      sensorValues[i][j] = 0;
    }
  }
}

void loop() {
  // Read sensor values
  int sensorValue = analogRead(A0);
  int sensorValue1 = analogRead(A1);
  int sensorValue2 = analogRead(A2);
  int sensorValue3 = analogRead(A3);

  // Update the sliding window for each sensor
  updateSensorValues(0, sensorValue);
  updateSensorValues(1, sensorValue1);
  updateSensorValues(2, sensorValue2);
  updateSensorValues(3, sensorValue3);

  // Print the results for each sensor
  for (int i = 0; i < 4; i++) {
    Serial.print("LDR");
    Serial.print(i);
    Serial.print(" - Avg: ");
    Serial.print(average[i]);
    Serial.print(" - Variance: ");
    Serial.println(variance[i]);
  }

  // Detect erroneous sensor
  int erroneousSensor = detectErroneousSensor();
  if (erroneousSensor != -1) {
    Serial.print("Erroneous Sensor Detected: LDR");
    Serial.println(erroneousSensor);
  } else {
    Serial.println("No Erroneous Sensor Detected.");
  }

  // Calculate and display voltage and resistance range for sensor A0
  calculateAndDisplayVoltageAndResistance(sensorValue);

  delay(100);  // Adjust this delay as per your requirement
}

// Update the sliding window for a specific sensor
void updateSensorValues(int sensorIndex, int newValue) {
  // Subtract the old value from sum and sumOfSquares
  sum[sensorIndex] -= sensorValues[sensorIndex][currentIndex];
  sumOfSquares[sensorIndex] -= (long)sensorValues[sensorIndex][currentIndex] * sensorValues[sensorIndex][currentIndex];

  // Add the new value to sum and sumOfSquares
  sum[sensorIndex] += newValue;
  sumOfSquares[sensorIndex] += (long)newValue * newValue;

  // Store the new value in the sensor's sliding window
  sensorValues[sensorIndex][currentIndex] = newValue;

  // Move to the next index (circular array)
  currentIndex = (currentIndex + 1) % WINDOW_SIZE;

  // Calculate the average
  average[sensorIndex] = sum[sensorIndex] / (double)WINDOW_SIZE;

  // Calculate the variance using the formula:
  // Variance = (sum(x^2) / N) - (mean^2)
  double squaredSum = sumOfSquares[sensorIndex] / (double)WINDOW_SIZE;
  variance[sensorIndex] = squaredSum - (average[sensorIndex] * average[sensorIndex]);

  // Ensure variance is non-negative due to precision errors
  if (variance[sensorIndex] < 0) {
    variance[sensorIndex] = 0;
  }
}

// Function to detect erroneous sensor readings
int detectErroneousSensor() {
  double avgThreshold = 20.0;  // Threshold for average deviation
  double varThreshold = 30.0;  // Threshold for variance deviation

  for (int i = 0; i < 4; i++) {
    // Compute the average and variance of the other three sensors
    double otherAvg = 0.0;
    double otherVar = 0.0;
    for (int j = 0; j < 4; j++) {
      if (j != i) {
        otherAvg += average[j];
        otherVar += variance[j];
      }
    }
    otherAvg /= 3.0;
    otherVar /= 3.0;

    // Check if the current sensor deviates significantly
    if (abs(average[i] - otherAvg) > avgThreshold || abs(variance[i] - otherVar) > varThreshold) {
      return i;  // Return the index of the erroneous sensor
    }
  }
  return -1;  // No erroneous sensor detected
}

// Function to calculate and display voltage and resistance
void calculateAndDisplayVoltageAndResistance(int sensorValue) {
  // Calculate the voltage
  float voltage = (sensorValue / 1023.0) * V_REF;

  // Calculate the resistance of the photoresistor using voltage divider formula
  float resistance = (V_REF / voltage - 1) * R1;

  Serial.print("Measured Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Photoresistor Resistance: ");
  Serial.print(resistance);
  Serial.println(" ohms");
} 
