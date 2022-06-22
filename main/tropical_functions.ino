void SummerTurnOndelays(){
  if(StartUp){
    flashDecimal = doDecimal = true;
    Display = LLPT;
    if(millis() - StartupTime >= TWO_MINUTE){
      doDecimal = StartUp = false;
    }
  }
  else if(Stabilization ==   STABILIZATION_ON){
    StartupTime = millis();
    while(millis() - StartupTime <= SIX_MINUTE){
      if(Pump == PUMP_OFF)
        pumpOn();
      Display = PACT;
    }
  }
}


void CheckTropicalOperations(){
  if(TropicalMode == TROPICAL_SUMMER_HEAT){
    if(Pump == PUMP_ON){
      if(CurrentPoolTemp < Current_Comport){
        if(CurrentRoofTemp > Current_Comport){
          if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
            Display = PACT;
        }
        else
          ClosePumpTropical();
      }
      else if(CurrentPoolTemp == Current_Comport)
        ClosePumpTropical();
      else if(CurrentPoolTemp > Current_Comport){
        TropicalMode = TROPICAL_SUMMER_COOLING;
      }
    }
    else{
      if(LastSavedTemp > Current_Comport){
        TropicalMode = TROPICAL_SUMMER_COOLING;
      }
      else{
        if(millis() - TropicalStartTime < FOUR_HOUR){
          if(LastSavedTemp < Current_Comport){
             if(CurrentRoofTemp > LastSavedTemp){
               // 6min. stabilization
             }
          }
        }
        else{
          // 6minute stabilization
        }
      }
    }
  }
  else if(TropicalMode == TROPICAL_SUMMER_COOLING){
    if(Pump == PUMP_ON){
      if(CurrentPoolTemp < Current_Comport){
        TropicalMode = TROPICAL_SUMMER_HEAT;
      }
      else if(CurrentPoolTemp > Current_Comport){
        if(CurrentRoofTemp < Current_Comport){
          if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
            Display = PACT;
        }
        else
          ClosePumpTropical();
      }
      else
        ClosePumpTropical();
    }
    else{
      if(LastSavedTemp > Current_Comport){
        if(millis() - TropicalStartTime >= FOUR_HOUR){
          // 6min stabilization
        }
        else{
          if(CurrentRoofTemp < Current_Comport){
            // 6min stabiliazation
          }
          else{
            if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
              Display = LTEMP;
          }
        }
      }
      else{
        TropicalMode = TROPICAL_SUMMER_HEAT;
      }
    }
  }
}


void ClosePumpTropical(){
  LastSavedTemp = CurrentPoolTemp;
  writeIntIntoEEPROM(LAST_POOL_EEPROM_ADDR, LastSavedTemp);
  pumpOff();
  TropicalStartTime = millis();
}
