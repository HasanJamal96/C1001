const byte digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
};

void updateDisplay(int number, String character, bool Char = false ){
  digitalWrite(DISPLAY_STROBE, LOW);
  uint8_t len = character.length();
  if(Char){
    switch(character[0]){
      case 'r':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11001000);
        break;
      case 'L':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00011100);
        break;
      case 'C':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00111100);
        break;
      case 'P':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11111001);
        break;
      case 'W':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00101000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00011011);
        break;
      case 'o':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11001110);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01010100);
        if(number == 1)
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000110);
        else
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01100110);
        break;
      case 'd':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11001111);
        break;
      case '+':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00010010);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11000000);
        break;
      case '-':
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00000000);
        shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b11000000);
        break;
    }
    if(len > 1){
      switch(character[1]){
        case 'r':
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01010000);
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01010000);
          break;
        case 'P':
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110011);
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110011);
          break;
        case 'i':
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b00110000);
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110001);
          break;
        case 'A':
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110111);
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110111);
          break;
        case 'F':
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110001);
          shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0b01110001);
          break;
      }
    }
  }
  if(number < 0 && len < 2){
    for(uint8_t x=0; x<2; x++)
      shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0);
  }
  else{
    uint8_t unit = number%10;
    uint8_t tens = number/10;
    shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, digitToSegment[tens]);
    shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, digitToSegment[unit]);
  }
  digitalWrite(DISPLAY_STROBE, HIGH);
}


void clearDisplay(){
  digitalWrite(DISPLAY_STROBE, LOW);
  for(uint8_t x=0; x<4; x++)
    shiftOut(DISPLAY_DATA, DISPLAY_CLOCK, MSBFIRST, 0);
  digitalWrite(DISPLAY_STROBE, HIGH);
}
