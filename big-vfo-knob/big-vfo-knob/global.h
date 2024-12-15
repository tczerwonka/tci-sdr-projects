#ifndef GLOBAL_h
#define GLOBAL_h

#include "TCI.h"

#define TCI_CONNECTED_PIN 2

#include "private.h"
/*
static char *ap_ssid = "TCI_ESP32";
static char *ap_pass = "12345678";
static char *wlan_ssid = "ssid";
static char *wlan_pass = "password";
static char *tci_host = "192.168.1.100"; 
static unsigned int tci_port = 50001;
static unsigned int iaru_region = 2;
*/

TCI tci;

ESP32Encoder vfo_encoder;
int v_step = 10;
int v_divider = 1;


//Pin definitions
#define SDA 21
#define SCL 22

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool refresh_display;       



//for rotary encoder
#define AGC_NORM_LED 12
#define AGC_NORM_FAST 13

#define VFO_ENCODER_A_PIN 19
#define VFO_ENCODER_B_PIN 18



#endif
