#include <Arduino.h>
#include <Hash.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ganti sesuai nama wifi/hotspot
const char *ssid = "SMK N 2 YOGYAKARTA";
const char *password = "amsangaji47"; //Ganti password wifi apa

#define DHTPIN 14 // Pin DHT di D5
#define RelayPIN 13 // Pin Relay di D7
#define ButtonIP 15 // Pin Button IP di D8
#define ButtonMode 16 // Pin Button Mode di D8

// Pilih Sensor DHT yang digunakan
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
//#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float f = 0.0;
float h = 0.0;
float reamur = 0;
float kelvin = 0;
float rankie = 0;

int mode = 0;
int x = 0;
int y = 0;
int off = 0;
int varA = 0;
int varB = 0;

// Create AsyncWebserver object on port 80
AsyncWebServer server(80);

// The value will quickly become too large for an int to store
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0; // will store last time DHT was updated
unsigned long lastmillis = 0;

// Updates DHT readings, every 10 seconds
const long interval = 1000;

const char index_html[] PROGMEM =R"rawliteral(
<!DOCTYPE html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
  integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
    font-family: Comic Sans MS;
    display: inline-flex;
    margin: 0px auto;
    text-align: center;
    }
    body {
  background-color: #1a1a1a; 
  color: #d4d4d4; 
}

button {
  background-color: #246826; 
  color: white; 
  border: none;
  padding: 10px 20px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 50px 0px 2px 2px;
  cursor: pointer;
  border-radius: 4px;
  transition: background-color 0.3s ease;
}

button:hover {
  background-color: #1c3b1d; 
}

a {
  color: #4CAF50; 
  text-decoration: none;
}

.container {
  display: flex;
  flex-wrap: wrap;
  gap: 20px; 
  justify-content: center; 
}
h1 {
    color: #74fc78;
}

.box {
  background-color: #1a1a1a; 
  color: #d4d4d4; 
  border: 1px solid #4CAF50; 
  padding: 20px;
  flex: 1 1 calc(50% - 20px); 
  text-align: center;
  border-radius: 10px;
}

@media (max-width: 600px) {
  .box {
    flex: 1 1 100%; 
  }
}

a:hover {
  color: #45a049; 
}


  </style>
</head>
<body>
  <p>
  <i class="fas fa-thermoneter-half" style="color:059e8a";"></i>
    <h1>Temperature List</h1>
  </p>
  <div class="container">
    <div class="box">
        <span class="dht-labels">Celcius</span><br>
        <span class="temperature1">%TEMPERATURE1%</span>
        <sup class="units">&deg;C</sup>
    </div>
    <div class="box">
        <span class="dht-labels">Fahrenheit </span><br>
        <span id="temperature2">%TEMPERATURE2% </span>
        <sup class="units">&deg;K</sup>
    </div>
    <div class="box">
        <span class="dht-labels">Reamur </span><br>
        <span id="reamur">%REAMUR% </span>
        <sup class="units">&deg;R</sup>
    </div>
    <div class="box">
        <span class="dht-labels">Kelvin </span><br>
        <span id="kelvin">%KELVIN% </span>
        <sup class="units">&deg;Ra</sup>
    </div>
    <div class="box">
        <i class="fas fa-tint" style="color:#00add6;"></i>
        <span class="dht-labels">Humidity </span><br>
        <span id="humidity">%HUMIDITY% </span>
        <sup class="units">%</sup>
    </div>
  </div>

  <button class="button" onclick="toggleRelay('on')">Relay ON</button>
  <button class="button button-off" onclick="toggleRelay('off')">Relay OFF</button>
  <h4>Made By Kelompok 6</h4>
</body>

<script>

function toggleRelay(state) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "/" + state, true);
    xhttp.send();
}

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperature1").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/temperature1", true);
    xhttp.send();
}, 10000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperature2").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/temperature2", true);
    xhttp.send();
}, 10000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("kelvin").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/kelvin", true);
    xhttp.send();
}, 10000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("reamur").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/reamur", true);
    xhttp.send();
}, 10000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("rankie").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/rankie", true);
    xhttp.send();
}, 10000);

setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("humidity").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/humidity", true);
    xhttp.send();
}, 10000);
</script>
</html>)rawliteral";

String processor(const String &var){
  if (var=="TEMPERATURE1") {
    return String(t);
  }else if (var == "TEMPERATURE2"){
    return String(f);
  }else if (var == "KELVIN"){
    return String(kelvin);
  }else if (var == "REAMUR"){
    return String(reamur);
  }else if (var == "RANKIE"){
    return String(rankie);
  }else if (var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  pinMode(RelayPIN, OUTPUT);
  pinMode(ButtonIP, INPUT);
  pinMode(ButtonMode, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Serial port
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connetcing to WiFi");
  lcd.setCursor(0,0);
  lcd.print("Connecting........ ");
  lcd.setCursor(0, 1);
  lcd.print("........To WiFi ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    x = digitalRead(ButtonMode);
    y = digitalRead(ButtonIP);

    if (y == HIGH && x == HIGH) {
      break;
    }
  }

  // digitalWrite(LED_BUILTIN, HIGH);

  // Print ESP8266 Local IP Address
  Serial.println();
  Serial.println(WiFi.localIP());

   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Existing routes
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(t).c_str());
  });

  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(f).c_str());
  });

  server.on("/kelvin", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(kelvin).c_str());
  });

  server.on("/reaumur", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(reamur).c_str());
  });

  server.on("/rankie", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(rankie).c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // New routes for controlling relay
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RelayPIN, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("ON ");
    request->send(200, "text/plain", "Relay ON");
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(RelayPIN, LOW);
    lcd.setCursor(0, 1);
    lcd.print("OFF");
    request->send(200, "text/plain", "Relay OFF");
  });

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP Server :....");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(1000);
  lcd.clear();

  // Start Server
  server.begin();
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    float newT = dht.readTemperature();
    float newF = dht.readTemperature(true);

    if (isnan(newT)){
      Serial.println("Failed to read from DHT sensor!");
    }else{
      t = newT;
      f = newF;
      reamur = t * 0.8;
      kelvin = t + 273.15;
      rankie  = t * 1.8 + 491.67;
      Serial.println(t);
      if (mode == 0){
        lcd.setCursor(7,0);
        lcd.print(t);
        lcd.print(" C");
      }else if (mode == 1){
        lcd.setCursor(7,0);
        lcd.print(f);
        lcd.print(" F");
      }else if (mode == 2){
        lcd.setCursor(7,0);
        lcd.print(reamur);
        lcd.print(" R");
      }else if (mode == 3){
        lcd.setCursor(7,0);
        lcd.print(kelvin);
        lcd.print(" K");
      }else if (mode == 4){
        lcd.setCursor(7,0);
        lcd.print(rankie);
        lcd.print(" Ra");
      }
    }
    // Read humdity
    float newH = dht.readHumidity();
    if (isnan(newH)){
      Serial.println("Failed to read from DHT sensor");
    }else{
      h = newH;
      Serial.println(h);
      lcd.setCursor(7,1);
      lcd.print(h);
      lcd.print(" %");
    }
    lcd.setCursor(0,0);
    lcd.print("Suhu :");
    lcd.setCursor(0, 1);
    lcd.print("Hum  :");
  }
  x = digitalRead(ButtonMode);
  y = digitalRead(ButtonIP);
  off = 0;

  if (y == HIGH && x == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Relay :");
    lcd.setCursor(0, 1);
    lcd.print("Manual");
    delay(1000);
    varB = 1;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Relay :");
    lcd.setCursor(0, 1);
    if (t > 29) {
      lcd.print("ON");
    } else {
      lcd.print("OFF");
    }

    while (varB == 1) {
      x = digitalRead(ButtonMode);
      y = digitalRead(ButtonIP);

      if (y == HIGH && x == HIGH) {
        varB = 0;
      } else if (x == HIGH) {
        digitalWrite(RelayPIN, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("ON ");
      } else if (y == HIGH) {
        digitalWrite(RelayPIN, LOW);
        lcd.setCursor(0, 1);
        lcd.print("OFF");
      }
      delay(50);
    }

    lcd.setCursor(0, 0);
    lcd.print("Relay :");
    lcd.setCursor(0, 1);
    lcd.print("AUTO");
    delay(1000);
    lcd.clear();
  }else if (y == HIGH){
    if (varA == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Mode : ");
      mode++;
      
      if(mode == 5){
        mode = 0;
      }

      lcd.setCursor(0, 1);
      if (mode == 0){
        lcd.print("Celcius");
      } else if (mode == 1){
        lcd.print("Fahrenheit");
      }else if (mode == 2){
        lcd.print("Reamur");
      }else if (mode == 3){
        lcd.print("Kelvin");
      }else if (mode == 4){
        lcd.print("Rankie");
      }
      delay(500);
      lcd.clear();
      varA=1;
      lastmillis = millis();
    } else if (varA == 1 && currentMillis - lastmillis > 3000) {
      lcd.clear();
      lcd.print("Restart.......");
      delay(500);
      ESP.restart();
    }
    
  }
  else if (x == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP Server : ");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(1000);
    lcd.clear();
  }

  else if (y == LOW) {
    varA = 0;
  }

  if (t >29) {
    digitalWrite(RelayPIN, HIGH);
  } else {
    digitalWrite(RelayPIN, LOW);
  }
}

