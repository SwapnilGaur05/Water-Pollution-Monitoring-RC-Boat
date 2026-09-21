#include <SoftwareSerial.h>

// ---------------- Turbidity Sensor ----------------
#define TURBIDITY_PIN A1    // Analog pin for turbidity sensor

float turbidityValue = 0.0;
String status = "Unknown";

// ---------------- Bluetooth (HC-05) ----------------
#define BT_TX 10           // Arduino TX → HC-05 RX
#define BT_RX 11           // Arduino RX → HC-05 TX
SoftwareSerial Bluetooth(BT_RX, BT_TX); // RX, TX

// ---------------- Motor Driver (L9110S) ----------------
// Left motor
#define LEFT_A 5    // IN1
#define LEFT_B 6    // IN2
// Right motor
#define RIGHT_A 9   // IN3
#define RIGHT_B 8   // IN4

void setup() {
  Serial.begin(9600);
  Bluetooth.begin(9600);

  // Motor pins setup
  pinMode(LEFT_A, OUTPUT);
  pinMode(LEFT_B, OUTPUT);
  pinMode(RIGHT_A, OUTPUT);
  pinMode(RIGHT_B, OUTPUT);

  Serial.println("Turbidity + Bluetooth + L9110S Motor System Initialized...");
  Bluetooth.println("Boat System Ready. Send commands: F,B,L,R,S");
}

// ---------------- Motor Control Functions ----------------

// MOVE FORWARD
void moveForward() {
  digitalWrite(LEFT_A, HIGH);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_A, HIGH);
  digitalWrite(RIGHT_B, LOW);
}

// MOVE BACKWARD
void moveBackward() {
  digitalWrite(LEFT_A, LOW);
  digitalWrite(LEFT_B, HIGH);
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(RIGHT_B, HIGH);
}

// TURN LEFT
void turnLeft() {
  // Left motor STOP
  digitalWrite(LEFT_A, LOW);
  digitalWrite(LEFT_B, LOW);

  // Right motor FORWARD
  digitalWrite(RIGHT_A, HIGH);
  digitalWrite(RIGHT_B, LOW);
}

// TURN RIGHT
void turnRight() {
  // Right motor STOP
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(RIGHT_B, LOW);

  // Left motor FORWARD
  digitalWrite(LEFT_A, HIGH);
  digitalWrite(LEFT_B, LOW);
}

void stopMotors() {
  digitalWrite(LEFT_A, LOW);
  digitalWrite(LEFT_B, LOW);
  digitalWrite(RIGHT_A, LOW);
  digitalWrite(RIGHT_B, LOW);
}

// ---------------- Main Loop ----------------
void loop() {
  // --- 1️⃣ Turbidity Reading ---
  int turbidityRaw = analogRead(TURBIDITY_PIN);
  turbidityValue = turbidityRaw * 5.0 / 1023.0;

  if (turbidityValue > 1.8) {
    status = "Bad (High turbidity)";
  } else {
    status = "Good";
  }

  // Send to Serial + Bluetooth
  Serial.print("Turbidity: ");
  Serial.print(turbidityValue, 2);
  Serial.print(" V | Status: ");
  Serial.println(status);

  Bluetooth.print("Turbidity: ");
  Bluetooth.print(turbidityValue, 2);
  Bluetooth.print(" V | Status: ");
  Bluetooth.println(status);

  // --- 2️⃣ Bluetooth Command Control ---
  if (Bluetooth.available()) {
    char command = Bluetooth.read();

    switch (command) {
      case 'F':
        moveForward();
        Serial.println("Command: Forward");
        break;

      case 'B':
        moveBackward();
        Serial.println("Command: Backward");
        break;

      case 'L':
        turnLeft();
        Serial.println("Command: Left");
        break;

      case 'R':
        turnRight();
        Serial.println("Command: Right");
        break;

      case 'S':
        stopMotors();
        Serial.println("Command: Stop");
        break;
    }
  }

  delay(1000); // you can reduce to 100 for smoother control
}
