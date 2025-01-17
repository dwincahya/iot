#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define trig D4
#define echo D3
#define relay D1

long duration;
int distance;

// You should get Auth Token in the Blynk App.
char auth[] = "3i7kVLtg7g2zjO0WW57IdVSYoS1B5XcT";
char ssid[] = "enn"; // your ssid
char pass[] = "manasayatau";   // your password

BlynkTimer timer;

void setup()
{
    // Debug console  
    pinMode(trig, OUTPUT); // Sets the trigPin as an Output
    pinMode(echo, INPUT);  // Sets the echoPin as an Input
    pinMode(relay, OUTPUT); // Sets the relay as an Output
    Serial.begin(9600);

    // Server and port configuration
    const char server[] = "iot.smk2-yk.sch.id"; // Your server address
    const uint16_t port = 8080; // Your server port

    // Connect to Blynk
    Blynk.begin(auth, ssid, pass, server, port);

    // Setup a function to be called every second
    timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sendSensor() {
  digitalWrite(trig, LOW); // Makes trigPin low
  delayMicroseconds(2);    // 2 micro second delay

  digitalWrite(trig, HIGH); // trigPin high
  delayMicroseconds(10);    // trigPin high for 10 micro seconds
  digitalWrite(trig, LOW);  // trigPin low

  duration = pulseIn(echo, HIGH); // Read echo pin, time in microseconds
  distance = duration * 0.034 / 2; // Calculating actual/real distance

  Serial.print("Distance = ");    // Output distance on Arduino serial monitor
  Serial.println(distance);

  if (distance < 50) {
    digitalWrite(relay, HIGH);
  } else {
    digitalWrite(relay, LOW);
  }

  Blynk.virtualWrite(V0, distance);
  delay(1000);
}
