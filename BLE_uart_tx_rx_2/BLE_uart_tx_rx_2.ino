/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE" 
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second. 
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint32_t value1 = 0;
char char_array[10];


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

//#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // UART service UUID
//#define CHARACTERISTIC_UUID_RX "2049779d-88a9-403a-9c59-c7df79e1dd7c"
//#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define SERVICE_UUID           "dd5f7232-1560-4792-953d-0b2015f15340" // UART service UUID
#define CHARACTERISTIC_UUID_RX "1e630bfc-08ca-44c0-a7c5-58dae380884d"
#define CHARACTERISTIC_UUID_TX "8796fa1b-986d-419a-8f84-137710a2354f"
//UUIDの取得方法→Powershellで　[Guid]::NewGuid()　と打つとUUIDが発行される。3回やって、1個目をService UUIDにして2個目と3個目をCharacteristic UUIDにする。



class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      String getstr="";
      
      if (rxValue.length() > 0) {
        Serial.println("*********");
        //Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){
          //Serial.print(rxValue[i]);
          getstr += rxValue[i];
        }
        Serial.print(getstr);
        Serial.println();
        Serial.println("*********");
      }
    }
};




void task1(void* arg) {
  int Xval=0;
  int Yval=0;
  int Xval2=0;
  int Yval2=0;
  
    
  while (1) {
    int X1=analogRead(33);
    int X2=analogRead(32);
    int Y1=analogRead(35);
    int Y2=analogRead(34);

    X1 -= 1775;
    X2 -= 1775;
    Y1 -= 1775;
    Y2 -= 1775;

    float AngX=0;
    float AngY=0;
     
    AngX=atan2(X1,X2);
    AngX *= 180;
    AngX /= 3.14159265359;
    AngX += 180;
    AngX /= 2;
  
    AngY=atan2(Y2,Y1);
    AngY *= 180;
    AngY /= 3.14159265359;
    AngY += 180;
    AngY /= 2;
  
    Xval=AngX;
    Yval=AngY;
    
    int Xoffset=0;
    int Yoffset=0;

    Xval -= Xoffset;
    Yval -= Yoffset;

    byte xmax=180;
    byte xmin=0;
    byte ymax=180;
    byte ymin=0;
    int Xrange=4000;
    int Yrange=4000;

    
    Xval2 = map(Xval,xmin,xmax,0,Xrange);
    Yval2 = map(Yval,ymax,ymin,0,Yrange);
  
    value=Xval2;
    value1=Yval2;

    Serial.print(Xval2);
    Serial.print(',');
    Serial.println(Yval2);
    
    
    delay(10); 
  }
}






void setup() {
  Serial.begin(115200);

  pinMode(15,OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
//  pServer->getAdvertising()->start();
//  Serial.println("Waiting a client connection to notify...");
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  digitalWrite(15,HIGH);

  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, NULL, 1);
  
}







void loop() {

    if (deviceConnected) {
      String MOJI;
      MOJI=(String)value;
      MOJI+=",";
      MOJI+=(String)value1;
      MOJI.toCharArray(char_array,10);
      //pTxCharacteristic->setValue(&txValue, 1);
      pTxCharacteristic->setValue(char_array);
      pTxCharacteristic->notify();
      //value++;
		  delay(50); // bluetooth stack will go into congestion, if too many packets are sent
	  }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

    
}
