#include <ESP8266WiFi.h>
#include <espnow.h>

// Cấu trúc dữ liệu nhận
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

// Tạo một biến kiểu struct_message
struct_message incomingData;

uint8_t tt;
long times;
// Chân điều khiển servo
#define coi D2

// Hàm xử lý dữ liệu nhận được
void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
  if (len == sizeof(incomingData)) { // Kiểm tra kích thước dữ liệu nhận được
    memcpy(&incomingData, data, sizeof(incomingData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Mapped Value: ");
    Serial.println(incomingData.mappedValue);

    // Điều khiển servo theo giá trị nhận được
  tt = incomingData.mappedValue;
  if(tt){
  digitalWrite(coi, HIGH);
  }
  else digitalWrite(coi, LOW);
    
  } else {
    Serial.println("Received data size mismatch");
  }
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);
  pinMode(coi, OUTPUT);
  digitalWrite(coi, LOW);
  // Khởi động WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
  uint8_t customMAC[6] = {0x98, 0xF4, 0xAB, 0xD6, 0x33, 0x19}; // Địa chỉ MAC tùy chỉnh
  wifi_set_macaddr(STATION_IF, customMAC);
  Serial.println(" ");
  
  Serial.println(WiFi.macAddress()); // In ra Serial Monitor địa chỉ MAC của ESP8266
  // Khởi tạo ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Đăng ký callback khi nhận được dữ liệu
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
 
}

void loop() {
   
}
