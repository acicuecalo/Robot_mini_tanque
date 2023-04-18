#include <ESP8266WiFi.h>
#include <espnow.h>

int myHeader = 255;
int myIDPacket = 1;
int myChecksum = myIDPacket + myHeader;

int headerRX;
int idPacketRX;
int checksumRX;

bool packetRX_OK;
bool failsafe;
unsigned long timeStampRX;
unsigned long oldTimeStampRX;

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  // Esta es la dirección MAC de broadcast (todos los receptores reciben el mensaje)

// Define variables to store readings to be sent
const uint8_t numChannelsTX = 16;
int16_t TX_Channel[numChannelsTX];

// Define variables to store incoming readings
const uint8_t numChannelsRX = 16;  // 16 canales en total (del 0 al 15)
int16_t RX_Channel[numChannelsRX];

//Structure to send data. Must match the receiver structure
typedef struct struct_send_message {
  int header;
  int idPacket;
  int checksum;
  unsigned long timeStamp;
  int16_t channel[numChannelsTX];
} struct_send_message;

// Create a struct_message called outgoingESPNOW
struct_send_message outgoingToESPNOW;

//Structure to receive data. Must match the receiver structure
typedef struct struct_receive_message {
  int header;
  int idPacket;
  int checksum;
  unsigned long timeStamp;
  int16_t channel[numChannelsRX];
} struct_receive_message;

// Create a struct_message called incomingESPNOW
struct_receive_message incomingReadingsFromESPNOW;

// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingReadingsFromESPNOW, incomingData, sizeof(incomingReadingsFromESPNOW));
  idPacketRX = incomingReadingsFromESPNOW.idPacket;
  headerRX = incomingReadingsFromESPNOW.header;
  checksumRX = incomingReadingsFromESPNOW.checksum;
  if ((headerRX == myHeader) && (idPacketRX == myIDPacket) && (checksumRX == myChecksum)) {
    packetRX_OK = true;
    timeStampRX = incomingReadingsFromESPNOW.timeStamp;
    for (int i = 0; i < numChannelsRX; i++) {
      RX_Channel[i] = incomingReadingsFromESPNOW.channel[i];
    }
  } else {
    packetRX_OK = false;
  }
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
 }

void getReadings() { // Variables to be sent
  outgoingToESPNOW.header = myHeader;
  outgoingToESPNOW.idPacket = myIDPacket;
  outgoingToESPNOW.checksum = myChecksum;
  outgoingToESPNOW.timeStamp = millis();
  outgoingToESPNOW.channel[0] = ejeX;
  outgoingToESPNOW.channel[1] = ejeY;
}

void iniciarESPNOW() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void actualizarESPNOW() {
  getReadings();
  sendMessageViaESPNOW();
}

void timerESPNOW(){
  const int intervaloDeActualizacion = 20; // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion; 
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    actualizarESPNOW();  // sending tasks
    timerFailSafe();
    //printPacketRX();
  }
}

void sendMessageViaESPNOW() {
  esp_now_send(broadcastAddress, (uint8_t *)&outgoingToESPNOW, sizeof(outgoingToESPNOW));
}

void checkFailsafe() {
  if (timeStampRX <= oldTimeStampRX) {
    failsafe = true;
    loadFailSafeValues();
  } else {
    failsafe = false;
  }
  oldTimeStampRX = timeStampRX;
}

void timerFailSafe() {
  const int intervaloDeActualizacion = 500;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    checkFailsafe();
  }
}

void printPacketRX() {
  if (failsafe) {
    Serial.println("failsafe");
  } else {
    if (packetRX_OK) {
      Serial.print("packet RX: OK");
      Serial.print("   ");
      Serial.print("idPacketRX: ");
      Serial.print(idPacketRX);
      Serial.print("   ");
      Serial.print("timeStampRX: ");
      Serial.print(timeStampRX);
      Serial.print("\t");
      Serial.print("RX_Channel: ");
      Serial.print("\t");
      for (int i = 0; i < numChannelsRX; i++) {
        Serial.print(i);
        Serial.print("_");
        Serial.print(RX_Channel[i]);
        Serial.print("\t");
      }
      Serial.println("");
    } else {
      Serial.println("last packet RX: BAD");
    }
  }
}

void loadFailSafeValues() {
  timeStampRX = 0;
  for (int i = 0; i < numChannelsRX; i++) {
    RX_Channel[i] = 0;
  }
}
