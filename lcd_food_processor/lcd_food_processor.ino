#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library
#include <avr/sleep.h>
#include <avr/power.h>

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

#define SWITCH_PIN 2
#define TIME_FOR_SPECIAL_MODE_MS 1000
#define TIME_TO_ENTER_SLEEP_MODE_MS 20000
// Variables will change:
int buttonState = HIGH;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
int buttonPushStarted = 0;
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
long buttonOnStartTime;
int wokeUpFromSleep = 0;



String makingMethodsNames[]={"Grilled", "Sauteed", "Deap-Fried", "Fresh", 
                             "Diced", "Chopped", "Roasted", "Fried", "Frozen", "Grated"};
int sizeOfmakingMethodsNames = sizeof(makingMethodsNames) / sizeof(makingMethodsNames[0]);
int lastUsedMakingMethod = -1;

String ingridients[]={"Scallops", "Orange", "Cinnamon", "Cheese", "Salmon", "Tomato", 
                      "Chicken Wings", "Goat milk", "Liver", "Cucumber", "Garlic", "Corn", 
                      "Sherry tomatos", "Green Tomatos", "Potato", "Potatos", "Egg plant", 
                      "Kale", "Mango", "Water Melon", "Cashew", "PineApple",};
int sizeOfingridients = sizeof(ingridients) / sizeof(ingridients[0]);
int lastUsedIngridient = -1;

String afterMethods[]={"Soup", "Salad", "Ice-Cream"};
int sizeOfafterMethods = sizeof(afterMethods) / sizeof(afterMethods[0]);
int lastUsedafterMethods = -1;

String connectionWords[]={"with", "and", "on", "coated with"};
int sizeOfconnectionWords = sizeof(connectionWords) / sizeof(connectionWords[0]);
int lastUsedConnectionWord = -1;

String otherSentances[]={"Maybe just take a hike?", "Isn't today your Birthday?", "You should call 0524686127", "This is an awesome gadget"};
int sizeOfOtherSentances = sizeof(otherSentances) / sizeof(otherSentances[0]);
int lastUsedOtherSentances = -1;

LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);


void setup() 
{
    randomSeed(analogRead(A1));
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    
    //attachInterrupt(digitalPinToInterrupt(interruptPin), isrButton, CHANGE);
    
    //Serial.begin(9600);
    //lcd.createChar(0, customChar);
    lcd.begin(20, 4, LCD_8BITMODE);  // initialize the lcd

    lcd.setBacklightPin(BACKLIGHT_PIN, NEGATIVE);
    great();

}


void great(){
    //Serial.println("in Great");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.setBacklight(LED_ON);
    stringToLCD("Welcome Devora,     Would you like an   idea for food?");
}
void loop()  
{
    if (!wokeUpFromSleep){
        setButtonState();
    }
    else{
        delay(500);
        wokeUpFromSleep = 0;
        great();
    }

}

void setButtonState(void){
    int reading = digitalRead(SWITCH_PIN);

    // check to see if you just pressed the button 
    // (i.e. the input went from LOW to HIGH),  and you've waited 
    // long enough since the last press to ignore any noise:  

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
        //Serial.println(lastDebounceTime);
    }
    
    if ((millis() - lastDebounceTime) < TIME_TO_ENTER_SLEEP_MODE_MS ){
        //Serial.print(millis());
        //Serial.print(", reading: ");
        //Serial.println(reading);        

        delay(50);
    }
    else{
        enterSleep();
        wokeUpFromSleep = 1;
        //Serial.println("waking up");
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        
        // if the button state has changed:
        if (reading != buttonState) {
            buttonState = reading;
            
            if (buttonState == LOW){
                if (!buttonPushStarted){
                    buttonOnStartTime = millis();
                    buttonPushStarted = 1;
                }
            }
            
            // only toggle the LED if the new button state is HIGH
            if (buttonState == HIGH) {
                
                //Serial.print("button catched");
                //Serial.println(millis() - buttonOnStartTime);
                
                if ((millis() - buttonOnStartTime) > TIME_FOR_SPECIAL_MODE_MS){
                    specialMode();
                }
                else{
                    introAnimation();
                    generateFoodName();
                    //delay(2000);
                    //lcd.setBacklight(LED_OFF);
                }
                buttonPushStarted = 0;
            }

        }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;
}

void pin2Interrupt(void)
{
      /* This will bring us back from sleep. */
      
      /* We detach the interrupt to stop it from 
       * continuously firing while the interrupt pin
       * is low.
       */
      detachInterrupt(0);
}

void enterSleep(void)
{
      //Serial.println("going to sleep");
      lcd.setBacklight(LED_OFF);
      /* Setup pin2 as an interrupt and attach handler. */
      attachInterrupt(0, pin2Interrupt, LOW);
      delay(100);
      
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      
      sleep_enable();
      
      sleep_mode();
      
      /* The program will continue from here. */
      
      /* First thing to do is disable sleep. */
      sleep_disable(); 
      lcd.setBacklight(LED_ON);    
}

void specialMode(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.setBacklight(LED_ON);
    stringToLCD("Happy Birthday 35!!!The best time to    start having fun.                Netanel");
    
}

void generateFoodName(){
    String recipe;
    if (random(100) >= 95){ // 5% to give some sentance - not a food name
        int sentanceIndex = getRandomValue(otherSentances, sizeOfOtherSentances, lastUsedOtherSentances);
        recipe += otherSentances[sentanceIndex];
    }
    else if (random(10) == 1){// soup, salad (ingridients + method)
        int ingridientIndex = getRandomValue(ingridients, sizeOfingridients, lastUsedIngridient);
        recipe += ingridients[ingridientIndex];
        lastUsedIngridient = ingridientIndex;
        
        recipe += " ";
        
        recipe += afterMethods[getRandomValue(afterMethods, sizeOfafterMethods, lastUsedafterMethods)]; 
    }
    else{
        recipe += generateSubName();
        recipe += " ";
        if (random(2)){ // 
            int connectionWord = random(0, sizeOfconnectionWords); 
            recipe += connectionWords[connectionWord];
            recipe += " ";
            recipe += generateSubName();
        }else if (random(4) == 1){
            recipe += "Soup"; 
        }
    }
    stringToLCD(recipe);
}

String generateSubName(){
    String subRecipe;
    int methodIndex = getRandomValue(makingMethodsNames, sizeOfmakingMethodsNames, lastUsedMakingMethod);
    lastUsedMakingMethod = methodIndex;
    subRecipe += makingMethodsNames[methodIndex];
    
    subRecipe += " ";
    
    int ingridient = getRandomValue(ingridients, sizeOfingridients, lastUsedIngridient);
    lastUsedIngridient = ingridient;
    subRecipe += ingridients[ingridient];
    
    return subRecipe;
}

int getRandomValue(String values[], int numOfValues, int lastValueIndex){
    int found = 0;
    int index = 0;                

    while (!found){
       index = random(0, numOfValues);
       if (index != lastValueIndex){
           found = 1;
       }
    }
    return index;
}

void introAnimation(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.setBacklight(LED_ON);
    for(int i=0; i< 20; i++){
        lcd.print((char)255);
        delay(110);
    }
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
             lcd.setCursor(0, lineNumber);   // move cursor down
             lineCount = 1;
        }

        lcd.print(data[charCount - 1]);

        if (!data[charCount]) {   // no more chars to process?
             stillProcessing = 0;
        }
        charCount += 1;
   }
}
