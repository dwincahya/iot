#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize I2C LCD

const char *ssid = "my"; // Replace with your WiFi name
const char *password = "andreeenya2"; // Replace with your WiFi password

// Pin
#define GasSensorPin A0 // Pin for gas sensor
#define Buzz D4 

AsyncWebServer server(80);

// Nada dan durasi melodi lagu "Golden Hour"
int melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; // Contoh nada (frekuensi dalam Hz)
int noteDurations[] = {4, 4, 4, 4, 4, 4, 4, 4}; // Durasi setiap nada (4 adalah seperempat nada)

void playMelody() {
  int length = sizeof(melody) / sizeof(melody[0]);
  for (int thisNote = 0; thisNote < length; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(Buzz, melody[thisNote], noteDuration);
    delay(noteDuration * 1.30);
    noTone(Buzz);
  }
}

void setup() {
  pinMode(Buzz, OUTPUT);
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  // Serve HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String ipAddress = WiFi.localIP().toString();
    request->send(200, "text/html", 
      "<!DOCTYPE html>"
      "<html lang='en'>"
      "<head>"
      "<meta charset='UTF-8'>"
      "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
      "<title>Gas Sensor Monitor</title>"
      "<style>"
      "body { font-family: Arial, sans-serif; background: linear-gradient(to bottom right, #B1B2FF, #AAC4FF, #EEF1FF); text-align: center; margin: 0; height: 100vh; display: flex; align-items: center; justify-content: center; }"
      ".container { display: flex; flex-direction: column; align-items: center; padding: 20px; background-color: white; border-radius: 15px; width: 250px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); }"
      ".title { font-size: 24px; font-weight: bold; color: #333; margin-bottom: 30px; }"
      ".value-box { width: 100px; height: 100px; background-color: #f2f2f2; border-radius: 10px; display: flex; align-items: center; justify-content: center; font-size: 24px; color: #333; margin-bottom: 10px; }"
      ".ip-address { font-size: 18px; color: #333; margin-bottom: 10px; margin-top: 5px;}"
      ".name-list { font-size: 18px; color: #333; list-style: none; padding: 0; margin-top: 15px; text-align: left;}"
      ".name-list li { margin: 5px 0; }"
      "</style>"
      "</head>"
      "<body>"
      "<div class='container'>"
      "<div class='title'>Gas Value</div>"
      "<div class='value-box' id='value-box'>0</div>"
      "<h2>Kelompok 6</h2>"
      "<ul class='name-list'>"
      "<li>1. Andree Agustian Wenas</li>"
      "<li>2. Dwi Nur Cahya</li>"
      "<li>3. Maulana Faqih Al Faruq</li>"
      "<li>4. Naufal Zaky Ramadhan</li>"
      "</ul>"
      "</div>"
      "<script>"
      "setInterval(() => {"
      "  fetch('/gas').then(response => response.json()).then(data => {"
      "    const gasValue = Math.min(Math.max(data.gasValue, 0), 1023);"
      "    document.getElementById('value-box').innerText = gasValue;"
      "  });"
      "}, 1000);"
      "</script>"
      "</body>"
      "</html>");
  });

  // Endpoint to get gas value
  server.on("/gas", HTTP_GET, [](AsyncWebServerRequest *request){
    int gasValue = analogRead(GasSensorPin); 
    request->send(200, "application/json", String("{\"gasValue\":") + gasValue + "}");
  });

  server.begin();
}

void loop() {
  int gasValue = analogRead(GasSensorPin); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas Value: ");
  lcd.print(gasValue); 
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  // Check for gas leak
  if (gasValue > 500) {
    playMelody(); // Mainkan melodi jika nilai gas > 500
  } else {
    noTone(Buzz); 
  }

  delay(1000); 
}
