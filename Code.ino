   // LCD INITIALIZATION
   #include <dht.h>
   #include <LiquidCrystal.h>  
   int Contrast=100;
   LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   
   #define dht_dpin A1 //no ; here. Set equal to channel sensor is on
   dht DHT;
   int Temp=0;
   int Humid=0;
   int MaxTempVal=37;
   int MaxHumidVal=85;
   int RelayPinFan=39;
   int RelayPinPump=41;
   int pushButton = 21;
   volatile byte state = LOW;
   

   // SD INITIALIZATION
   #include<SD.h>
   #include <SPI.h>
  
   const int cs = 53;
   File myFile;

   // MOTOR PINS INITILIZATION
   const int stepPin = 8; 
   const int dirPin = 9; 

   // BUZZER PIN INITILIZATION
   const int BuzzerPin =  22;

   // RELAY PIN INITILIZATION
   const int RelayPin =  7;

   // KEYPAD INITILIZATION
   #include <Keypad.h>
   const byte ROWS = 4; //four rows
   const byte COLS = 3; //three columns
   char keys[ROWS][COLS] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
    };
    byte rowPins[ROWS] = {46, 44, 42, 40}; //connect to the row pinouts of the keypad
    byte colPins[COLS] = {38, 36, 34}; //connect to the column pinouts of the keypad
    Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


    //DS1302 INITIALIZATION
    #include  <virtuabotixRTC.h>  //Library used
    virtuabotixRTC myRTC( 31, 29,27); //If you change the wiring change the pins here also

   

    
void setup() {
  //This is to set the time of clock
  // myRTC.setDS1302Time(30, 22, 10, 6,5, 5, 2018);  //Here you write your actual time/date as shown above 
                                                   //but remember to "comment/remove" this function once you're done
                                                   //The setup is done only one time and the module will continue counting it automatically

  
  //Serail Monitor INITIALIZATION
  Serial.begin(9600);
  
  // LCD INITIALIZATION
   analogWrite(6,Contrast);
   lcd.begin(16, 2);
   
  // SD INITIALIZATION
  pinMode(53, OUTPUT);

  // SETTING MOTOR PINS AS OUTPUT
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  // SETTING buzzer PIN AS OUTPUT
  pinMode(BuzzerPin,OUTPUT);
  
  // SETTING RELAY PIN AS OUTPUT
  pinMode(RelayPin,OUTPUT);
  pinMode(RelayPinFan, OUTPUT);
  pinMode(RelayPinPump, OUTPUT);
  digitalWrite(RelayPinFan, HIGH);
  digitalWrite(RelayPinPump, HIGH);
   pinMode(pushButton, INPUT);
  
  // see if the card is present
  if (!SD.begin(cs)) 
  {
    Serial.println("Card failed to initialize, or not present");
  
    return;
  }
  Serial.println("card initialized.");
  
  //Setting up keypad 
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad


 
}


void loop() {
 
//Shows the values of DHT11 on LCD.
ShowingValLcd();

//Check from keypad.
CheckKeypad();

//Checking the sd card values.
SDCardVal();

//Checking LDR values, for Bulbs feed back
LDRval();

//Motor Rotate
MotorRotate();

//Clock Time checking
Clock();

//Clock Time checking
CheckHumidity();

//Getting Kepad key
char key = keypad.getKey();
  if (key) {
     Serial.println(key);
  }

}



////////////Shows the values of DHT11 on LCD.////////////////////////////////////
void ShowingValLcd()
{
   
    DHT.read11(dht_dpin);
    lcd.setCursor(0, 0);
    lcd.print( "TEMPRATURE");
    lcd.setCursor(0, 1);
    lcd.print(DHT.temperature);
    delay(2000);
    lcd.clear();
  
    lcd.setCursor(0, 0);
    lcd.print( "HUMIDITY");
    lcd.setCursor(0, 1);
    lcd.print(DHT.humidity);
    delay(2000);
    lcd.clear(); 
    //Setting temperature values
    Temp=DHT.temperature;
    Humid=DHT.humidity;
  
  }


//////////print time to Serial////////////////////////////////////////////////////////////
void Clock()
{
   // This allows for the update of variables for time or accessing the individual elements.
  myRTC.updateTime();

  // Start printing elements as individuals
  Serial.print("Current Date / Time: ");
  Serial.print(myRTC.dayofmonth);             //You can switch between day and month if you're using American system
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print(" ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print(":");
  Serial.println(myRTC.seconds);

  // Delay so the program doesn't print non-stop
  delay(1000);

  lcd.clear();
  lcd.print(myRTC.dayofmonth);             //You can switch between day and month if you're using American system
  lcd.print("/");
  lcd.print(myRTC.month);
  lcd.print("/");
  lcd.print(myRTC.year);
  lcd.print(" ");
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes);
  lcd.print(":");
  lcd.println(myRTC.seconds);
  delay(5000);
  
  int dcPump=0;
  if(myRTC.hours==11)
  {
   dcPump=1;
    }

    else if(myRTC.hours==12 && dcPump==1){
    Serial.println("This is the hour. Time to turn on DC PUMP");
    digitalWrite(RelayPinPump,LOW);
    delay(5000);
    digitalWrite(RelayPinPump,HIGH);
    dcPump=0;
      }
   else{
     Serial.println("NO need to turn on DC PUMP");
    digitalWrite(RelayPinPump,HIGH);
    }
  
  }



////////////////////Checking the sd card values.////////////////////////////////
void SDCardVal()
{   
   if (SD.exists("example.txt")) {
    Serial.println("example.txt exists.");
    lcd.clear();
    lcd.print("Card working");

  } else {
    Serial.println("example.txt doesn't exist.");
  }


  myFile = SD.open("example.txt", FILE_WRITE);
  myFile.println("temperature");
  myFile.println(Temp);
  myFile.println("Humidity");
  myFile.println(Humid);
  myFile.close();

//  // re-open the file for reading:
//  myFile = SD.open("example.txt");
//  if (myFile) {
//    Serial.println("example.txt:");
//
//    // read from the file until there's nothing else in it:
//    Serial.println("READING");
//    while (myFile.available()) {
//      
//      Serial.write(myFile.read());
//    }
//    Serial.println("############# End of reading SD card #################");
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
  
  }


//////////////////Checking LDR values, for Bulbs feed back////////////////////////////
void LDRval()
{
  
int LDR = analogRead(A0);
String stringOne = "LDR value: ";
String stringThree = stringOne + LDR;
Serial.println(stringThree);
stringOne = "Temperature value: ";
stringThree = stringOne + Temp;
Serial.println(stringThree);
stringOne = "Humidity value: ";
stringThree = stringOne + Humid;
Serial.println(stringThree);
lcd.clear();
lcd.print("LDR");
lcd.print(LDR);


if(Temp<MaxTempVal)
{
digitalWrite(RelayPin,LOW); //HERE BULB GETS ON
//digitalWrite(RelayPinFan,LOW); //HERE FAN GETS ON  
if((LDR<120))
  {
    Serial.println("Bulb is fused");
     //Turn buzzer on
     Serial.println(LDR);
    digitalWrite(BuzzerPin,HIGH); //This will turn buzzer on
     delay(500);
    digitalWrite(BuzzerPin,LOW); //This will turn buzzer on
    lcd.clear();
    lcd.print("BULB FUSED");
   
    }
else if((LDR>120)) {
      Serial.println("Bulb is working good");
       //digitalWrite(BuzzerPin,LOW); //This will turn buzzer off
       //No buzzer on
      Serial.println(LDR);
      delay(200);
      lcd.clear();
      lcd.print("BULB WORKING");
      }
else{
         Serial.println("Nothing");
         delay(200);
     }
}

else{
  digitalWrite(RelayPin,HIGH);
  
  }

  }


  // Taking care of some special events of KeyPAD.
void keypadEvent(KeypadEvent key){
    switch (keypad.getState()){
    case PRESSED:
        if (key == '#') {
            Serial.println("HASH LONG CLICKED");
            
        }
        break;

    case RELEASED:
        if (key == '*') {
           Serial.println("STAR click Relaeased");
        }
        break;

    case HOLD:
        if (key == '*') {
            Serial.println("STAR click HOLD");
        }
        break;
    }
} 


//////////////////MOTOR ROTATION CODE////////////////////////////
void MotorRotate(){
    //Rotating motor to 30 degree

    int ToRotate=0;
// //Rotating motor to 30 degree
//    digitalWrite(dirPin,HIGH); // On high anti-clockwise Enables the motor to move in a particular direction
//  // Makes 200 pulses for making one full cycle rotation
//  for(int x = 0; x < 250; x++) {
//    digitalWrite(stepPin,HIGH); 
//    delayMicroseconds(500); 
//    digitalWrite(stepPin,LOW); 
//    delayMicroseconds(500); 
//  }
//  delay(1000); // One second delay
  

  
//  digitalWrite(dirPin,LOW); //Changes the rotations direction
//  // Makes 400 pulses for making two full cycle rotation
//  for(int x = 0; x < 250; x++) {
//    digitalWrite(stepPin,HIGH);
//    delayMicroseconds(500);
//    digitalWrite(stepPin,LOW);
//    delayMicroseconds(500);
//  }
//  delay(1000);
if(myRTC.hours==6 || myRTC.hours ==13 || myRTC.hours|| 20)
{
 Serial.println("in next hour we have to rotate the motor. so setting up the variable");
 ToRotate=1;
  }
  if((myRTC.hours==7 && ToRotate==1)|| (myRTC.hours ==14 && ToRotate==1)|| (myRTC.hours==21&& ToRotate==1))
  {
  //Rotating motor to 30 degree
    lcd.clear();
    lcd.print("MOTOR ROTATING");
  //Go to 30 degree clockwise
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 180; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(2000); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(2000); 
  }
  delay(1000); // One second delay

  //Return back to zero degree
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 180; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(2000);
  }
  delay(1000);

  //Go 30 degree anticlockwise
    digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 180; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(2000);
  }
  delay(1000);

   //comeback to zero degrees
   digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 180; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(2000); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(2000); 
  }
  delay(1000); // One second delay

  ToRotate=2;
  }
      //Rotating motor to 180 degree
//    digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
//  // Makes 200 pulses for making one full cycle rotation
//  for(int x = 0; x < 1000; x++) {
//    digitalWrite(stepPin,HIGH); 
//    delayMicroseconds(500); 
//    digitalWrite(stepPin,LOW); 
//    delayMicroseconds(500); 
//  }
//  delay(1000); // One second delay
//
//   
//    digitalWrite(dirPin,LOW); //Changes the rotations direction
//  // Makes 400 pulses for making two full cycle rotation
//  for(int x = 0; x < 1000; x++) {
//    digitalWrite(stepPin,HIGH);
//    delayMicroseconds(500);
//    digitalWrite(stepPin,LOW);
//    delayMicroseconds(500);
//  }
//  delay(1000);    
  }

void CheckHumidity(){
  Serial.println("Checking humidity. to turn on fan if humidity above 85%");
  Serial.println(Humid);
  if(Humid>MaxHumidVal)
  {
    lcd.clear();
    lcd.print("Humid>85%");
    Serial.println("Humidity above 85 percent turn on Fan");
    digitalWrite(RelayPinFan,LOW);
    }

    else{
      lcd.clear();
    lcd.print("Humid<85%");
    Serial.println("Humidity below 85 percent turn OFF Fan");
    digitalWrite(RelayPinFan,HIGH);
      }
  }


void CheckKeypad() {
Serial.println("Write using keypad Temp value");
int num = 0;
int num1=0;
  int buttonState = digitalRead(pushButton);
  if(buttonState==HIGH){
    Serial.println("##### BUTTON IS HIGH So input keypad values #####");
    lcd.print("Enter temp");
    delay(2000);
    
   char key = keypad.getKey();
   while(key != '#')
   {
      switch (key)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            lcd.print(key);
            num = num * 10 + (key - '0');
            break;

         case '*':
            num = 0;
            lcd.clear();
            break;
      }

      key = keypad.getKey();
   }

   Serial.println(num);
   lcd.print("Set Temp:");
   lcd.print(num);
   Serial.println(num);
   delay(1000);
   lcd.clear();
   MaxTempVal=num;

   //////////// SET HUMIDITY THROUGH KEYPAD //////////////////////////////

       lcd.print("Enter Humid");
    delay(2000);
    
   char key1 = keypad.getKey();
   while(key1 != '#')
   {
      switch (key1)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            lcd.print(key1);
            num1 = num1 * 10 + (key1 - '0');
            break;

         case '*':
            num1 = 0;
            lcd.clear();
            break;
      }

      key1 = keypad.getKey();
   }

   Serial.println(num1);
   lcd.print("Set Temp:");
   lcd.print(num1);
   Serial.println(num1);
   delay(1000);
   lcd.clear();
   MaxHumidVal=num1;

    }
    else
    {
    Serial.println("##### BUTTON IS  LOWW #####");
    }
   
   
}
//////////////////////////////////////////////////////////////////////////////////////////