// Rotary Encoder Inputs
#define CLK 19
#define DT 21
#define SW 35

int counter = 0; //// Value 
int currentStateCLK;
int lastStateCLK;
String currentDir =""; ////
unsigned long lastButtonPress = 0;

void setup() {
  
  // Set encoder pins as inputs
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  // Setup Serial Monitor
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
}

void loop() {
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    if (digitalRead(DT) != currentStateCLK) {
      counter ++;
      currentDir ="Right";
    } else {
      counter --;
      currentDir ="Left";
    }
   //////////////Config Rotation /////////
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }

  lastStateCLK = currentStateCLK; ///// laststate = state
  
  int btnState = digitalRead(SW);

  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
    }
    lastButtonPress = millis();
  }
  delay(1);
}
