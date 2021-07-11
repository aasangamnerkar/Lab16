SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

#include "oled-wing-adafruit.h"

const size_t UART_TX_BUF_SIZE = 20;
bool firstConnected = false;

OledWingAdafruit display;

void printToDisplay(String output)
{
  //reset display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

      //output to display
  display.println(output);
  display.display();
}

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    for (size_t i = 0; i < len; i++) {
        if ((char) data[i] == '1')
        {
          printToDisplay("SLATT");
        }
        else if ((char) data[i] == '0')
        {
          BLE.disconnect();
          firstConnected = false;
        }
    }
}

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

//set up BLE
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);


// setup() runs once, when the device is first turned on.
void setup() {
  BLE.on();

  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);

  RGB.control(true);

  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("");
  display.display();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  if (BLE.connected() == 0)
  {
    RGB.control(true);
    RGB.color(255, 255, 0);
    RGB.brightness(255);
  }
  else if (BLE.connected() == 1 && !firstConnected)
  {
    if (!firstConnected)
    {
      RGB.control(true);
      RGB.color(0, 0, 255);
      RGB.brightness(255);

      delay(5000);


      uint8_t txBuf[UART_TX_BUF_SIZE];
      String message = "SLATT!";
      message.toCharArray((char *)txBuf, message.length() + 1);
      txCharacteristic.setValue(txBuf, message.length() + 1);

      firstConnected = true;
    }
    else
    {
      
    }
  }
  
}