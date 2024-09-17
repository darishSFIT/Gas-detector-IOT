#include <ESP8266WiFi.h>
#include <ESP8266Firebase.h>
#include <DHT.h>

// Firebase configuration
#define _SSID "Pixel 7a"            // Your WiFi SSID
#define _PASSWORD "87654321"        // Your WiFi Password
#define REFERENCE_URL "https://parkbuddy-653a1-default-rtdb.firebaseio.com"  // Your Firebase project reference URL

// DHT sensor configuration
#define DHTPIN D1        // DHT11 Data pin connected to D3 (GPIO0)
#define DHTTYPE DHT11    // DHT 11 sensor
DHT dht(DHTPIN, DHTTYPE);

// MQ-2 sensor pin configuration
#define MQ2_PIN A0       // MQ-2 Analog pin connected to A0

// Initialize Firebase
Firebase firebase(REFERENCE_URL);

void setup() {
  Serial.begin(115200);
  
  // Setup Wi-Fi connection
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }

  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read temperature and humidity values from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Read temperature in Celsius


  
//  // Uncomment when faulty DHT replaced  
//  // Check if any readings failed
//  if (isnan(humidity) || isnan(temperature)) {
//    Serial.println("Failed to read from DHT sensor!");
//    delay(2000);
//    return;
//  }



  // Read MQ-2 gas sensor value (analog value)
  int gasValue = analogRead(MQ2_PIN);
  float gasPercentage = map(gasValue, 0, 1023, 0, 100); // Convert to percentage

  // Print sensor values on Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Gas Level: ");
  Serial.print(gasPercentage);
  Serial.println(" %");

  // Send data to Firebase (under 'gas_detector' path)
  // Added this 'if' part to send the string format of dht values (i.e. nan) which is error to firebase
  if (isnan(humidity) || isnan(temperature)) {
    firebase.setString("gas_detector/temperature", "Sensor not working");
    firebase.setString("gas_detector/humidity", "Sensor not working");
  }else{
    firebase.setFloat("gas_detector/temperature", temperature);
    firebase.setFloat("gas_detector/humidity", humidity);
  }
  
  firebase.setFloat("gas_detector/gas_level", gasPercentage);

  // Delay to avoid flooding Firebase with updates
  delay(1000); // Update every 1 seconds
}
