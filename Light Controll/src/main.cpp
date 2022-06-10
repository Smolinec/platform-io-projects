#include <Arduino.h>

/**
 * Config relay pins
 */
const int relay1 = 2;
const int relay2 = 3;
const int relay3 = 4;
const int relay4 = 5;
const int relay5 = 6;
const int relay6 = 7;

/**
 * Config Button pins
 */
const int button1 = 8;
const int button2 = 9;
const int button3 = 10;
const int button4 = 11;
const int button5 = 12;
const int button6 = 13;

/**
 * @brief 
 * 
 */
enum State{
  ON, OFF
};

/**
 * @brief 
 * 
 */

enum RelayEnum{
  REPLAY_1,
  REPLAY_2,
  REPLAY_3,
  REPLAY_4,
  REPLAY_5,
  REPLAY_6
};

typedef struct{
  RelayEnum relayEnum;
  State state;
} Relay;

/**
 * @brief 
 * 
 */
enum ButtonEnum{
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_5,
  BUTTON_6
};

/**
 * @brief
 * 
 */
void setupRelay(){
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);

  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  digitalWrite(relay5, LOW);
  digitalWrite(relay6, LOW);
}

/**
 * @brief 
 * 
 */
void setupButton(){
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(button6, INPUT);
}

Relay relays[6] = {{REPLAY_1, OFF}, {REPLAY_2, OFF}, {REPLAY_3, OFF}, {REPLAY_4, OFF}, {REPLAY_5, OFF}, {REPLAY_6, OFF}};

/**
 * @brief 
 * 
 */
void setup() {
  /**
   * @brief 
   * 
   */
  Serial.begin(57600);

  /**
   * @brief Construct a new setup Relay object
   * 
   */
  setupRelay();

  /**
   * @brief Construct a new setup Button object
   * 
   */
  setupButton();
}


/**
 * @brief 
 * 
 * @param relay 
 */
void changeStateRelay(RelayEnum relayEnum){
  switch (relayEnum){
    case REPLAY_1:
      if (relays[0].state == OFF){
        digitalWrite(relay1, HIGH);
        relays[0].state = ON;
        Serial.println("Change state for relay 1 to: ON");
      }else{
        digitalWrite(relay1, LOW);
        relays[0].state = OFF;
        Serial.println("Change state for relay 1 to: OFF");
      }
      break;
    case REPLAY_2:
      if (relays[1].state == OFF){
        digitalWrite(relay2, HIGH);
        relays[1].state = ON;
        Serial.println("Change state for relay 2 to: ON");
      }else{
        digitalWrite(relay2, LOW);
        relays[1].state = OFF;
        Serial.println("Change state for relay 2 to: OFF");
      }
      break;
    case REPLAY_3:
      if (relays[2].state == OFF){
        digitalWrite(relay3, HIGH);
        relays[2].state = ON;
        Serial.println("Change state for relay 3 to: ON");
      }else{
        digitalWrite(relay3, LOW);
        relays[2].state = OFF;
        Serial.println("Change state for relay 3 to: OFF");
      }
      break;
    case REPLAY_4:
      if (relays[3].state == OFF){
        digitalWrite(relay4, HIGH);
        relays[3].state = ON;
        Serial.println("Change state for relay 4 to: ON");
      }else{
        digitalWrite(relay4, LOW);
        relays[3].state = OFF;
        Serial.println("Change state for relay 4 to: OFF");
      }
      break;
    case REPLAY_5:
      if (relays[4].state == OFF){
        digitalWrite(relay5, HIGH);
        relays[4].state = ON;
        Serial.println("Change state for relay 5 to: ON");
      }else{
        digitalWrite(relay5, LOW);
        relays[4].state = OFF;
        Serial.println("Change state for relay 5 to: OFF");
      }
      break;
    case REPLAY_6:
      if (relays[5].state == OFF){
        digitalWrite(relay6, HIGH);
        relays[5].state = ON;
        Serial.println("Change state for relay 6 to: ON");
      }else{
        digitalWrite(relay6, LOW);
        relays[5].state = OFF;
        Serial.println("Change state for relay 6 to: OFF");
      }
      break;
    default:
      Serial.println("Error");
      break;
  }
}

/**
 * @brief 
 * 
 */
void loop() {
  // put your main code here, to run repeatedly:
  int button1State = digitalRead(button1);
  if (button1State == HIGH){
    Serial.println("Button 1 switch ON");
    changeStateRelay(REPLAY_1);
  }
  int button2State = digitalRead(button2);
  if (button2State == HIGH){
    Serial.println("Button 2 switch ON");
    changeStateRelay(REPLAY_2);
  }
  int button3State = digitalRead(button3);
  if (button3State == HIGH){
    Serial.println("Button 3 switch ON");
    changeStateRelay(REPLAY_3);
  }
  int button4State = digitalRead(button4);
  if (button4State == HIGH){
    Serial.println("Button 4 switch ON");
    changeStateRelay(REPLAY_4);
  }
  int button5State = digitalRead(button5);
  if (button5State == HIGH){
    Serial.println("Button 5 switch ON");
    changeStateRelay(REPLAY_5);
  }
  int button6State = digitalRead(button6);
  if (button6State == HIGH){
    Serial.println("Button 6 switch ON");
    changeStateRelay(REPLAY_6);
  }
}