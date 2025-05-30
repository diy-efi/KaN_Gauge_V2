
//==============================================
/*  

  'gauge.h'


*/
//==============================================

#pragma once
#include "wifi.h"
#include "display.h"
#include "pinMap.h"
#include "DataFunctions.h"
#include "SensorData.h"
#include "config.h"
#include "led.h"
#include "nvs.h"
#include "cli.h"
#include "gauge.h"
#include "can.h"


bool ledAlert = false;
bool alertActive = false;
bool showAlert = false;
bool analogMeterSetup = false;
bool screenSet = false;
bool alertInProgress = false;

int lastAlert_ms = 0;
int alertTime_ms = 250;
int alertTime2_ms = 250;

int printX;
int printY;
int useTextSize;

#define ALERT_TEXT_COLOR 0
#define ALERT_EXC 1
#define ALERT_FF 2
#define ALERT_LED 3

String alert_names[] = { "No Icon", "Exclamation", "FF", "LED" };
bool checkFullscreenAlert[] = { false, false, true, false };

void checkAlerts() {

#ifdef DEBUG_SERIAL
  Serial.println("Checking Alert Status");
#endif

  showAlert = false;

  for (int alInc = 0; alInc < 8; alInc++) {
    ptrData[alInc]->alert = false;
    ptrData[alInc]->alert = sensorAlertCheck(ptrData[alInc]->alertLow, ptrData[alInc]->maximum, ptrData[alInc]->scaledValue);

#ifdef DEBUG_SERIAL
    Serial.print(alInc);
    Serial.print(" : ");
    Serial.println(ptrData[alInc]->alert);
#endif

    if (ptrData[alInc]->alert == true) {
      showAlert = true;
    }
  }
}

void displayFullscreenAlert_ff() {

  if (showAlert == true) {

    if (millis() > (lastAlert_ms + alertTime_ms)) {
      alertActive = !alertActive;
      lastAlert_ms = millis();
    }

    if (alertActive == true) {

      tft.fillScreen(TFT_RED);
      tft.setTextColor(TFT_BLACK);
      tft.setTextPadding(25 * 5);
      tft.setTextSize(4);
      tft.drawString("Warning!!!", 120, 120);
      tft.setTextSize(2);
      tft.drawString("Danger to", 120, 170);
      tft.drawString("Manifold", 120, 195);

    }

    else if (alertActive == false) {

      tft.fillScreen(TFT_ORANGE);
      tft.setTextColor(TFT_BLACK);
      tft.setTextPadding(25 * 5);
      tft.setTextSize(4);
      tft.drawString("Warning!!!", 120, 120);
      tft.setTextSize(2);
      tft.drawString("Danger to", 120, 170);
      tft.drawString("Manifold", 120, 195);
    }
  }

  else {
    alertActive = false;
    drawAlert(120, 210, 50, 0);
  }
}

void displayAlert_exc() {

  if (showAlert == true) {

    if (millis() > (lastAlert_ms + alertTime_ms)) {
      alertActive = !alertActive;
      lastAlert_ms = millis();
    }

    if (alertActive == true) {
      drawAlert(120, 210, 50, 1);

    } else if (alertActive == false) {
      drawAlert(120, 210, 50, 0);
    }
  }

  else {
    alertActive = false;
    drawAlert(120, 210, 50, 0);
  }
}

void displayAlert_noIcon() {
  if (showAlert == true) {

    if (millis() > (lastAlert_ms + alertTime_ms)) {
      alertActive = !alertActive;
      lastAlert_ms = millis();
    }
  }
}
/*
Updates the display based on the selected configurations
This includes two arcs across the top as well as 4 sets of digits
*/

void updateDisplay() {
  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;


  showAlert = false;

  int aCheck[] = { 0, 1, 2, 3, 4, 5 };
  for (int aCheckInc = 0; aCheckInc < 6; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);

    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      showAlert = true;
    }
  }

  if (showAlert == true && fullscreenAlert == true) {
  } else {

    // Inner
    if (ptrData[0]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeArc(ptrData[0]->scaledValue, ptrData[0]->minimum, ptrData[0]->maximum, (gaugeposition_x + x_2 + 18), (gaugeposition_y + y_2 + 15), radius2,
               4, 240, arcColor1, arcSeg1, arcInc1, ptrData[0]->digits, ptrData[0]->decimal);
    }

    // Outer
    if (ptrData[1]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeArc(ptrData[1]->scaledValue, ptrData[1]->minimum, ptrData[1]->maximum, (gaugeposition_x + x_1), (gaugeposition_y + y_1), radius1,
               8, 240, arcColor2, arcSeg2, arcInc2, ptrData[1]->digits, ptrData[1]->decimal);
    }

    // Center Top Text
    if (ptrData[2]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[2]->scaledValue, ptrData[2]->minimum, ptrData[2]->maximum, (120), (95), 1,
                ptrData[2]->units, ptrData[2]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[2]->alert, selectedColor[2], ptrData[2]->digits, ptrData[2]->decimal);
    }

    // Lower Left
    if (ptrData[3]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[3]->scaledValue, ptrData[3]->minimum, ptrData[3]->maximum, (60), (190), 1,
                ptrData[3]->units, ptrData[3]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[3]->alert, selectedColor[2], ptrData[3]->digits, ptrData[3]->decimal);
    }

    // Lower Text
    if (ptrData[4]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[4]->scaledValue, ptrData[4]->minimum, ptrData[4]->maximum, (120), (160), 1,
                ptrData[4]->units, ptrData[4]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[4]->alert, selectedColor[2], ptrData[4]->digits, ptrData[4]->decimal);
    }

    // Right
    if (ptrData[5]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[5]->scaledValue, ptrData[5]->minimum, ptrData[5]->maximum, (180), (190), 1,
                ptrData[5]->units, ptrData[5]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[5]->alert, selectedColor[2], ptrData[5]->digits, ptrData[5]->decimal);
    }
  }
}


void updateDisplay4x() {
  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;


  showAlert = false;

  int aCheck[] = { 0, 1, 2, 3, 4, 5 };
  for (int aCheckInc = 0; aCheckInc < 6; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);

    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      //showAlert = true;
    }
  }

  if (showAlert == true && fullscreenAlert == true) {
  } else {


    // Outer
    if (ptrData[1]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeArc(ptrData[1]->scaledValue, ptrData[1]->minimum, ptrData[1]->maximum, (gaugeposition_x + x_1), (gaugeposition_y + y_1), radius1,
               30, 140, arcColor2, arcSeg2, arcInc2, ptrData[1]->digits, ptrData[1]->decimal);
    }

    // Inner
    if (ptrData[0]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeArcInvert(ptrData[0]->scaledValue, ptrData[0]->minimum, ptrData[0]->maximum, (gaugeposition_x + x_1), (gaugeposition_y + y_1), radius1,
                     30, 140, arcColor1, arcSeg1, arcInc1, ptrData[0]->digits, ptrData[0]->decimal);
    }

    // Center Top Text
    if (ptrData[2]->dataName != "None") {

      if (ptrData[3]->dataName == "None" && ptrData[5]->dataName == "None" && ptrData[4]->dataName == "None") {
        printX = 120;
        printY = 140;
        useTextSize = 5;
      } else if (ptrData[3]->dataName == "None" && ptrData[5]->dataName == "None") {
        printX = 105;
        printY = 90;
        useTextSize = 6;
      } else {
        printX = 105;
        printY = 72;
        useTextSize = 6;
      }

      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[2]->scaledValue, ptrData[2]->minimum, ptrData[2]->maximum, (printX), (printY), useTextSize,
                ptrData[2]->units, ptrData[2]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[2]->alert, selectedColor[2], ptrData[2]->digits, ptrData[2]->decimal);
    }

    //  Left
    if (ptrData[3]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      if (ptrData[5]->dataName == "None") {
        printX = 105;
        printY = 140;
      } else {
        printX = 48;
        printY = 140;
      }

      gaugeText(ptrData[3]->scaledValue, ptrData[3]->minimum, ptrData[3]->maximum, (printX), (printY), 6,
                ptrData[3]->units, ptrData[3]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[3]->alert, selectedColor[2], ptrData[3]->digits, ptrData[3]->decimal);
    }

    // Lower Text
    if (ptrData[4]->dataName != "None") {

      if (ptrData[3]->dataName == "None" && ptrData[5]->dataName == "None" && ptrData[2]->dataName == "None") {
        printX = 120;
        printY = 140;
        useTextSize = 5;
      } else if (ptrData[3]->dataName == "None" && ptrData[5]->dataName == "None") {
        printX = 105;
        printY = 180;
        useTextSize = 6;
      } else {
        printX = 105;
        printY = 208;
        useTextSize = 6;
      }

      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[4]->scaledValue, ptrData[4]->minimum, ptrData[4]->maximum, (printX), (printY), useTextSize,
                ptrData[4]->units, ptrData[4]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[4]->alert, selectedColor[2], ptrData[4]->digits, ptrData[4]->decimal);
    }

    // Right
    if (ptrData[5]->dataName != "None") {
      if (ptrData[3]->dataName == "None") {
        printX = 105;
        printY = 140;
      } else {
        printX = 172;
        printY = 140;
      }
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[5]->scaledValue, ptrData[5]->minimum, ptrData[5]->maximum, (printX), (printY), 6,
                ptrData[5]->units, ptrData[5]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[5]->alert, selectedColor[2], ptrData[5]->digits, ptrData[5]->decimal);
    }
  }
}

/*
Updates the display based on the selected configurations
This is similar to the updateDisplay function with an increased size for the center digits
Includes one Arc and three sets of digits
*/

void updateDisplay_LargeText() {

  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;

  showAlert = false;
  int aCheck[] = { 1, 2, 3, 5 };
  for (int aCheckInc = 0; aCheckInc < 4; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);
    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      showAlert = true;
    }
  }


  if (showAlert == true && fullscreenAlert == true) {
  } else {


    // Outer
    if (ptrData[1]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeArc(ptrData[1]->scaledValue, ptrData[1]->minimum, ptrData[1]->maximum, (gaugeposition_x + x_1), (gaugeposition_y + y_1), radius1,
               8, 240, arcColor2, arcSeg2, arcInc2, ptrData[1]->digits, ptrData[1]->decimal);
    }

    // Center Top Text
    if (ptrData[2]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[2]->scaledValue, ptrData[2]->minimum, ptrData[2]->maximum, (120), (120), 4,
                ptrData[2]->units, ptrData[2]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[2]->alert, selectedColor[2], ptrData[2]->digits, ptrData[2]->decimal);
    }

    // Lower Left
    if (ptrData[3]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[3]->scaledValue, ptrData[3]->minimum, ptrData[3]->maximum, (60), (190), 1,
                ptrData[3]->units, ptrData[3]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[3]->alert, selectedColor[2], ptrData[3]->digits, ptrData[3]->decimal);
    }

    // Right
    if (ptrData[5]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[5]->scaledValue, ptrData[5]->minimum, ptrData[5]->maximum, (180), (190), 1,
                ptrData[5]->units, ptrData[5]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[5]->alert, selectedColor[2], ptrData[5]->digits, ptrData[5]->decimal);
    }
  }
}


/*
Updates the display based on the selected configurations
Includes one large set of digits
*/

void updateDisplay_Large() {

  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;

  showAlert = false;

  int aCheck[] = { 2 };

  for (int aCheckInc = 0; aCheckInc < 1; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);

    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      showAlert = true;
    }
  }

  if (showAlert == true && fullscreenAlert == true) {
  } else {

    // Center Top Text
    if (ptrData[2]->dataName != "None") {

      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[2]->scaledValue, ptrData[2]->minimum, ptrData[2]->maximum, (120), (140), 5,
                ptrData[2]->units, ptrData[2]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[2]->alert, selectedColor[2], ptrData[2]->digits, ptrData[2]->decimal);
    }
  }
}

/*
Updates the display based on the selected configurations
This creates an analog style needle on top with up to 5 sets of digits
*/

void updateDisplay_Analog() {
  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;

  if (analogMeterSetup == false) {
    //tft.fillScreen(TFT_BLACK);
    setupAnalogMeter(0, 10, ptrData[9]->dataName, ptrData[9]->units, ptrData[9]->minimum, ptrData[9]->maximum);
    tft.setTextColor(selectedColor[1], TFT_BLACK);
    tft.drawString(ptrData[9]->dataName, 20, 95, 2);
    tft.drawString(ptrData[9]->units, 205, 95, 2);
    analogMeterSetup = true;
  }

  // Meter Needle

  AnMeter.updateNeedle(ptrData[9]->scaledValue, 0, selectedColor[3], TFT_BLACK, selectedColor[0]);
  //Check if value is outside of alert thresholds, then print the values

  showAlert = false;
  int aCheck[] = { 3, 4, 5, 7, 8 };
  for (int aCheckInc = 0; aCheckInc < 5; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);
    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      showAlert = true;
    }
  }

  if (showAlert == true && fullscreenAlert == true) {
  } else {

    // Left Top
    if (ptrData[7]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[7]->scaledValue, ptrData[7]->minimum, ptrData[7]->maximum, (40), (140), 1,
                ptrData[7]->units, ptrData[7]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[7]->alert, selectedColor[2], ptrData[7]->digits, ptrData[7]->decimal);
    }

    // Right Top
    if (ptrData[8]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[8]->scaledValue, ptrData[8]->minimum, ptrData[8]->maximum, (200), (140), 1,
                ptrData[8]->units, ptrData[8]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[8]->alert, selectedColor[2], ptrData[8]->digits, ptrData[8]->decimal);
    }

    // Center
    if (ptrData[4]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[4]->scaledValue, ptrData[4]->minimum, ptrData[4]->maximum, (120), (160), 1,
                ptrData[4]->units, ptrData[4]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[4]->alert, selectedColor[2], ptrData[4]->digits, ptrData[4]->decimal);
    }

    // Bottom Left
    if (ptrData[3]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[3]->scaledValue, ptrData[3]->minimum, ptrData[3]->maximum, (60), (190), 1,
                ptrData[3]->units, ptrData[3]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[3]->alert, selectedColor[2], ptrData[3]->digits, ptrData[3]->decimal);
    }

    // Bottom Right
    if (ptrData[5]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[5]->scaledValue, ptrData[5]->minimum, ptrData[5]->maximum, (180), (190), 1,
                ptrData[5]->units, ptrData[5]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[5]->alert, selectedColor[2], ptrData[5]->digits, ptrData[5]->decimal);
    }
  }
}

/*
Updates the display based on the selected configurations
This creates an analog style needle on top with larger center digits
*/


void updateDisplay_Analog_LargeText() {
  int radius1 = 116;
  int radius2 = 80;
  int x_1 = 15;
  int y_1 = 17;

  int x_2 = x_1 + (radius1 - radius2) / 2;
  int y_2 = y_1 + (radius1 - radius2) / 2;

  if (analogMeterSetup == false) {
    //tft.fillScreen(TFT_BLACK);
    setupAnalogMeter(0, 10, ptrData[9]->dataName, ptrData[9]->units, ptrData[9]->minimum, ptrData[9]->maximum);
    tft.setTextColor(selectedColor[1], TFT_BLACK);
    tft.drawString(ptrData[9]->dataName, 20, 95, 2);
    tft.drawString(ptrData[9]->units, 205, 95, 2);

    analogMeterSetup = true;
  }

  // Meter Needle
  AnMeter.updateNeedle(ptrData[9]->scaledValue, 0, selectedColor[3], TFT_BLACK, selectedColor[0]);
  //Check if value is outside of alert thresholds, then print the values
  showAlert = false;
  int aCheck[] = { 3, 4, 5 };
  for (int aCheckInc = 0; aCheckInc < 3; aCheckInc++) {
    ptrData[aCheck[aCheckInc]]->alert = sensorAlertCheck(ptrData[aCheck[aCheckInc]]->alertLow, ptrData[aCheck[aCheckInc]]->alertHigh, ptrData[aCheck[aCheckInc]]->scaledValue);
    if (ptrData[aCheck[aCheckInc]]->alert == true) {
      showAlert = true;
    }
  }

  if (showAlert == true && fullscreenAlert == true) {
  } else {

    // Center
    if (ptrData[4]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[4]->scaledValue, ptrData[4]->minimum, ptrData[4]->maximum, (130), (150), 3,
                ptrData[4]->units, ptrData[4]->dataName, selectedColor[0], selectedColor[1], false, false, 15, ptrData[4]->alert, selectedColor[2], ptrData[4]->digits, ptrData[4]->decimal);
    }

    // Bottom Left
    if (ptrData[3]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[3]->scaledValue, ptrData[3]->minimum, ptrData[3]->maximum, (60), (190), 1,
                ptrData[3]->units, ptrData[3]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[3]->alert, selectedColor[2], ptrData[3]->digits, ptrData[3]->decimal);
    }

    // Bottom Right
    if (ptrData[5]->dataName != "None") {
      //Check if value is outside of alert thresholds, then print the values
      gaugeText(ptrData[5]->scaledValue, ptrData[5]->minimum, ptrData[5]->maximum, (180), (190), 1,
                ptrData[5]->units, ptrData[5]->dataName, selectedColor[0], selectedColor[1], TRUE, TRUE, 15, ptrData[5]->alert, selectedColor[2], ptrData[5]->digits, ptrData[5]->decimal);
    }
  }
}


/*
Task to update the UI
Inludes display and LEDs
Update rate defined by displayRefresh_ms
*/

void updateDisplayTask(void *pvParameters) {

#ifdef DEBUG_SERIAL
  Serial.print("Display Task on core: ");
  Serial.println(xPortGetCoreID());
#endif

  tft.fillScreen(TFT_BLACK);
  int lastDisplayUpdate_ms = 0;

  for (;;) {


    if ((millis() - lastDisplayUpdate_ms) > displayRefresh_ms) {
      getBrightness();
      setDisplayBrightness(dispBrightness);
      fullscreenAlert = checkFullscreenAlert[config_alertType];

      lastDisplayUpdate_ms = millis();

      if (showAlert == false && alertInProgress == true) {
        if (config_alertType == 2) {
          resetDisplay = true;
        }
        alertInProgress = false;
      }

      if (showAlert == true) {
        alertInProgress = true;
      }

      // When updating config reset the display to remove old text
      if (resetDisplay == true || updatedArc == true) {
        tft.fillScreen(TFT_BLACK);
        resetDisplay = false;
        updatedArc = false;
        analogMeterSetup = false;
#ifdef DEBUG_SERIAL
        Serial.println("Display reset");
#endif
      }

      showAlert = false;

      switch (gaugeDisplayType) {

        case 0:
          updateDisplay();
          break;

        case 1:
          updateDisplay_Analog();
          break;

        case 2:
          updateDisplay_LargeText();
          break;

        case 3:
          updateDisplay_Analog_LargeText();
          break;

        case 4:
          updateDisplay_Large();
          break;

        case 5:
          updateDisplay4x();
          break;

        default:
          updateDisplay();
          break;
      }


      if (showAlert == true) {
        switch (config_alertType) {

          case 0:
            displayAlert_noIcon();
            break;

          case 1:
            displayAlert_exc();
            break;

          case 2:
            displayFullscreenAlert_ff();
            break;

          case 3:
            displayAlert_noIcon();
            break;

          default:
            displayAlert_noIcon();
            break;
        }
      } else {

        if (config_alertType != 0 && config_alertType != 3) {
          // Clean up Exclaimation if printed and no Alert exsists
          drawAlert(120, 210, 50, 0);
        }
      }


      if (ledEnabled == true) {

        bool skipLED = false;

        if (showAlert == true) {
          if (alertActive == true) {

            if (config_alertType == ALERT_FF) {
              ledSeries(6, 255, 0, 0, ledBrightness);
            }

            if (config_alertType == ALERT_LED) {
              ledSeries(6, 255, 0, 0, ledBrightness);
            }
            skipLED = true;
          }

          else {
            if (config_alertType == ALERT_FF) {
              ledSeries(6, 255, 185, 0, ledBrightness);
            }
            if (config_alertType == ALERT_LED) {
              ledSeries(6, 0, 0, 0, ledBrightness);
            }
            skipLED = true;
          }
        }

        if (skipLED == true) {
        } else {

          int ledsON = 7 * (ptrData[6]->scaledValue - ptrData[6]->minimum) / (ptrData[6]->maximum - ptrData[6]->minimum);
          int ledCLR = ledsON - 1;

          if (ledCLR < 0) {
            ledsON = 0;
            ledCLR = 0;
          }

          if (ledCLR > 5) {
            ledCLR = 5;
            ledsON = 6;
          }

          switch (LEDstyle) {

            case 0:
              ledSeries(ledsON, LEDred[LEDcolor][ledCLR], LEDgreen[LEDcolor][ledCLR], LEDblue[LEDcolor][ledCLR], ledBrightness);
              break;

            case 1:
              ledSeriesMulticolor(ledsON, LEDred[LEDcolor][ledCLR], LEDgreen[LEDcolor][ledCLR], LEDblue[LEDcolor][ledCLR], ledBrightness);
              break;

            case 2:
              ledSingle(ledsON, LEDred[LEDcolor][ledCLR], LEDgreen[LEDcolor][ledCLR], LEDblue[LEDcolor][ledCLR], ledBrightness);
              break;

            default:
              ledSeries(ledsON, LEDred[LEDcolor][ledCLR], LEDgreen[LEDcolor][ledCLR], LEDblue[LEDcolor][ledCLR], ledBrightness);
              break;
          }
        }
      }
    }
  }
}
