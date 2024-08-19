#include <Adafruit_Sensor.h>
#include <BluetoothSerial.h>
#include <DHT.h>

#define DHTPIN 4        // Chân DATA của DHT11 nối với GPIO4
#define DHTTYPE DHT11   // Loại cảm biến DHT11

#define LED_PIN 2       // Chân GPIO nối với LED
#define sleep_PIN 0    // Chân GPIO nhận tín hiệu để vào Deep Sleep

#define temp_thresh 40 // Ngưỡng nhiệt độ cảnh báo

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_bt"); // Tên Bluetooth
  dht.begin();
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(sleep_PIN, INPUT_PULLUP); // Cảm biến hoặc nút nhấn kết nối với chân GPIO0
  
  Serial.println("Bluetooth Started");

  // Nếu chân sleep_PIN có tín hiệu LOW, vào chế độ Deep Sleep
  if (digitalRead(sleep_PIN) == LOW) {
    enterDeepSleep();
  }
}

void loop() {
  // Đọc nhiệt độ và độ ẩm
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Gửi dữ liệu qua Bluetooth
  SerialBT.print("Temperature: ");
  SerialBT.print(t);
  SerialBT.print(" °C ");
  SerialBT.print("Humidity: ");
  SerialBT.print(h);
  SerialBT.println(" %");
  
  // Kiểm tra nếu nhiệt độ vượt quá ngưỡng
  if (t > temp_thresh ) {
    // Nhấp nháy LED để cảnh báo
    blinkLED();
    // Gửi cảnh báo qua Bluetooth
    SerialBT.println("ALERT: Temperature exceeds 40°C!");
  }
  
  delay(5000); // Gửi dữ liệu mỗi 5 giây
  
  // Kiểm tra tín hiệu từ chân sleep_PIN để vào chế độ Deep Sleep
  if (digitalRead(sleep_PIN) == LOW) {
    enterDeepSleep();
  }
}

void blinkLED() {
  for (int i = 0; i < 10; i++) { // Nhấp nháy LED 10 lần
    digitalWrite(LED_PIN, HIGH);
    delay(100); // LED sáng trong 100ms
    digitalWrite(LED_PIN, LOW);
    delay(100); // LED tắt trong 100ms
  }
}

void enterDeepSleep() {
  Serial.println("Entering Deep Sleep...");
  esp_sleep_enable_ext0_wakeup(sleep_PIN, LOW); // Kích hoạt chế độ Deep Sleep với tín hiệu LOW trên sleep_PIN
  esp_deep_sleep_start();
}
