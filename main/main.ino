#include "config.h"
#include <EEPROM.h>
#include "Buttons.h"



const unsigned long TWO_MINUTE       = 120000;
const unsigned long SIX_MINUTE       = 360000;

const unsigned long ONE_HOUR         = 3600000;
const unsigned long TWO_HOUR         = 7200000;
const unsigned long FOUR_HOUR        = 14400000;
const unsigned long TWENTY_FOUR_HOUR = 86400000;

bool NextContent = false;

unsigned long OverrideStartTime = 0;
unsigned long TropicalStartTime = 0;
unsigned long WinterStartTime = 0;
unsigned long NormalStartTime_2 = 0;
unsigned long NormalStartTime_4 = 0;
unsigned long NormalStartTime_24 = 0;
unsigned long PumpOnTime = 0;
unsigned long PumpOffAfter = 0;
unsigned long LastButtonPress = 0;
bool NormalTimers[3] = {false, false, false};


int Current_Comport = 20;
int Calibrated_Temp = 0;
int Differential_Temp = 3;

int CurrentPoolTemp = 15;
int CurrentRoofTemp = 15;


int Pool_Temp_Pump_On = 28;
int LastSavedTemp = 22;
int Roof_Temp = 30;

bool StartUp = true;
bool doDecimal = false;
bool LastDecimal = false;
bool flashDecimal = false;
bool LastScreenFlash = false;
unsigned long LastLEDLFlash = 0;
unsigned long LastDisplayUpdate = 0;
unsigned long StartupTime = 0;
bool inStablization = false;
byte H,L,U,T;

Button Display_Btn  (BUTTON_DISPLAY, BUTTON_DEBOUNCE);
Button Winter_Btn   (BUTTON_WINTER, BUTTON_DEBOUNCE);
Button Plus_Btn      (BUTTON_ADD, BUTTON_DEBOUNCE);
Button Minus_Btn      (BUTTON_SUB, BUTTON_DEBOUNCE);


working_modes         WorkingMode     = NORMAL_SUMMER;
working_modes         LastWorkingMode = NORMAL_SUMMER;
tropical_mode         TropicalMode    = TROPICAL_NONE;
override_modes        OverrideMode    = OVERRIDE_NONE;
display_modes         Display         = BLANK;
display_modes         LastDisplay     = BLANK;
display_mode_content  DisplayContent  = ROOF_TEMP;
pump                  Pump            = PUMP_OFF;
unit                  Unit            = UNIT_ON;
stabilazation         Stabilization   = STABILIZATION_OFF;
sensors_check         SensorsCheck    = READ_TRUE;



void initPins(){
  Display_Btn.begin();
  Winter_Btn.begin();
  Plus_Btn.begin();
  Minus_Btn.begin();
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
    PumpOnTime = millis();
    digitalWrite(PUMP_PIN, ACTIVATE);
  }
}

void pumpOff(){
  if(Pump == PUMP_ON){
    Pump = PUMP_OFF;
    digitalWrite(PUMP_PIN, DEACTIVATE);
  }
}

void setup(){
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    while(!Serial){}
    Serial.println("[Main] Setup started");
  #endif
  initPins();

  Current_Comport   = readIntFromEEPROM(COMF_EEPROM_ADDR);
  Calibrated_Temp   = readIntFromEEPROM(CALI_EEPROM_ADDR);
  Differential_Temp = readIntFromEEPROM(DIFF_EEPROM_ADDR);
  
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    Serial.println("[Main] Setup complete");
  #endif

  H = digit14ToSegment[VERSION_MAJ][1];
  L = digit14ToSegment[VERSION_MAJ][0];
  T = digitToSegment[VERSION_MIN];
  U = 0;
  H = H | 64;
  Refresh();
  delay(1500);
  StartupTime = millis();
}

void loop(){
  readButtons();
  if(WorkingMode == OVERRIDE)
    CheckOverrideOperations();
  else if(WorkingMode == TROPICAL)
    CheckTropicalOperations();
  else if(WorkingMode == WINTER)
    CheckWinterOperations();
  else if(WorkingMode == NORMAL_SUMMER)
    CheckNormalSummerOperations();


  if(millis() - LastDisplayUpdate >= 500)
    UpdateDisplay();


  if(Display == COMFORT || Display == CALIBRATION || Display == DIFFERENTIAL){
    if(millis() - LastButtonPress >= 5000){
      if(Display == COMFORT)
        writeIntIntoEEPROM(COMF_EEPROM_ADDR, Current_Comport);
      else if(Display == CALIBRATION)
        writeIntIntoEEPROM(CALI_EEPROM_ADDR, Calibrated_Temp);
      else if(Display == DIFFERENTIAL)
        writeIntIntoEEPROM(DIFF_EEPROM_ADDR, Differential_Temp);
      Display = LastDisplay;
      Serial.println("Data saved");
    }
  }
}


void readButtons(){
  if(Winter_Btn.read()){
    delay(300);
    Display_Btn.read();
    Plus_Btn.read();
    Minus_Btn.read();
  }
  else if(Minus_Btn.read()){
    delay(300);
    Winter_Btn.read();
    Plus_Btn.read();
  }
  else if(Plus_Btn.read()){
    delay(300);
    Display_Btn.read();
    Minus_Btn.read();
    Winter_Btn.read();
  }
  else if(Display_Btn.read()){
    delay(300);
    Winter_Btn.read();
    Plus_Btn.read();
  }
  

///////////////////////////// Working Override On Mode //////////////////////////////////////

  if(Plus_Btn.wasPressed() && Minus_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed + and -");
    #endif
    if(WorkingMode != OVERRIDE){
      LastWorkingMode = WorkingMode;
      WorkingMode     = OVERRIDE;
    }
    while(Plus_Btn.read() && Minus_Btn.read()){
      if(Plus_Btn.pressedFor(5000) && Minus_Btn.pressedFor(5000)){
        if(OverrideMode != OVERRIDE_ON4){
          OverrideMode  = OVERRIDE_ON4;
          #ifdef DEBUGGING
            Serial.println("[Mode] Override ON 4 selected");
          #endif
          OverrideStartTime = millis();
        }
        return;
      }
    }
    if(OverrideMode == OVERRIDE_ON1){
      WorkingMode = LastWorkingMode;
      OverrideMode = OVERRIDE_NONE;
      #ifdef DEBUGGING
        Serial.println("[Mode] Going last working mode");
      #endif
    }
    else{
      OverrideMode = OVERRIDE_ON1;
      #ifdef DEBUGGING
        Serial.println("[Mode] Override ON 1 selected");
      #endif
      OverrideStartTime = millis();
    }
  }


///////////////////////////// Working Override Off Mode //////////////////////////////////////

  else if(Plus_Btn.wasPressed() && Display_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed + and display");
    #endif
    if(WorkingMode != OVERRIDE){
      LastWorkingMode = WorkingMode;
      WorkingMode     = OVERRIDE;
    }
    while(Plus_Btn.read() && Display_Btn.read()){
      if(Plus_Btn.pressedFor(5000) && Display_Btn.pressedFor(5000)){
        if(OverrideMode != OVERRIDE_OFF_PER){
          OverrideMode = OVERRIDE_OFF_PER;
          SensorsCheck = READ_FALSE;
          Unit = UNIT_OFF;
          #ifdef DEBUGGING
            Serial.println("[Mode] Override OFF permanent selected");
          #endif
          OverrideStartTime = millis();
          return;
        }
      }
    }
    if(OverrideMode == OVERRIDE_OFF_MOM){
      WorkingMode = LastWorkingMode;
      OverrideMode = OVERRIDE_NONE;
      #ifdef DEBUGGING
        Serial.println("[Mode] Going last working mode");
      #endif
    }
    else{
      OverrideMode = OVERRIDE_OFF_MOM;
      SensorsCheck = READ_TRUE;
      #ifdef DEBUGGING
        Serial.println("[Mode] Override OFF momentary selected");
      #endif
      OverrideStartTime = millis();
    }
  }


///////////////////////////// Working Tropical Mode //////////////////////////////////////

  else if(Winter_Btn.wasPressed() && Display_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed winter and display");
    #endif
    while(Winter_Btn.read() && Display_Btn.read()){
      if(Winter_Btn.pressedFor(5000) && Display_Btn.pressedFor(5000)){
        if(WorkingMode != TROPICAL){
          LastWorkingMode = WorkingMode;
          WorkingMode     = TROPICAL;
          TropicalMode    = TROPICAL_SUMMER_COOLING;
          digitalWrite(LED_TROPICAL, HIGH);
          TropicalStartTime = millis();
          #ifdef DEBUGGING
            Serial.println("[Mode] Tropical mode selected");
          #endif
          return;
        }
      }
    }
    if(WorkingMode == TROPICAL){
      WorkingMode = LastWorkingMode;
      #ifdef DEBUGGING
        Serial.println("[Mode] Last working mode selected");
      #endif
    }
  }

///////////////////////////// Display Diff Mode //////////////////////////////////////

  else if(Winter_Btn.wasPressed() && Minus_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed winter and -");
    #endif
    if(Unit != UNIT_OFF && WorkingMode == NORMAL_SUMMER){
      if(Display != DIFFERENTIAL && Display != DIFF){
        LastDisplay = Display;
        Display = DIFF;
        UpdateDisplay();
        delay(1000);
        LastButtonPress = millis();
      }
    }
  }



///////////////////////////// Display Calibration Mode //////////////////////////////////////

  else if(Winter_Btn.wasPressed() && Plus_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed winter and +");
    #endif
    if(Unit != UNIT_OFF && WorkingMode == NORMAL_SUMMER){
      if(Display != CALIBRATION && Display != CAL){
        LastDisplay = Display;
        Display = CAL;
        UpdateDisplay();
        while(Winter_Btn.read() && Plus_Btn.read()){}
        delay(1000);
        
        LastButtonPress = millis();
      }
      else{
        Display = LastDisplay;
      }
    }
  }

  

///////////////////////////// Display Display Mode //////////////////////////////////////
  
  else if(Display_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed display");
    #endif
    if(WorkingMode != WINTER && Unit != UNIT_OFF && WorkingMode != OVERRIDE){
      while(Display_Btn.read()){
        if(Display_Btn.pressedFor(5000)){
          //run pump for 3 sec
          // display P decimal and current pool temp
          PumpOffAfter = 3000;
          return;
        }
      }
    }
    if(Display != DISPLAY_CONTENT){
      LastDisplay = Display;
      DisplayContent = ROOF_TEMP;
    }
    else
      NextContent = true;
  }

///////////////////////////// Working Winter Mode //////////////////////////////////////

  else if(Winter_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed winter");
    #endif
    if(WorkingMode == TROPICAL)
      digitalWrite(LED_TROPICAL, LOW);
    if(WorkingMode != WINTER){
      WorkingMode = WINTER;
      WinterStartTime = millis();
      #ifdef DEBUGGING
        Serial.println("[Mode] Winter selected");
      #endif
    }
    else{
      WorkingMode = NORMAL_SUMMER;
      NormalStartTime_24 = NormalStartTime_4 = NormalStartTime_2 = millis();
      for(uint8_t i=0; i<3; i++)
        NormalTimers[i] = true;
      
      #ifdef DEBUGGING
        Serial.println("[Mode] Normal selected");
      #endif
    }
  }

  else if(Plus_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed +");
    #endif
    if(WorkingMode == NORMAL_SUMMER){
      if(Display == CALIBRATION || Display == CAL){
        Display = CALIBRATION;
        Calibrated_Temp += 1;
        if(Calibrated_Temp > MAX_CALIBRATION_TEMP)
          Calibrated_Temp = MAX_CALIBRATION_TEMP;
        UpdateDisplay();
        while(Plus_Btn.read()){
          if(Plus_Btn.pressedFor(500)){
            while(Plus_Btn.read()){
              if(Plus_Btn.pressedFor(INCREMENT_AFTER)){
                Calibrated_Temp += INCREMENT_DECEMENT_FACTOR;
                if(Calibrated_Temp > MAX_CALIBRATION_TEMP)
                  Calibrated_Temp = MAX_CALIBRATION_TEMP;
                UpdateDisplay();
              }
            }
          }
        }
      }
      else if(Display == DIFFERENTIAL || Display == DIFF){
        Display = DIFFERENTIAL;
        Differential_Temp += 1;
        if(Differential_Temp > MAX_DIFFERENTIAL_TEMP)
          Differential_Temp = MAX_DIFFERENTIAL_TEMP;
        UpdateDisplay();
        while(Plus_Btn.read()){
          if(Plus_Btn.pressedFor(500)){
            while(Plus_Btn.read()){
              if(Plus_Btn.pressedFor(INCREMENT_AFTER)){
                Differential_Temp += INCREMENT_DECEMENT_FACTOR;
                if(Differential_Temp > MAX_DIFFERENTIAL_TEMP)
                  Differential_Temp = MAX_DIFFERENTIAL_TEMP;
                UpdateDisplay();
              }
            }
          }
        }
      }
      else if(Display != COMFORT){
        #ifdef DEBUGGING
          Serial.println("[Display] Comfort");
        #endif
        LastDisplay = Display;
        Display = COMFORT;
        LastButtonPress = millis();
      }
      else if(Display == COMFORT){
        #ifdef DEBUGGING
          Serial.println("[Display] Comfort +");
        #endif
        Current_Comport += INCREMENT_DECEMENT_FACTOR;
        if(Current_Comport > MAX_COMFORT_TEMP)
          Current_Comport = MAX_COMFORT_TEMP;
        UpdateDisplay();
        while(Plus_Btn.read()){
          if(Plus_Btn.pressedFor(500)){
            while(Plus_Btn.read()){
              if(Plus_Btn.pressedFor(INCREMENT_AFTER)){
                Current_Comport += INCREMENT_DECEMENT_FACTOR;
                if(Current_Comport > MAX_COMFORT_TEMP)
                  Current_Comport = MAX_COMFORT_TEMP;
                UpdateDisplay();
              }
            }
          }
        }
      }
      LastButtonPress = millis();
    }
  }

  else if(Minus_Btn.wasPressed()){
    #ifdef DEBUGGING
      Serial.println("[Button] Pressed -");
    #endif
    if(WorkingMode == NORMAL_SUMMER){
      if(Display == CALIBRATION || Display == CAL){
        Display = CALIBRATION;
        Calibrated_Temp -= 1;
        if(Calibrated_Temp < MIN_CALIBRATION_TEMP)
          Calibrated_Temp = MIN_CALIBRATION_TEMP;
        UpdateDisplay();
        while(Minus_Btn.read()){
          if(Minus_Btn.pressedFor(500)){
            while(Minus_Btn.read()){
              if(Minus_Btn.pressedFor(INCREMENT_AFTER)){
                Calibrated_Temp -= INCREMENT_DECEMENT_FACTOR;
                if(Calibrated_Temp < MIN_CALIBRATION_TEMP)
                  Calibrated_Temp = MIN_CALIBRATION_TEMP;
                UpdateDisplay();
              }
            }
          }
        }
      }
      else if(Display == DIFFERENTIAL || Display == DIFF){
        Display = DIFFERENTIAL;
        Differential_Temp -= 1;
        if(Differential_Temp < MIN_DIFFERENTIAL_TEMP)
          Differential_Temp = MIN_DIFFERENTIAL_TEMP;
        UpdateDisplay();
        while(Minus_Btn.read()){
          if(Minus_Btn.pressedFor(500)){
            while(Minus_Btn.read()){
              if(Minus_Btn.pressedFor(INCREMENT_AFTER)){
                Differential_Temp -= INCREMENT_DECEMENT_FACTOR;
                if(Differential_Temp < MIN_DIFFERENTIAL_TEMP)
                  Differential_Temp = MIN_DIFFERENTIAL_TEMP;
               UpdateDisplay();
              }
            }
          }
        }
      }
      else if(Display != COMFORT){
        LastDisplay = Display;
        Display = COMFORT;
        LastButtonPress = millis();
      }
      else if(Display == COMFORT){
        Current_Comport -= INCREMENT_DECEMENT_FACTOR;
        if(Current_Comport < MIN_COMFORT_TEMP)
          Current_Comport = MIN_COMFORT_TEMP;
        UpdateDisplay();
        while(Minus_Btn.read()){
          if(Minus_Btn.pressedFor(500)){
            while(Minus_Btn.read()){
              if(Minus_Btn.pressedFor(INCREMENT_AFTER)){
                Current_Comport -= INCREMENT_DECEMENT_FACTOR;
                if(Current_Comport < MIN_COMFORT_TEMP)
                  Current_Comport = MIN_COMFORT_TEMP;
                UpdateDisplay();
              }
            }
          }
        }
      }
      LastButtonPress = millis();
    }
  }
}


void readAllSensors(){
  
}

void writeIntIntoEEPROM(int addr, int number){ 
  EEPROM.write(addr, number >> 8);
  EEPROM.write(addr + 1, number & 0xFF);
}

int readIntFromEEPROM(int addr){
  byte byte1 = EEPROM.read(addr);
  byte byte2 = EEPROM.read(addr + 1);
  return (byte1 << 8) + byte2;
}
