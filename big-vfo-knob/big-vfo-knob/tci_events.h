#ifndef TCI_EVENTS_h
#define TCI_EVENTS_h

#include "global.h"
#include "TCI.h"

int done = 0;

void on_connect_disconnect_event() {
  if (tci.connected()) {
    digitalWrite(TCI_CONNECTED_PIN, HIGH);
    Serial.println("== TCI: Connected");
  } else {
    digitalWrite(TCI_CONNECTED_PIN, LOW);
    Serial.println("== TCI: Disconnected");
  }
  displayVfo();
}

// ===== Please, for better parameters understanding
// ===== read the TCI official documentation on
// ===== https://github.com/ExpertSDR3/TCI
//
// ===== TCI commands implementation follows the same order
// ===== described in the above document

// ===== VFO =====
void on_vfo_event(int rtxId, int vfoId) {

  if (rtxId == 0 && vfoId == 0 && done == 0) {
    Serial.printf("== TCI: vfo event - rtxId:%d - vfoId:%d - Frequency: %d\n", rtxId, vfoId, tci.rtx[rtxId].getVfo(vfoId));
    vfo_encoder.setCount(tci.rtx[rtxId].getVfo(vfoId) * v_divider);
    done = 1;
  } /*else {
      Serial.printf("== ignored ==\n");
  }*/

  displayVfo();
}

// ===== AGC_MODE =====
void on_agc_mode_event(int rtxId) {
  switch (tci.rtx[rtxId].getAgcMode()) {
    case 0:
      Serial.printf("== TCI: agc_mode event - rtxId:%d - value:off\n", rtxId);
      break;
    case 1:
      Serial.printf("== TCI: agc_mode event - rtxId:%d - value:fast\n", rtxId);
      break;
    case 2:
      Serial.printf("== TCI: agc_mode event - rtxId:%d - value:normal\n", rtxId);
      break;
  }
}





void configure_tci_events() {
  tci.attach_conn_disc_event(on_connect_disconnect_event);
  tci.attach_vfo_event(on_vfo_event);
}

#endif
