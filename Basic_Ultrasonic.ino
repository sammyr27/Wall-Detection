#define TRIG_PIN 32 // Trig pin connected to D32
#define ECHO_PIN 33 // Echo pin connected to D33

// #define RED_LED 25 // Red LED connected to D25, delete this later or comment it out

const float distanceThreshold1 = 30.48; // This is the distance that will set off haptic feedback, currently it is at 30.48 cm (1 foot)

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT); // Trigger pin for ultrasonic sensor
  pinMode(ECHO_PIN, INPUT);  // Echo pin for ultrasonic sensor
  pinMode(RED_LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // send trigger signal
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure time for echo to return
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  // Convert time to distance
  float distance = duration * 0.034 / 2;

  // Print out distance (cm) to serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  /* // LED testing for thresholds for now, replace with haptic feedback in PSOC
  // Could add more thresholds for haptic feedback intensity
  if (distance <= distanceThreshold1) {
    // distance less than or equal to threshold 1, high haptic feedback intensity/ Red LED ON
    digitalWrite(RED_LED, HIGH);
  }
  else {
    // distance greater than threshold, haptic feedback / Red LED OFF
    digitalWrite(RED_LED, LOW);
  } */ 


  delay(100); // 100 ms delay 

}
