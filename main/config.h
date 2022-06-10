#define DEBUGGING true

#ifdef DEBUGGING
  #define BAUDRATE 115200
#endif


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
#define INCREMENT_FACTOR 1
const int COMF_EEPROM_ADDR = 0;
