#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <AceButton.h>
#include <ESP32Encoder.h>



void displayVfo();

//includes required by the TCI library
#include "global.h"
#include "network.h"
#include "tci_events.h"

//Set TCI parameters in the "global.h" file
//Set pins definition in the "global.h" file
int last_agc_mode;
int last_frequency = 0;
const long send_interval = 75;  //ms - means max 20 times per second
unsigned long lastSendMillis = 0;
long frequency = 0;

//Buttons configuration
using namespace ace_button;
AceButton btn_mute(BTN_MUTE);
void handleMuteEvent(AceButton*, uint8_t, uint8_t);
AceButton btn_apf(BTN_APF);
void handleApfEvent(AceButton*, uint8_t, uint8_t);

//MUTE is a rig related event
void handleMuteEvent(AceButton* /* button */, 
                     uint8_t eventType,
                     uint8_t /* buttonState */) {
  switch (eventType) {
    case AceButton::kEventPressed:      
      if (tci.is_mute()) {
        tci.set_mute(false);
      } else {
        tci.set_mute(true);
      }
      break;
  }
}


//APF is an RX related event
void handleApfEvent(AceButton* /* button */, 
                     uint8_t eventType,
                     uint8_t /* buttonState */) {  
  switch (eventType) {
    case AceButton::kEventPressed:  
      if (tci.rtx[0].isRxApfEnable()) {
        tci.set_rx_apf_enable(0,false);
      } else {
        tci.set_rx_apf_enable(0,true);
      }
      break;
  }
}


void buttonHandler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (button->getPin()) {
    case BTN_MUTE:
      handleMuteEvent(button, eventType, buttonState);
      break;
    case BTN_APF:
      handleApfEvent(button, eventType, buttonState);
      break;
  }
}



void displayVfo() {
  display.clearDisplay();
  display.drawFastHLine(0, 43, 127, WHITE);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  //RX1-VFOA
  int val = tci.rtx[0].getVfo(0);
  if (tci.connected() && val > 0 && val < 100000000) {
    int mhz = val / 1000000;
    int khz = (val - mhz * 1000000) / 1000;
    int hz = val - mhz * 1000000 - khz * 1000;
    char buf[11];
    sprintf(buf, "%02d.%03d.%03d", mhz, khz, hz);
    display.println(buf);
  } else {
    display.println("   .   .   ");
  }
  display.setCursor(0, 21);
  //RX1-VFOB
  val = tci.rtx[0].getVfo(1);
  if (tci.connected() && val > 0 && val < 100000000) {
    int mhz = val / 1000000;
    int khz = (val - mhz * 1000000) / 1000;
    int hz = val - mhz * 1000000 - khz * 1000;
    char buf[11];
    sprintf(buf, "%02d.%03d.%03d", mhz, khz, hz);
    display.println(buf);
  } else {
    display.println("   .   .   ");
  }
display.setCursor(0, 45);
char buf[11];
sprintf(buf, "test TEST");
display.println(buf);

  display.display();
}




void setup() {
  Serial.begin(115200);
  delay(2000);

  //Led config
  pinMode(TCI_CONNECTED_PIN, OUTPUT);
  digitalWrite(TCI_CONNECTED_PIN, LOW);


  //Button config
  pinMode(BTN_MUTE,INPUT_PULLUP);
  pinMode(BTN_APF,INPUT_PULLUP);
  ButtonConfig* config = ButtonConfig::getSystemButtonConfig();
  config->setEventHandler(buttonHandler);



  //Rotary encoder setup
  // Enable the weak pull up resistors
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  //vfo_encoder.attachHalfQuad(VFO_ENCODER_A_PIN, VFO_ENCODER_B_PIN);
  vfo_encoder.attachFullQuad(VFO_ENCODER_A_PIN, VFO_ENCODER_B_PIN);
  // This value will be overwritten when the TCI will connect
  vfo_encoder.setCount(14100000 * v_divider);



  //Display config
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  // Clear the buffer.
  display.clearDisplay();
  displayVfo();

  //TCI mandatory methods
  configure_tci_events();
  tci.set_host(tci_host);
  tci.set_port(tci_port);
  tci.set_iaru_region(iaru_region);
  init_network();

  Serial.println("Setup done");
}


void loop() {
  // Do not use the "delay" command
  // Use instead the "millis()" command as explained in the following article
  // https://www.digikey.com/en/maker/blogs/2022/how-to-avoid-using-the-delay-function-in-arduino-sketches

  //You don't need do display the vfo periodically
  //Just recall the displayVfo() function from the
  //event handler in the "tci_events.h" file

  btn_mute.check();
  btn_apf.check();



  unsigned long currentMillis = millis();
  if (currentMillis - lastSendMillis >= send_interval) {
    frequency = vfo_encoder.getCount() / v_divider;
    int r = frequency % v_step;
    if (r >= 5) {
      frequency = frequency + v_step - r;
    } else {
      frequency = frequency - r;
    }
    //Serial.println(frequency);
    if (frequency != last_frequency) {
      lastSendMillis = currentMillis;
      tci.set_vfo(0, 0, frequency);
      last_frequency = frequency;
    }
  }


  if (currentMillis - lastSendMillis > (3 * send_interval)) {
    long f = tci.rtx[0].getVfo(0);
    if (f != frequency) {
      vfo_encoder.setCount(f * v_divider);
      Serial.printf("== %d == TCI: vfo event - rtxId:%d - vfoId:%d - Frequency: %d\n",
                    millis(), 0, 0, tci.rtx[0].getVfo(0));
    }
  }
}
