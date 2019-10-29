#include<LiquidCrystal_I2C_Hangul.h>
#include<Wire.h>

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); //j Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4][4] = {{0xA8, 0x88, 0x4A, 0x27},
                       {0x98, 0xBF, 0xA8, 0x27},
                       {0xDC, 0x93, 0xBF, 0x49},
                       {0x60, 0x2D, 0xC2, 0x49}};

byte preId[4];

int line = 0;

char box = 0xff;

int rfid_num = 0;
byte answer_str[50];
int answer_str_len=0;
bool lcd_answer_flag=false;
unsigned long time;
int time_count=                      -15;
LiquidCrystal_I2C_Hangul lcd(0x3f,16,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  lcd.noBacklight();
  lcd.setCursor(2,0);
  lcd.print("Corporal");
  lcd.setCursor(8,1);
  lcd.print("Jarvis");
  
  SPI.begin(); // Init SPI bus

  pinMode(8, OUTPUT);
  
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  pinMode(A0, INPUT_PULLUP);
  

 // Serial.println(F("This code scan the MIFARE Classsic NUID."));
  //Serial.print(F("Using the following key:"));
  //printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
  // put your main code here, to run repeatedly:
  int a = analogRead(A0);
  int interval=300;
  if(lcd_answer_flag==true && millis()-time>interval){
    //32
    lcd.setCursor(0,1);
    for(int i=0;i<16;i++){
      if(i+time_count>=answer_str_len || i+time_count<0) lcd.write(32);
      else lcd.write(answer_str[i+time_count]);
    }
    time+=interval;
    time_count++;
    if(time_count==answer_str_len) time_count=-15;
  }
  if(a<600) {
    lcd.backlight();
    rfid_num = rfid_control();
    lcd_control(rfid_num);
    }
  else {
    lcd_answer_flag=false;
    lcd.noBacklight();
    lcd.setCursor(0,0);
    lcd.print("  Corporal      ");
    lcd.setCursor(0,1);
    lcd.print("        Jarvis  ");
    }
}

void lcd_control(int rfid_num) {
  String str;
  if (rfid_num == 1) {
    str = "Jung Min";
    }
  else if (rfid_num == 2) {
    str = "Seong Jin";
    }
  else if (rfid_num == 3) {
    str = "Won Seok";
    }
  else {
    str = "Kyun Chul";
    }
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    lcd.setCursor(0,0);
    lcd.print("Name : " + str);
    lcd.setCursor(0,1);
    answer_str_len=0;
    time_count=-15;
    while (Serial.available() > 0) {
      // display each character to the LCD
      answer_str[answer_str_len++]=Serial.read();
    }
    lcd_answer_flag=true;
    time=millis();
  }
  }

int rfid_control() {
   // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  //Serial.print(F("PICC type: "));
  //MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  //if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
  //  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
  //  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
  //  Serial.println(F("Your tag is not of type MIFARE Classic."));
  //  return;
  //}

  int i, j;
  int count = 0;
  int box_count = 2;
  int box_reverse = 0;
  int box_cursor = 7;
  int rfid_id = 0;

  //tone(8, 392, 4);
  digitalWrite(8, HIGH);
  delay(100);
  digitalWrite(8, LOW);

  for(i = 0; i < 4; i++){
    count = 0;
    for(j = 0; j < 4; j++){
      if(rfid.uid.uidByte[j] == nuidPICC[i][j])count++;
    }
    if(count == 4){
      rfid_id = (i+1);
      Serial.write(rfid_id);
      for(j = 0; j < 4; j++){
        preId[j] = rfid.uid.uidByte[j];
      }
    }
    
  }

//  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
//    rfid.uid.uidByte[1] != nuidPICC[1] || 
//    rfid.uid.uidByte[2] != nuidPICC[2] || 
//    rfid.uid.uidByte[3] != nuidPICC[3] ) {
//    Serial.println(F("A new card has been detected."));
//
//    // Store NUID into nuidPICC array
//    for (byte i = 0; i < 4; i++) {
//      nuidPICC[i] = rfid.uid.uidByte[i];
//    }
//   
//    Serial.println(F("The NUID tag is:"));
//    Serial.print(F("In hex: "));
//    printHex(rfid.uid.uidByte, rfid.uid.size);
//    Serial.println();
//    Serial.print(F("In dec: "));
//    printDec(rfid.uid.uidByte, rfid.uid.size);
//    Serial.println();
//  }
//  else Serial.println(F("Card read previously."));

//  while(true){
//    rfid.PICC_IsNewCardPresent();
//    for(i = 0; i < 4; i++){
//      if(rfid.uid.uidByte[i] == preId[i]){
//        count++;
//      }
//    }
//    printHex(rfid.uid.uidByte, 4);
//    printHex(preId, 4);
//    Serial.println();
//    if(count < 4) break;
//    delay(10);
//  }
  lcd_answer_flag=false;
  lcd.clear();
  while(rfid.PICC_ReadCardSerial() || rfid.PICC_IsNewCardPresent()){
    if(box_reverse == 0) {
      lcd.setCursor(box_cursor,0);
      for(i = 0; i < box_count; i++) {
        lcd.print(box);
        }
      lcd.setCursor(box_cursor,1);
      for(i = 0; i < box_count; i++) {
        lcd.print(box);
        }
      box_count += 2;
      box_cursor--;
      }
    if(box_count == 18) {
        box_cursor = 0;
        box_reverse = 1;
        box_count = 16;
        }
     
    if(box_reverse == 1) {
      lcd.setCursor(box_cursor,0);
      for(i = 0; i < box_count; i++) {
        lcd.print(box);
        }
      lcd.setCursor(box_cursor,1);
      for(i = 0; i < box_count; i++) {
        lcd.print(box);
        }
      box_count -= 2;
      box_cursor++;
      }
      if(box_count == -2) {
        box_cursor = 7;
        box_reverse = 0;
        box_count = 2;
        }
    delay(64);
    lcd.clear();
  }

  lcd.clear();

  Serial.write(0);
  digitalWrite(8, HIGH);
  delay(50);
  digitalWrite(8, LOW);
  delay(50);
  digitalWrite(8, HIGH);
  delay(50);
  digitalWrite(8, LOW);

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return rfid_id;
  }
