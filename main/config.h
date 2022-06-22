#define DEBUGGING true

#ifdef DEBUGGING
  #define BAUDRATE 115200
#endif

const uint8_t VERSION_MAJ =  0;
const uint8_t VERSION_MIN =  1;

/*
 * All buttons should be pull down
 */

#define BUTTON_ADD      A0
#define BUTTON_SUB      A1
#define BUTTON_WINTER   A2
#define BUTTON_DISPLAY  A3

#define LED_TROPICAL  2
#define LED_OVERRIDE  3

#define BUTTON_DEBOUNCE 100

#define PUMP_PIN   8
#define ACTIVATE   HIGH
#define DEACTIVATE LOW

#define DISPLAY_STROBE  4
#define DISPLAY_DATA    5
#define DISPLAY_CLOCK   6

#define ROOF_SENSOR_PIN A4
#define POOL_SENSOR_PIN A5

#define MIN_COMFORT_TEMP 20
#define MAX_COMFORT_TEMP 40

#define MIN_CALIBRATION_TEMP -5
#define MAX_CALIBRATION_TEMP  5

#define MIN_DIFFERENTIAL_TEMP  3
#define MAX_DIFFERENTIAL_TEMP  20

#define INCREMENT_DECEMENT_FACTOR 1
const uint16_t INCREMENT_AFTER = 300; // time in milli sec

const int COMF_EEPROM_ADDR = 0;
const int DIFF_EEPROM_ADDR = 2;
const int CALI_EEPROM_ADDR = 4;

const int LAST_POOL_EEPROM_ADDR = 6;

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


const byte digit14ToSegment[10][2] = {
 // G2G1FEDCBA    XXMLKJIH
  0b00111111, 0b00000000,    // 0
  0b00000110, 0b00000000,    // 1
  0b11011011, 0b00000000,    // 2
  0b11001111, 0b00000000,    // 3
  0b11100110, 0b00000000,    // 4
  0b11101101, 0b00000000,    // 5
  0b11111101, 0b00000000,    // 6
  0b00000111, 0b00000000,    // 7
  0b11111111, 0b00000000,    // 8
  0b11101111, 0b00000000     // 9
};

typedef enum{
  WINTER,
  NORMAL_SUMMER,
  TROPICAL,
  OVERRIDE,  
}working_modes;


typedef enum{
  TROPICAL_NONE,
  TROPICAL_SUMMER_HEAT,
  TROPICAL_SUMMER_COOLING,
}tropical_mode;


typedef enum{
  OVERRIDE_NONE,
  OVERRIDE_ON1,
  OVERRIDE_ON4,
  OVERRIDE_OFF_MOM,
  OVERRIDE_OFF_PER,
}override_modes;



typedef enum{
  BLANK,
  DISPLAY_CONTENT,
  CAL,
  CALIBRATION,
  DIFF,
  DIFFERENTIAL,
  COMFORT,
  PACT, // p with acutal temp
  LTEMP, // Last stored temp
  LDER, // L with derated temp
  LLPT,
}display_modes;


typedef enum{
  ROOF_TEMP,
  POOL_TEMP,
  LAST_POOL_TEMP,
  COMFORT_TEMP,
}display_mode_content;


typedef enum{
  PUMP_ON,
  PUMP_OFF,
}pump;

typedef enum{
  UNIT_ON,
  UNIT_OFF,
}unit;

typedef enum{
  STABILIZATION_ON,
  STABILIZATION_OFF,
}stabilazation;


typedef enum{
  READ_TRUE,
  READ_FALSE,
}sensors_check;
