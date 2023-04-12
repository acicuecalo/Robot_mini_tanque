#include <esp_now.h>
#include <WiFi.h>

int myHeader = 255;
int myIDPacket = 1;
int myChecksum = myIDPacket + myHeader;

int headerRX;
int idPacketRX;
int checksumRX;

const int idTelemetriaReceptor = 2;
const int idMandoAuxiliar = 4;

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

// Create a struct_message called incomingReadingsFromESPNOW
struct_receive_message incomingReadingsFromESPNOW;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingReadingsFromESPNOW, incomingData, sizeof(incomingReadingsFromESPNOW));
  idPacketRX = incomingReadingsFromESPNOW.idPacket;
  headerRX = incomingReadingsFromESPNOW.header;
  checksumRX = incomingReadingsFromESPNOW.checksum;

  if ((headerRX == myHeader) && (idPacketRX == idTelemetriaReceptor) && (checksumRX == myHeader + idTelemetriaReceptor)) {
    packetRX_OK = true;
    timeStampRX = incomingReadingsFromESPNOW.timeStamp;
    for (int i = 0; i < 7; i++) {
      RX_Channel[i] = incomingReadingsFromESPNOW.channel[i];
    }
  } else {
    packetRX_OK = false;
  }

  // Mando auxiliar
  if ((headerRX == myHeader) && (idPacketRX == idMandoAuxiliar) && (checksumRX == myHeader + idMandoAuxiliar)) {
    const int offset = 8;
    packetRX_OK = true;
    for (int i = 0; i < 8; i++) {
      RX_Channel[i + offset] = incomingReadingsFromESPNOW.channel[i];
      RX_Channel[7] = incomingReadingsFromESPNOW.channel[8];
    }
  }
}

void loadFailSafeValues() {
  timeStampRX = 0;
  for (int i = 0; i < numChannelsRX; i++) {
    RX_Channel[i] = 0;
  }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void getReadings() {  // Variables to be sent
  outgoingToESPNOW.header = myHeader;
  outgoingToESPNOW.idPacket = myIDPacket;
  outgoingToESPNOW.checksum = myChecksum;
  outgoingToESPNOW.timeStamp = millis();


  outgoingToESPNOW.channel[1] = joy_y;
  outgoingToESPNOW.channel[3] = !joy_button;
  if (modoJoy == 0) {
      outgoingToESPNOW.channel[0] = joy_x;
      outgoingToESPNOW.channel[4] = 0;
    }
  else {
    outgoingToESPNOW.channel[0] = 0;
    outgoingToESPNOW.channel[4] = joy_x;
  }

  outgoingToESPNOW.channel[7] = RX_Channel[7];
  for (int i = 8; i < 16; i++) {
    outgoingToESPNOW.channel[i] = RX_Channel[i];  // Valores de ejemplo
  }
}

void iniciarESPNOW() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void actualizarESPNOW() {
  getReadings();
  sendMessageViaESPNOW();
}

void timerESPNOW() {
  const int intervaloDeActualizacion = 20;  // milisegundos entre cada ciclo de temporización
  static unsigned long ultimaActualizacion;
  if ((millis() - ultimaActualizacion) > intervaloDeActualizacion) {
    ultimaActualizacion = millis();
    actualizarESPNOW();  // sending tasks
    timerFailSafe();
    //printPacketRX();
  }
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
      //Serial.println("last packet RX: BAD");
    }
  }
}

void sendMessageViaESPNOW() {
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingToESPNOW, sizeof(outgoingToESPNOW));
}
