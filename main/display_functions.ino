void UpdateDisplay(){
  uint8_t numner_unit;
  uint8_t numner_tens;

  
  if(WorkingMode == WINTER){
    H = 40;
    L = 10;
    U = T = 0;
  }
  else if(WorkingMode == OVERRIDE){
    H = 0;
    L = 206;
    if(LastScreenFlash){
      if(OverrideMode == OVERRIDE_ON1){
        T = 84;
        U = 6;
      }
      else if(OverrideMode == OVERRIDE_ON4){
        T = 84;
        U = 102;
      }
      else if(OverrideMode == OVERRIDE_OFF_MOM || OverrideMode == OVERRIDE_OFF_PER){
        T = 113;
        U = 113;
      }
      LastScreenFlash = false;
    }
    else{
      if(OverrideMode != OVERRIDE_OFF_PER){
        H = L = 0;
        numner_unit = CurrentPoolTemp % 10;
        numner_tens = CurrentPoolTemp / 10;
        T = digitToSegment[numner_tens];
        U = digitToSegment[numner_unit];
      }
      LastScreenFlash = true;
    }
  }
  else if(Display == LTEMP){
    H = 0;
    L = 28; // L
    numner_unit = LastSavedTemp % 10;
    numner_tens = LastSavedTemp / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }
  else if(Display == PACT){ //  P with actual temp
    H = 0;
    L = 249;
    numner_unit = CurrentPoolTemp % 10;
    numner_tens = CurrentPoolTemp / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }
  else if(Display == LDER){ // L with derated temp
    H = 0;
    L = 249;
    numner_unit = CurrentPoolTemp % 10;
    numner_tens = CurrentPoolTemp / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }

  else if(Display == LLPT){ // L with last pool temp
    H = 0;
    L = 249;
    numner_unit = LastSavedTemp % 10;
    numner_tens = LastSavedTemp / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }

  
  
  else if(Display == CAL){
    H = 0;
    L = 60; // C
    T = 119; // A
    U = 56; // L
  }
  else if(Display == CALIBRATION){
    int n = Calibrated_Temp;
    if(Calibrated_Temp >= 0){ // +
      H = 18;
      L = 192;
    }
    else{ // -
      H = 0;
      L = 192;
      n = n *-1;
    }
    numner_unit = n % 10;
    numner_tens = n / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }
  else if(Display == DIFF){
    H = 0;
    L = 207; // d
    T = 48; // I
    U = 113; // F
  }
  else if(Display == DIFFERENTIAL){
    H = 0;
    L = 0;
    numner_unit = Differential_Temp % 10;
    numner_tens = Differential_Temp / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }
  else if(Display == COMFORT){
    H = 0;
    L = 60;
    numner_unit = Current_Comport % 10;
    numner_tens = Current_Comport / 10;
    T = digitToSegment[numner_tens];
    U = digitToSegment[numner_unit];
  }
  else if(Display == DISPLAY_CONTENT){
    if(DisplayContent == ROOF_TEMP){
      H = 0;
      L = 200; // r
      numner_unit = CurrentRoofTemp % 10;
      numner_tens = CurrentRoofTemp / 10;
      T = digitToSegment[numner_tens];
      U = digitToSegment[numner_unit];
      DisplayContent = POOL_TEMP;
    }
    else if(DisplayContent == POOL_TEMP){
      H = 0;
      L = 249; // P
      numner_unit = CurrentPoolTemp % 10;
      numner_tens = CurrentPoolTemp / 10;
      T = digitToSegment[numner_tens];
      U = digitToSegment[numner_unit];
      DisplayContent = LAST_POOL_TEMP;
    }
    else if(DisplayContent == LAST_POOL_TEMP){
      H = 0;
      L = 28; // L
      numner_unit = LastSavedTemp % 10;
      numner_tens = LastSavedTemp / 10;
      T = digitToSegment[numner_tens];
      U = digitToSegment[numner_unit];
      DisplayContent = COMFORT_TEMP;
    }
    else if(DisplayContent == COMFORT_TEMP){
      H = 0;
      L = 60; // C
      numner_unit = Current_Comport % 10;
      numner_tens = Current_Comport / 10;
      T = digitToSegment[numner_tens];
      U = digitToSegment[numner_unit];
      DisplayContent = COMFORT_TEMP;
      DisplayContent = POOL_TEMP;
      Display = LastDisplay;
    }
  }
  Refresh();
  LastDisplayUpdate = millis();
}


void Refresh(){
  if(doDecimal){
    H = H | 64;
    if(flashDecimal){
      if(LastDecimal)
        LastDecimal = false;
      else{
        H = H & 191;
        LastDecimal = true;
      }
    }
  }
  digitalWrite(DISPLAY_STROBE, LOW);
  shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, H);
  shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, L);
  shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, U);
  shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, T);
  digitalWrite(DISPLAY_STROBE, HIGH);
}
