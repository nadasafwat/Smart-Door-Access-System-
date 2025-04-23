// (1) call the libtaties

// 1- oled 
#include<Adafruit_SSD1306.h>
#include<Adafruit_GFX.h>
#include<Wire.h>

// 2- RFID 
#include<MFRC522.h> 
#include<SPI.h>

// 3- Fire base and wifi
#include<Firebase.h>
#include<WiFi.h>

// (2) set Macros

// 1- o led 
#define width 128
#define hight 64
Adafruit_SSD1306 OLed(width,hight,&Wire,-1); // &Wire --> I2C protocol || -1 --> no pin used 

// 2- RFID
#define SS 2 // --> slave select 
#define RST 5  // --> reset pin 
MFRC522 RFID(SS,RST);

// 3- Firebase 
#define Fb_url "https://iot-project-755d4-default-rtdb.firebaseio.com/"
#define Fb_token "ecvfbuhe9H7zDqrRSkZUouYfUGGGvloqOeMX8Y5R"
Firebase Fb(Fb_url,Fb_token);

// 4- WiFi 
#define SSID "Nada Safwat"
#define PASSWORD "nada2123"

// Outputs

#define bLed 12
#define rLed 14
#define buzzer 27



void setup() {
  // put your setup code here, to run once:

  // enable serial
  Serial.begin(115200);
  
  // set pin modes :
  pinMode(bLed, OUTPUT);
  pinMode(rLed, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // reset pins value 

  digitalWrite(bLed,LOW);
  digitalWrite(rLed,LOW);
  digitalWrite(buzzer,LOW);

  // setup rfid 
  SPI.begin();
  RFID.PCD_Init();

  // enable OLed
  OLed.begin(SSD1306_SWITCHCAPVCC,0x3C);
  OLed.clearDisplay();        
  OLed.display();

  // //enable wifi 
  WiFi.begin(SSID,PASSWORD);

  // check wifi connectivity 
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n Connected! ");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  //check o led connectivity 
  if(!OLed.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    Serial.println("Can't coonect the o led ");
    while (true);
  }

}

String get_UId(){
  String Uid = "";
  for(byte i = 0 ; i < RFID.uid.size;i++){
    Uid+= String(RFID.uid.uidByte[i]);
  }
  Serial.println(Uid);
  return Uid;

}

void showMessage(String msg){
  OLed.clearDisplay();
  OLed.setTextSize(1);
  OLed.setTextColor(WHITE);
  OLed.setCursor(10, 20);
  OLed.println(msg);
  OLed.display();
}

void AccessGranted(String UID){
  digitalWrite(bLed,HIGH);
  digitalWrite(rLed,LOW);
  digitalWrite(buzzer,HIGH);
  delay(500);
  digitalWrite(buzzer,LOW);
  showMessage("Access Granted");
  Fb.pushString("logs", "Access Granted  "+ UID);
  delay(2000);
  digitalWrite(bLed,LOW);
  digitalWrite(rLed,LOW);
  digitalWrite(buzzer,LOW);
}

void AccessDenied(String UID){
  digitalWrite(bLed,LOW);
  digitalWrite(rLed,HIGH);
  digitalWrite(buzzer,HIGH);
  showMessage("Access Denied");
  Fb.pushString("logs", "Access Denied  "+ UID);
  delay(1000);
  digitalWrite(bLed,LOW);
  digitalWrite(rLed,LOW);
  digitalWrite(buzzer,LOW);
}

void loop() {
  // check rfid scanning 
  if(RFID.PICC_IsNewCardPresent()&& RFID.PICC_ReadCardSerial()){
    String u_id = get_UId();
    showMessage("sacanning");
    String path = "authorized/"+u_id;
    int result = Fb.getInt(path);
    Serial.println("result "+String(result));
    if(result==1){
      AccessGranted(u_id);
    }
    else{
      AccessDenied(u_id);
    }
    delay(1000);
    RFID.PICC_HaltA();
  }
}
