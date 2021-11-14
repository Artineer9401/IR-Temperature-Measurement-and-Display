/***********************
Name - Shubham Panigrahi
************************/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
 
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)

#define SERVO_PIN 6
#define BUZZ_PIN 9
#define PRX_PIN 2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

Servo testServo;
 
double ambTemp;
double objTemp;
bool inProximity;

void setup()
{
  testServo.attach(SERVO_PIN);               //Bind the pin with Servo object
  Serial.begin(9600);
  mlx.begin();                               //Initialize MLX90614
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  pinMode (BUZZ_PIN, OUTPUT);
  pinMode (PRX_PIN, INPUT);
  
  testServo.write(0);                        //Initialize servo at 0 degrees
  Serial.println("Initializing");
 
  display.clearDisplay();                    //Clear the buffer
  display.setCursor(25,5);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Initializing!!");
  display.display();
  delay(1000);
}
 
void loop()
{
  double avgTemp = 0;
  int readingsToAvg = 16;
  
  ambTemp = mlx.readAmbientTempC();    //Reading room temperature and object temp
  objTemp = mlx.readObjectTempC();
  inProximity = digitalRead(PRX_PIN);   //Reads the Proximity Sensor. Checks whether object is in Proximity. if present, sensor outputs LOW value

  //Averaging Filter
  for(int i=0; i<readingsToAvg; ++i){
    avgTemp += objTemp;
    delay(1);
  }
  avgTemp /= readingsToAvg;
  
  //Serial Monitor
  Serial.print("Object temp = ");
  Serial.println(objTemp);

  if (!inProximity){
    //Object within Proximity
    testServo.write(45);
    if (objTemp <= 30.00) {
      displayText('TEMPR', avgTemp);
      
    }
    else if (objTemp > 30.00) {
      displayText('ALERT!!', avgTemp);
    }
    delay(500);
  }
  else {
    //Object out of range
    testServo.write(0);
    display.clearDisplay();
    display.fillRect(0, 0, 128, 32, BLACK);   //Displays a Black Rectangle on entire screen
    display.display();
  }
}

// Function to display the temperature and alert meassage on the OLED
void displayText(char *txt, double tempr){
   if(txt == 'TEMPR'){
      display.clearDisplay();             //Clear the buffer 
      display.setCursor(25,5);  
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.println(" Temperature");
      display.setCursor(25,15);
      display.setTextSize(2);
      display.print(tempr);
      display.print((char)247);           //For printing 'degrees' symbol
      display.print("C");
      display.display();
      //display.clearDisplay();
    }

    else if (txt == 'ALERT!!'){
      display.clearDisplay();
      display.setTextColor(BLACK,WHITE);//For displaying tempr in Inverted Colors. White -> Background, Black -> Text
      display.setCursor(25,5);  
      display.setTextSize(1);
      display.println("Temperature");
      display.setCursor(20,15);
      display.setTextSize(2);
      display.print(tempr);
      display.print((char)247);
      display.print("C");
      display.display();
      
      //Beeps the buzzer with 0.1s interval, at freq. 5kHz
      tone(BUZZ_PIN, 5000);
      delay(100);
      noTone(BUZZ_PIN);
      delay(100);
    }
  }

  
