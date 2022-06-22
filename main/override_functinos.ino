void CheckOverrideOperations(){
  if(OverrideMode == OVERRIDE_ON1){
    if(millis() - OverrideStartTime >= ONE_HOUR)
      ExitOverride();
    else if(Pump == PUMP_OFF)
      pumpOn();
  }
  else if(OverrideMode == OVERRIDE_ON4){
    if(millis() - OverrideStartTime >= ONE_HOUR)
      ExitOverride();
    else if(Pump == PUMP_OFF)
      pumpOn();
  }
  else if(OverrideMode == OVERRIDE_OFF_PER){
    if(millis() - OverrideStartTime >= ONE_HOUR)
      Unit = UNIT_ON;
    if(Pump == PUMP_ON)
      pumpOff();
  }
  else if(OverrideMode == OVERRIDE_OFF_MOM){
    if(millis() - OverrideStartTime >= ONE_HOUR)
      ExitOverride();
    else if(Pump == PUMP_ON)
      pumpOff();
  }
  if(millis() -   LastLEDLFlash >= 500)
    FlashLED();
}

void FlashLED(){
  if(digitalRead(LED_OVERRIDE))
    digitalWrite(LED_OVERRIDE, LOW);
  else
    digitalWrite(LED_OVERRIDE, HIGH);
  LastLEDLFlash = millis();
}


void ExitOverride(){
  OverrideMode = OVERRIDE_NONE;
  WorkingMode = LastWorkingMode;
  digitalWrite(LED_OVERRIDE, LOW);
}
