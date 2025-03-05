#define PHONE_NUMBER "+84328813393"
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SoftwareSerial.h>
static const int RXPin = 27, TXPin = D1; 
SoftwareSerial simSerial(RXPin, TXPin);
#include <Servo.h>
Servo myServo;
// Cấu trúc dữ liệu nhận
typedef struct struct_message {
  uint8_t mappedValue;
} struct_message;

// Tạo một biến kiểu struct_message
struct_message incomingData;

uint8_t tt;
bool tt1 = true;
bool tt2 = true;
bool tt3 = true;
long times,time1;
// Chân điều khiển servo
#define coi D2
#define bom D3
#define CB_1 D5
#define CB_2 D6
#define CB_3 D7
#define cb_lut A0
// Hàm xử lý dữ liệu nhận được
void OnDataRecv(uint8_t *mac, uint8_t *data, uint8_t len) {
    if (len == sizeof(incomingData)) {
        memcpy(&incomingData, data, sizeof(incomingData));
        tt = incomingData.mappedValue; 
        Serial.print("Mapped Value: ");
        Serial.println(tt);
    }
}

void setup() {
  // Khởi tạo Serial Monitor
  Serial.begin(115200);
  pinMode(CB_1, INPUT_PULLUP);
  pinMode(CB_2, INPUT_PULLUP);
  pinMode(CB_3, INPUT_PULLUP);
  pinMode(cb_lut, INPUT);
  myServo.attach(D4,500,2400);
  myServo.write(90);
  pinMode(coi, OUTPUT);
  digitalWrite(coi, LOW);
  pinMode(bom, OUTPUT);
  digitalWrite(bom, LOW);
  // Khởi động WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
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
 simSerial.begin(115200);
    delay(1000);
    // Check AT Command
    sim_at_cmd("AT");

    // Product infor
    sim_at_cmd("ATI");

    // Check SIM Slot
    sim_at_cmd("AT+CPIN?");

    // Check Signal Quality
    sim_at_cmd("AT+CSQ");

    sim_at_cmd("AT+CIMI");
}

void loop() {
  //Serial.println("DANG CHAY");
 
  if(analogRead(cb_lut) <= 400){
    digitalWrite(coi, HIGH);
    times = millis();
    Serial.println("CO LU LUT");
    if(tt3) nhantin("PHAT HIEN LU LUT DANG CAO");
    tt3 = false;
  } 
  if(digitalRead(CB_1) == 0){
    Serial.println("PHAT HIEN LUA GOC 1");
   myServo.write(135);
   if(tt2) nhantin("CANH BAO! PHAT HIEN LUA");
   tt2 =false;
   time1 = millis();
   digitalWrite(bom, HIGH);
   tt2 = false;
  }
  else if(digitalRead(CB_2) == 0){
    Serial.println("PHAT HIEN LUA GOC 2");
   myServo.write(90);
   if(tt2) nhantin("CANH BAO! PHAT HIEN LUA");
   time1 = millis();
   digitalWrite(bom, HIGH);
   tt2 = false;
  }
  else if(digitalRead(CB_3) == 0){
    Serial.println("PHAT HIEN LUA GOC 3");
   myServo.write(45);
   if(tt2) nhantin("CANH BAO! PHAT HIEN LUA");
   time1 = millis();
   digitalWrite(bom, HIGH);
   tt2 = false;
  }
  else {
    //myServo.write(90);
    digitalWrite(bom, LOW);
    if(millis() - time1 >= 3000){
      tt2 = true;
    }
  }
  if(tt){
    Serial.println("CO RUNG CHAN");
    digitalWrite(coi, HIGH);
    times = millis();
    if(tt1){
      nhantin("CANH BAO CO RUNG CHAN TU NUI");
      tt1 = false;
    }
  }
  if(millis() - times > 3000 && !tt){
    digitalWrite(coi, LOW);
    tt = false;
    tt1 = true;
    tt3 = true;
  }
  delay(100);
}
void nhantin(String ib) {
  Serial.println("DANG NHAN TIN");
   sim_at_cmd("AT+CMGF=1");
    String temp = "AT+CMGS=\"";
    temp += (String)PHONE_NUMBER;
    temp += "\"";
    sim_at_cmd(temp);
    sim_at_cmd(ib);
    // End charactor for SMS
    sim_at_send(0x1A);
}
void sim_at_wait()
{
    delay(100);
    while (simSerial.available()) {
        Serial.write(simSerial.read());
    }
}
bool sim_at_send(char c){
    simSerial.write(c);
    return true;
}
bool sim_at_cmd(String cmd){
    simSerial.println(cmd);
    sim_at_wait();
    return true;
}
