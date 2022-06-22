void CheckNormalSummerOperations(){
  if(Pump == PUMP_ON){
    if(CurrentPoolTemp < Current_Comport){
      if(CurrentRoofTemp > Current_Comport){
        if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
          Display = PACT;
      }
      else{
        ClosePumpNormal();
        if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
          Display = LDER;
        for(uint8_t i=1; i<3; i++)
          NormalTimers[i] = true;
      }
    }
    else if(CurrentPoolTemp == Current_Comport){
      ClosePumpNormal();
      for(uint8_t i=0; i<3; i++)
        NormalTimers[i] = true;
    }
  }
  else{
    bool twenty_four = false;
    bool four = false;
    bool one = false;
    if(millis() - NormalStartTime_24 >= TWENTY_FOUR_HOUR && NormalTimers[0]){
      // check if unit not running for 24 hours
      // if true 6 min stabilization
      twenty_four = true;
      NormalStartTime_24 = millis();
    }
    if(millis() - NormalStartTime_4 >= FOUR_HOUR && NormalTimers[1]){
      // drate last temp temp by 1 deg
      four = true;
      NormalStartTime_4 = millis();
    }
    if(millis() - NormalStartTime_2 >= TWO_HOUR && NormalTimers[3]){
      one = true;
      NormalStartTime_2 = millis();
    }
    if(one && four && twenty_four){
      if(CurrentRoofTemp > LastSavedTemp){
        // 6 min. stabilization
      }
    }
    if(Display != COMFORT && Display != CALIBRATION && Display != DIFFERENTIAL && Display != CAL && Display != DIFF)
      Display = LDER;
  }
}


void CheckWinterOperations(){
  doDecimal = false;
  if(StartUp){
    doDecimal = true;
    if(millis() - WinterStartTime >= TWO_MINUTE){
      StartUp = false;
      WinterStartTime = millis();
    }
  }
  else{
    if(Pump == PUMP_ON){
      if(millis() - PumpOnTime >= SIX_MINUTE)
        pumpOff();
    }
    else{
      if(millis() - WinterStartTime >= TWENTY_FOUR_HOUR){
        pumpOn();
        WinterStartTime = millis();
      }
    }
  }
}


void ClosePumpNormal(){
  LastSavedTemp = CurrentPoolTemp;
  writeIntIntoEEPROM(LAST_POOL_EEPROM_ADDR, LastSavedTemp);
  pumpOff();
  NormalStartTime_2 = millis();
  NormalStartTime_4 = millis();
  NormalStartTime_24 = millis();
}
