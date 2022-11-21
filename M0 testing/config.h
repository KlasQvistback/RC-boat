/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "Klas_Qvistback"
#define IO_KEY       "aio_NTPz58QEdWU9tiybpCEOK5ojlqgb"

/******************************* WIFI **************************************/

#define WIFI_SSID "D410"
#define WIFI_PASS "Axe2018"

// comment out the following lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
