#include "config.h"
#include <EEPROM.h>
#include "Buttons.h"


typedef enum{
  AUTO_MODE,
  TROPICAL_MODE,
  OVERRIDE_MODE,
  WINTER_MODE,
}working_modes;

typedef enum{
  ON1,
  ON4,
  OFF,
  NONE,
}override_modes;

typedef enum{
  DISPLAY_CALIBRATION,
  DISPLAY_DISPLAY,
  DISPLAY_DIFFERENTIAL,
  DISPLAY_WINTER,
  DISPLAY_NORMAL,
  DISPLAY_COMFORT,
}display_modes;

typedef enum{
  ROOF_TEMP,
  POOL_TEMP,
  LAST_POOL_TEMP,
  COMFORT,
}display_modes_content;

typedef enum{
  PUMP_ON,
  PUMP_OFF,
}pump;


pump                  Pump          = PUMP_OFF;
working_modes         WorkingMode   = AUTO_MODE;
override_modes        OverrideMode  = NONE;
display_modes         DisplayMode   = DISPLAY_NORMAL;
display_modes_content DisplayContent = ROOF_TEMP;


uint32_t OverrideStartTime = 0;
uint32_t OverrideLastUpdate = 0;
bool LastOn = false;
uint32_t ContentLastUpdate = 0;

uint8_t Current_Comport = 20;
uint8_t Calibrated_Temp = 0;
float Pool_Temp_Pump_On = 28;
float Pool_Temp_Pump_Off = 22;
float Roof_Temp = 30;


Button b_Display(BUTTON_DISPLAY, BUTTON_DEBOUNCE);
Button b_Winter (BUTTON_WINTER, BUTTON_DEBOUNCE);
Button b_Add    (BUTTON_ADD, BUTTON_DEBOUNCE);
Button b_sub    (BUTTON_SUB, BUTTON_DEBOUNCE);


void updateDisplay(int number, String character, bool Char = false);

void initPins(){
  b_Display.begin();
  b_Winter.begin();
  b_Add.begin();
  b_sub.begin();
  pinMode(LED_TROPICAL , OUTPUT);
  pinMode(LED_OVERRIDE , OUTPUT);
  pinMode(DISPLAY_STROBE, OUTPUT);
  pinMode(DISPLAY_DATA, OUTPUT);
  pinMode(DISPLAY_CLOCK, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
}


void pumpOn(){
  if(Pump == PUMP_OFF){
    Pump = PUMP_ON;
    digitalWrite(PUMP_PIN, ACTIVATE);
  }
}

void pumpOff(){
  if(Pump == PUMP_ON){
    Pump = PUMP_OFF;
    digitalWrite(PUMP_PIN, DEACTIVATE);
  }
}



void changeDisplay(int i, String st){
  if(LastOn){
    LastOn = false;
    digitalWrite(LED_OVERRIDE, LastOn);
  }
  else{
    LastOn = true;
    digitalWrite(LED_OVERRIDE, LastOn);
    updateDisplay(i, st, true);
  }
}

void displayCalibrated(bool ShowCal = false){
  if(ShowCal){
    updateDisplay(-1, "CAL", true);
    delay(800);
  }
  if(Calibrated_Temp > 0){
    updateDisplay(Calibrated_Temp, "+", true);
  }
  else{
    updateDisplay(Calibrated_Temp, "-", true);
  }
}

void setup(){
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    while(!Serial){}
    Serial.println("[Main] Setup started");
  #endif
  initPins();
  Current_Comport = readComfortTemp();
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    Serial.println("[Main] Setup complete");
  #endif
}

void loop(){
  readButtons();
  if(WorkingMode == OVERRIDE_MODE){
    if(millis() - OverrideLastUpdate >= 500){
      if(OverrideMode == ON1){
        if(millis() - OverrideStartTime >= 3600000)
          WorkingMode = AUTO_MODE;
        else
          changeDisplay(1, "on");
      }
      else if(OverrideMode == ON4){
        if(millis() - OverrideStartTime >= 14400000)
          WorkingMode = AUTO_MODE;
        else
          changeDisplay(4, "on");
      }
      else if(OverrideMode == OFF){
        if(millis() - OverrideStartTime >= 3600000){
          WorkingMode = AUTO_MODE;
          #ifdef DEBUGGING
            Serial.println("[Working] Closing Override");
          #endif
        }
        else
          changeDisplay(-1, "oFF");
      }
      OverrideLastUpdate = millis();
    }
  }
  if(DisplayMode == DISPLAY_DISPLAY){
    if(millis() - ContentLastUpdate >= 500){
      if(DisplayContent == ROOF_TEMP){
        DisplayContent = POOL_TEMP;
        updateDisplay(Roof_Temp, "r", true);
      }
      else if(DisplayContent == POOL_TEMP){
        DisplayContent = LAST_POOL_TEMP;
        updateDisplay(Pool_Temp_Pump_On, "P", true);
      }
      else if(DisplayContent == LAST_POOL_TEMP){
        DisplayContent = COMFORT;
        updateDisplay(Pool_Temp_Pump_Off, "L", true);
      }
      else if(DisplayContent == COMFORT){
        DisplayContent = ROOF_TEMP;
        updateDisplay(Current_Comport, "C", true);
      }
      ContentLastUpdate = millis();
    }
  }
}


void readButtons(){
  if(b_Winter.read()){
    delay(300);
    b_Display.read();
    b_Add.read();
    b_sub.read();
  }
  else if(b_sub.read()){
    delay(300);
    b_Winter.read();
    b_Add.read();
  }
  else if(b_Add.read()){
    delay(300);
    b_Display.read();
    b_sub.read();
    b_Winter.read();
  }
  else if(b_Display.read()){
    delay(300);
    b_Winter.read();
    b_Add.read();
  }
  


///////////////////////////// Working Override Off Mode //////////////////////////////////////

  if(b_Add.wasPressed() && b_Display.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Display and + pressed");
    #endif
    if(WorkingMode != OVERRIDE_MODE){
      WorkingMode = OVERRIDE_MODE;
      OverrideMode = OFF;
      OverrideStartTime = millis();
      OverrideLastUpdate = 0;
    }
    else{
      WorkingMode = AUTO_MODE;
      OverrideMode = NONE;
      digitalWrite(LED_OVERRIDE, LOW);
    }
    return;
  }


///////////////////////////// Working Tropical Mode //////////////////////////////////////

  else if(b_Winter.wasPressed() && b_Display.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Display and winter pressed");
    #endif
    if(WorkingMode != TROPICAL_MODE){
      while(b_Winter.read() && b_Display.read()){
        if(b_Winter.pressedFor(5000) && b_Display.pressedFor(5000)){ 
          WorkingMode = TROPICAL_MODE;
          digitalWrite(LED_TROPICAL, HIGH);
          return;
        }
      }
    }
    else{
      WorkingMode = AUTO_MODE;
      digitalWrite(LED_TROPICAL, LOW);
    }
  }

///////////////////////////// Display Calibration Mode //////////////////////////////////////

  else if(b_Winter.wasPressed() && b_sub.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Winter and - pressed");
    #endif
    if(DisplayMode != DISPLAY_CALIBRATION){
      DisplayMode = DISPLAY_CALIBRATION;
      displayCalibrated(true);
    }
    else{
      DisplayMode = DISPLAY_NORMAL;
    }
  }


///////////////////////////// Display Diff Mode //////////////////////////////////////

  else if(b_Winter.wasPressed() && b_Add.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Winter and + pressed");
    #endif
  }

  
///////////////////////////// Working Override On Mode //////////////////////////////////////

  else if(b_Add.wasPressed() && b_sub.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] + and - pressed");
    #endif
    if(WorkingMode != OVERRIDE_MODE){
      OverrideMode = ON1;
      WorkingMode = OVERRIDE_MODE;
      while(b_Add.read() && b_sub.read()){
        if(b_Add.pressedFor(4000) && b_sub.pressedFor(4000)){
          OverrideMode = ON4;
        }
      }
      digitalWrite(LED_OVERRIDE, HIGH);
      LastOn = true;
      OverrideStartTime = millis();
    }
    else{
      WorkingMode = AUTO_MODE;
      OverrideMode = NONE;
      digitalWrite(LED_OVERRIDE, LOW);
    }
  }
///////////////////////////// Display Display Mode //////////////////////////////////////
  
  else if(b_Display.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Display pressed");
    #endif
    if(DisplayMode != DISPLAY_DISPLAY){
      DisplayMode = DISPLAY_DISPLAY;
      DisplayContent = ROOF_TEMP;
    }
    else
      DisplayMode = DISPLAY_NORMAL;
  }

///////////////////////////// Working Winter Mode //////////////////////////////////////

  else if(b_Winter.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Winter pressed");
    #endif
    if(WorkingMode != WINTER_MODE){
      if(WorkingMode == TROPICAL_MODE)
        digitalWrite(LED_TROPICAL, LOW);
      WorkingMode = WINTER_MODE;
    }
    else{
      WorkingMode = AUTO_MODE;
    }
  }

  else if(b_Add.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] + pressed");
    #endif
    if(DisplayMode == DISPLAY_CALIBRATION){
      Calibrated_Temp += 1;
      displayCalibrated();
    }
    else{
      if(DisplayMode != DISPLAY_COMFORT){
        DisplayMode = DISPLAY_COMFORT;
        updateDisplay(Current_Comport, "C", true);
        return;
      }
      else{
        while(b_Add.read()){
          if(b_Add.pressedFor(5000)){
            saveComfortTemp();
            DisplayMode = DISPLAY_NORMAL;
            return;
          }
        }
        Current_Comport += INCREMENT_FACTOR;
        if(Current_Comport > MAX_COMFORT_TEMP || Current_Comport < MIN_COMFORT_TEMP)
          Current_Comport = MAX_COMFORT_TEMP;
        updateDisplay(Current_Comport, "C", true);
      }
    }
  }

  else if(b_sub.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] - pressed");
    #endif
    if(DisplayMode == DISPLAY_CALIBRATION){
      Calibrated_Temp -= 1;
      displayCalibrated();
    }
    else{
      if(DisplayMode != DISPLAY_COMFORT){
        DisplayMode = DISPLAY_COMFORT;
        updateDisplay(Current_Comport, "C", true);
        return;
      }
      else{
        while(b_Add.read()){
          if(b_Add.pressedFor(5000)){
            saveComfortTemp();
            DisplayMode = DISPLAY_NORMAL;
            return;
          }
        }
        Current_Comport -= INCREMENT_FACTOR;
        if(Current_Comport < MIN_COMFORT_TEMP || Current_Comport > MAX_COMFORT_TEMP)
          Current_Comport = MIN_COMFORT_TEMP;
        updateDisplay(Current_Comport, "C", true);
      }
    }
  }
}


void readAllSensors(){
  
}


void saveComfortTemp(){
  EEPROM.write(COMF_EEPROM_ADDR, Current_Comport);
  #ifdef DEBUGGING
    Serial.println("[Comfort] Saved");
  #endif
}


uint8_t readComfortTemp(){
  #ifdef DEBUGGING
    Serial.println("[Comfort] Reading");
  #endif
  return EEPROM.read(COMF_EEPROM_ADDR);
}
