#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library

#define I2C_ADDR    0x27  // Define I2C Address for controller
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define BACKLIGHT_PIN 3
#define LED_OFF  1
#define LED_ON  0

#define SW_PIN A0

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

String makingMethodsNames[]={"Grilled", "Sauteed", "Shallow-fried", "Deap-Fried"};
int sizeOfmakingMethodsNames = sizeof(makingMethodsNames) / sizeof(makingMethodsNames[0]);
int lastUsedMakingMethod = -1;

String ingridients[]={"Scallops", "Orange", "Cinnamon"};
int sizeOfingridients = sizeof(ingridients) / sizeof(ingridients[0]);
int lastUsedIngridient = -1;

String connectionWords[]={"with", "and", "on"};
int sizeOfconnectionWords = sizeof(connectionWords) / sizeof(connectionWords[0]);
int lastUsedConnectionWord = -1;

LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

void setup() 
{
    pinMode(SW_PIN, INPUT);
    digitalWrite(SW_PIN, HIGH);       // turn on pullup resistors
    Serial.begin(9600);
    lcd.begin(20, 4, LCD_8BITMODE);  // initialize the lcd

    lcd.setBacklightPin(BACKLIGHT_PIN, NEGATIVE);
    lcd.setBacklight(LED_ON);
    
    stringToLCD("Welcome Devora,     Would you like an   idea for food?");

}

void loop()  
{

    // read the state of the switch into a local variable:
    int reading = digitalRead(SW_PIN);

    // check to see if you just pressed the button 
    // (i.e. the input went from LOW to HIGH),  and you've waited 
    // long enough since the last press to ignore any noise:  

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
        //Serial.println(lastDebounceTime);
    } 

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState) {
            buttonState = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonState == LOW) {
                generateFoodName();
                //delay(2000);
                //lcd.setBacklight(LED_OFF);
            }
        }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;
}

void generateFoodName(){
    String recipe;

    recipe += generateSubName();
    recipe += " ";
    if (random(2)){
        int connectionWord = random(0, sizeOfconnectionWords - 1); 
        recipe += connectionWords[connectionWord];
        recipe += " ";
        recipe += generateSubName();
    }
    

    //String recipe = makingMethodsNames[methodIndex]  + String(" ") + ingridients[ingridient];
    
    stringToLCD(recipe);
}

String generateSubName(){
    String subRecipe;
//    int methodIndex = random(0, sizeOfmakingMethodsNames - 1);
    int methodIndex = getRandomValue(makingMethodsNames, sizeOfmakingMethodsNames, lastUsedMakingMethod);  //random(0, sizeOfmakingMethodsNames - 1);
    lastUsedMakingMethod = methodIndex;
    subRecipe += makingMethodsNames[methodIndex];
    
    subRecipe += " ";
    
    //int ingridient = random(0, sizeOfingridients - 1);
    int ingridient = getRandomValue(ingridients, sizeOfingridients, lastUsedIngridient);  //random(0, sizeOfmakingMethodsNames - 1);
    lastUsedIngridient = ingridient;
    subRecipe += ingridients[ingridient];
    return subRecipe;
}

int getRandomValue(String values[], int numOfValues, int lastValueIndex){
    int found = 0;
    int index = 0;
    //int stringArraySize = sizeof(values) / sizeof(values[0]);
    //Serial.print("size: ");
    //Serial.println(stringArraySize);                        

    while (!found){
       index = random(0, numOfValues - 1);
       if (index != lastValueIndex){
           found = 1;
           Serial.print("index: ");
           Serial.println(index);   
       }
    }
    return index;
}

void stringToLCD(String data) {
   int lineCount = 0;
   int lineNumber = 0;
   byte stillProcessing = 1;
   byte charCount = 1;
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.setBacklight(LED_ON);
   while(stillProcessing) {
        if (++lineCount > 20) {    // have we printed 20 characters yet (+1 for the logic)
             lineNumber += 1;
             lcd.setCursor(0,lineNumber);   // move cursor down
             lineCount = 1;
        }

        lcd.print(data[charCount - 1]);

        if (!data[charCount]) {   // no more chars to process?
             stillProcessing = 0;
        }
        charCount += 1;
   }
}
