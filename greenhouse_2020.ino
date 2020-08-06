#include <DHT.h>
#include <WiFi.h>
#define DHTPIN 14
#define DHTTYPE DHT11
const char* ssid = "OurNetworkName";
const char* password = "ourNetworkPassword";
int hydroZone[3] = {1, 2, 3};
int hydroValve[3] = {4, 5, 6};
int heaterValve = 7;
int heater = 14;
int pump = 8;
int waterLevel = 9;
int fan = 10;
int lightSensor = 11;
int growLight = 12;
int waterTemp = 13;
int tempSetting = 55;
bool waterSafety = true;
long currentTime = 0;

DHT dht(DHTPIN, DHTTYPE);

portMUX_TYPE synch = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR alarm() {
  portENTER_CRITICAL(&synch);  
  waterSafety = false;
  portEXIT_CRITICAL(&synch);
}

void connectToNetwork() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing WiFi connection");
  }
  Serial.println("Connected to Network");
}



void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to the Belknap's Greenhouse Control Program");
  attachInterrupt(digitalPinToInterrupt(waterLevel), alarm, LOW);

  for (int i = 1; i <= 3; i++) {
    pinMode(hydroZone[i], INPUT);
  }
  for (int j = 4; j < 6; j++) {
    pinMode(hydroValve[j], OUTPUT);
  }
  pinMode(heaterValve, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(waterLevel, INPUT);
  pinMode(fan, OUTPUT);
  pinMode(lightSensor, INPUT);
  pinMode(growLight, OUTPUT);
  pinMode(waterTemp, INPUT);

  dht.begin();
}
void watering() {
  long wateringTime = 10000; //sets the duration of the watering cycle in milliseconds
  if (waterSafety == true) {
    for (int i = 1; i <= 3; i++) {
      int zoneReading = analogRead(hydroZone[i]);
      if (zoneReading > 155) {
        int j = i + 3;
        digitalWrite(hydroValve[j], HIGH);
        digitalWrite(pump, HIGH);
        if ((millis() - currentTime) > wateringTime) {
          digitalWrite(pump, LOW);
          digitalWrite(hydroValve[j], LOW);
        }
      }
    }
  }
}
void heating() {
  float waterTempSetting = 150.0;  //Sets the saftey temp of the water reservoir
  //  float humid = dht.readHumidity();
  float temp = dht.readTemperature(true);
  float waterTemperature = analogRead(waterTemp);
  if (temp < tempSetting) {
    if (waterSafety == true) {
      digitalWrite(heater, HIGH);
      digitalWrite(heaterValve, HIGH);
      digitalWrite(pump, HIGH);
      if (waterTemperature > waterTempSetting) {
        digitalWrite(heater, LOW);
      }

    }
  }
  else {
    digitalWrite(heater, LOW);
    digitalWrite(heaterValve, LOW);
    digitalWrite(pump, LOW);
  }
  
  }





void loop() {

  watering();
  heating();


}
