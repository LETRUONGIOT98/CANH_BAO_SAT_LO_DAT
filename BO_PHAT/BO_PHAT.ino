#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Khởi tạo MPU6050 và các biến
Adafruit_MPU6050 mpu;
//98:F4:AB:D6:33:19
uint8_t peer1Address[] = {0x98, 0xF4, 0xAB, 0xD6, 0x33, 0x19}; // Địa chỉ MAC ESP nhận
uint8_t goc = 0;
float donhay = 0.08;
#define nhac D3
// Cấu trúc dữ liệu để gửi qua ESP-NOW
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

struct_message myData;

// Hàm callback khi gửi dữ liệu thành công
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Trạng thái gửi dữ liệu: ");
  Serial.println(sendStatus == 0 ? "Thành công" : "Thất bại");
}
long times = 0;
bool tt1 =true;
void setup() {
  // Khởi tạo Serial và MPU6050
  Serial.begin(115200);
  pinMode(nhac, OUTPUT);
  digitalWrite(nhac, HIGH);
  delay(2000);
  if (!mpu.begin()) {
    Serial.println("Không tìm thấy chip MPU6050");
      delay(10);
    }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // Cài đặt ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Không thể khởi tạo ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(peer1Address, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  // Đọc giá trị cảm biến
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Lấy giá trị gia tốc góc
  float ax = g.gyro.x;
  float ay = g.gyro.y;
  float az = g.gyro.z;

  // Kiểm tra rung chấn (vượt ngưỡng ±donhay)
  if (abs(ax) > donhay || abs(ay) > donhay || abs(az) > donhay) { // Trừ đi trọng lực trái đất
    goc = 1;
    if(tt1){
    digitalWrite(nhac, LOW);
    Serial.print("THONG BAO");
    times = millis();
    tt1 = false;
    delay(1000);
    digitalWrite(nhac, HIGH);
    }
  }  
  if(millis() - times >= 32000){
    tt1 = true;
    goc = 0;
    times = millis();
  }
  // In giá trị gia tốc và trạng thái rung
  Serial.print("AX: ");
  Serial.print(ax, 2);
  Serial.print(" | AY: ");
  Serial.print(ay, 2);
  Serial.print(" | AZ: ");
  Serial.print(az, 2);
  Serial.print(" | Rung: ");
  Serial.println(goc);

  // Gửi trạng thái qua ESP-NOW
  myData.mappedValue = goc;
  esp_now_send(peer1Address, (uint8_t *)&myData, sizeof(myData));

  delay(200); // Độ trễ 200ms
}
