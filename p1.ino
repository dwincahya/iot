#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char *ssid = "IOT5H";
const char *password = "vicineko";

#define DHTPIN D5
#define RelayPIN D7
#define ButtonIP D0
#define ButtonMode D8

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

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
int relayStatus = 0;  
bool automode = true;

AsyncWebServer server(80);

unsigned long previousMillis = 0;
unsigned long lastmilis = 0;

const long interval = 1000;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
        integrity="sha384-fnm0CqbTlWIlj8LyTjo7mOUStjsKC4p0pQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
        html {
            font-family: Comic Sans MS;
            margin: 0;
            text-align: center;
        }

        body {
            padding: 1px;
            padding-bottom: 10px;
            background-color: #f4f4f9;
        }

        h2 {
            font-size: 3.0rem;
            margin-bottom: 20px;
        }

        .container {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            gap: 10px;
        }

        .data-box {
            border: 2px solid #ddd;
            border-radius: 8px;
            padding: 20px;
            width: 140px;
            background-color: #fff;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        .data-box i {
            font-size: 2rem;
            margin-bottom: 10px;
            display: block;
        }

        .data-box .label {
            font-size: 1.5rem;
            margin-top: 10px;
            display: block;
        }

        .data-box .value {
            font-size: 2rem;
        }

        .data-box .units {
            font-size: 1rem;
            color: #888;
        }

        .buttons {
            margin-top: 20px;
            background-color: #0056b3;
            padding: 1rem;
            border-radius: 8px;
        }

        .buttons h4 {
            margin-bottom: 10px;
            color: white;
        }

        .auto-button {
            width: 200px;
            background-color: #ffae00;
            color: white;
            font-size: 1.2rem;
            height: 50px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-bottom: 10px;
        }

        .auto-button:hover {
            background-color: #ff8c00;
        }

        .control-buttons {
            display: flex;
            justify-content: center;
            gap: 10px;
        }

        .control-buttons button {
            width: 100px;
            height: 50px;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 1.2rem;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .control-buttons .on-button {
            background-color: #00ff15;
        }

        .control-buttons .off-button {
            background-color: #ff0000;
        }

        .control-buttons .on-button:hover {
            background-color: #00cc00;
        }

        .control-buttons .off-button:hover {
            background-color: #cc0000;
        }

        @media (max-width: 600px) {
            .buttons {
                padding: 0.5rem;
            }

            .auto-button,
            .control-buttons button {
                width: 100%;
                font-size: 1rem;
                height: 40px;
            }
        }
    </style>
</head>

<body>
    <h2>ESP8266 DHT Server</h2>
    <h4>Design By Oreo</h4>
    <div class="container">
        <div class="data-box">
            <i class="fas fa-thermometer-half" style="color:#059e8a;"></i>
            <span class="label">Celcius</span>
            <span class="value" id="temperature1">%TEMPERATURE1%</span>
            <span class="units">&deg;C</span>
        </div>
        <div class="data-box">
            <span class="label">Fahrenheit</span>
            <span class="value" id="temperature2">%TEMPERATURE2%</span>
            <span class="units">&deg;F</span>
        </div>
        <div class="data-box">
            <span class="label">Reamur</span>
            <span class="value" id="reamur">%REAMUR%</span>
            <span class="units">&deg;R</span>
        </div>
        <div class="data-box">
            <span class="label">Kelvin</span>
            <span class="value" id="kelvin">%KELVIN%</span>
            <span class="units">&deg;K</span>
        </div>
        <div class="data-box">
            <span class="label">Rankie</span>
            <span class="value" id="rankie">%RANKIE%</span>
            <span class="units">&deg;Ra</span>
        </div>
        <div class="data-box">
            <i class="fas fa-tint" style="color:#00add6;"></i>
            <span class="label">Humidity</span>
            <span class="value" id="humidity">%HUMIDITY%</span>
            <span class="units">%</span>
        </div>
    </div>
    <div class="buttons">
        <h4>Relay Control</h4>
        <button class="auto-button" onclick="toggleRelay('auto')">Auto Mode</button>
        <div class="control-buttons">
            <button class="on-button" onclick="toggleRelay('on')">Turn On</button>
            <button class="off-button" onclick="toggleRelay('off')">Turn Off</button>
        </div>
    </div>
</body>
<script>
    function toggleRelay(action) {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/" + action, true);
        xhttp.send();
    }

    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("temperature1").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/temperature1", true);
        xhttp.send();
    }, 10000);

    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("temperature2").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/temperature2", true);
        xhttp.send();
    }, 10000);
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("kelvin").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/kelvin", true);
        xhttp.send();
    }, 10000);
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("reamur").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/reamur", true);
        xhttp.send();
    }, 10000);
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("rankie").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/rankie", true);
        xhttp.send();
    }, 10000);
    setInterval(function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("humidity").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/humidity", true);
        xhttp.send();
    }, 10000);
</script>

</html>

)rawliteral";

String processor(const String &var) {
  if (var == "TEMPERATURE1") {
    return String(t);
  } else if (var == "TEMPERATURE2") {
    return String(f);
  } else if (var == "KELVIN") {
    return String(kelvin);
  } else if (var == "REAMUR") {
    return String(reamur);
  } else if (var == "RANKIE") {
    return String(rankie);
  } else if (var == "HUMIDITY") {
    return String(h);
  }
  return String();
}

void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Relay : ");
    
    lcd.setCursor(0, 1);
    if (relayStatus == 0) {
        lcd.print("ON     ");
    } else if (relayStatus == 1) {
        lcd.print("OFF    ");
    } else if (relayStatus == 3) {
        lcd.print("AUTO   ");
    } else if (relayStatus == 4) {
        lcd.print("Manual ");
    }
}


void setup() {
  pinMode(RelayPIN, OUTPUT);
  pinMode(ButtonIP, INPUT);
  pinMode(ButtonMode, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);

  lcd.begin();

  lcd.backlight();

  dht.begin();

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  lcd.setCursor(0, 0);
  lcd.print("Connecting.......");
  lcd.setCursor(0, 1);
  lcd.print("........To WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    x = digitalRead(ButtonMode);
    y = digitalRead(ButtonIP);

    if (y == HIGH && x == HIGH) {
      break;
    }
  }

  Serial.println();
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature1", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(t).c_str());
  });
  server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(f).c_str());
  });
  server.on("/kelvin", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(kelvin).c_str());
  });
  server.on("/reamur", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(reamur).c_str());
  });
  server.on("/rankie", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(rankie).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", String(h).c_str());
  });
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    relayStatus = 0;
    automode = false;
    digitalWrite(RelayPIN, LOW);  
    updateLCD();  
    request->send(200, "text/plain", "Relay is ON");
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
      relayStatus = 1;
      automode = false;
      digitalWrite(RelayPIN, HIGH);  
      updateLCD();  
      request->send(200, "text/plain", "Relay is OFF");
  });

  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request) {
      relayStatus = 3;
      automode = true;
      updateLCD();  
      request->send(200, "text/plain", "Relay is in AUTO mode");
  });

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP Server :....");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1000);
  lcd.clear();

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float newT = dht.readTemperature();
    float newF = dht.readTemperature(true);

    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      t = newT;
      f = newF;
      reamur = t * 0.8;
      kelvin = t + 273.15;
      rankie = t * 1.8 + 491.67;
      Serial.println(t);
      if (mode == 0) {
        lcd.setCursor(7, 0);
        lcd.print(t);
        lcd.print("C");
      } else if (mode == 1) {
        lcd.setCursor(7, 0);
        lcd.print(f);
        lcd.print("F");
      } else if (mode == 2) {
        lcd.setCursor(7, 0);
        lcd.print(reamur);
        lcd.print("R");
      } else if (mode == 3) {
        lcd.setCursor(7, 0);
        lcd.print(kelvin);
        lcd.print("K");
      } else if (mode == 4) {
        lcd.setCursor(7, 0);
        lcd.print(rankie);
        lcd.print("Ra");
      }
    }

    float newH = dht.readHumidity();
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      h = newH;
      Serial.println(h);
      lcd.setCursor(7, 1);
      lcd.print(h);
      lcd.print(" %");
    }
    lcd.setCursor(0, 0);
    lcd.print("Suhu : ");
    lcd.setCursor(0, 1);
    lcd.print("Hum  : ");
  }
  x = digitalRead(ButtonMode);
  y = digitalRead(ButtonIP);

  off = 0;

  if (y == HIGH && x == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Relay : ");
    relayStatus = 4;
    updateLCD();
    delay(1000);
    varB = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Relay : ");
    lcd.setCursor(0, 1);
    if (t > 29) {
      lcd.print("ON ");
      digitalWrite(RelayPIN, LOW);
    } else {
      lcd.print("OFF ");
      digitalWrite(RelayPIN, HIGH);
    }
    while (varB == 1) {
      x = digitalRead(ButtonMode);
      y = digitalRead(ButtonIP);
      if (y == HIGH && x == HIGH) {
        varB = 0;
      } else if (x == HIGH) {
        digitalWrite(RelayPIN, LOW);
        relayStatus = 0;
        updateLCD();
      } else if (y == HIGH) {
        digitalWrite(RelayPIN, HIGH);
        relayStatus = 1;
        updateLCD();
      }

      delay(50);
    }

    lcd.setCursor(0, 0);
    lcd.print("Relay :");
    relayStatus = 3;
    updateLCD();
    delay(1000);
    lcd.clear();
  } else if (y == HIGH) {

    if (varA == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode :");
      mode++;
      if (mode == 5) {
        mode = 0;
      }

      lcd.setCursor(0, 1);
      if (mode == 0) {
        lcd.print("Celcius");
      } else if (mode == 1) {
        lcd.print("Fahrenheit");
      } else if (mode == 2) {
        lcd.print("Reamur");
      } else if (mode == 3) {
        lcd.print("Kelvin");
      } else if (mode == 4) {
        lcd.print("Rankie");
      }
      delay(500);
      lcd.clear();
      varA = 1;
      lastmilis = millis();
    } else if (varA == 1 && currentMillis - lastmilis > 3000) {
      lcd.clear();
      lcd.print("Restart.....");
      delay(500);
      ESP.restart();
    }
  }
  //melihat ip address
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

  if (automode) {
    if (t > 29) {
      digitalWrite(RelayPIN, LOW);  
    } else {
      digitalWrite(RelayPIN, HIGH);   
    }
  }

}
