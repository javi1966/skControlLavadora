
#include <ESP8266WiFi.h>

//AP definitions

#define AP_SSID "Wireless-N"
#define AP_PASSWORD "z123456z"

#define  LED  5  //D1 es GPIO5
#define REPORT_INTERVAL 10 // in sec

WiFiClient client;

const char* server = "api.carriots.com";
const String APIKEY = "9dccd2c601b7658bda4207f165a42f92f17d25accdf9f79121f61bc9c402fed3";   // Replace with your Carriots apikey
const String DEVICE = "defaultDevice@jeg1966.jeg1966";  // Replace with the id_developer of your device
int valor=0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200);



  digitalWrite(LED, HIGH);

  delay(1000);

  digitalWrite(LED, LOW);

  Serial.println("\r\n");
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

  wifiConnect();

  Serial.println();

  IPAddress http_server_ip = WiFi.localIP();

  Serial.print("nuestra server IP:");
  Serial.print(http_server_ip);
  Serial.print("\r\n");

  //primera lectura

  //valor = analogRead(0);

  delay(1000);
  
}
//*************************************************************************************

void loop() {
  // put your main code here, to run repeatedly:


  valor = analogRead(0);

  Serial.println(String(valor));

  if (valor > 600 )
    sendValores(valor);

  int cnt = REPORT_INTERVAL;

  while (cnt--)
    delay(1000);

}
//********************************************************************************************************************************
void wifiConnect()
{
  Serial.print("Connecting to AP");
  WiFi.begin(AP_SSID, AP_PASSWORD);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("*");

    if (++timeout > 100)
    {
      Serial.println("Sin Conexion WIFI");
      while (1) {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
      }
    }
  }

  Serial.println("");
  Serial.println("WiFi conectado");
}
//**********************************************************************************************************************
void sendValores(int Corr)
{


  while (!client.connect(server, 80)) {
    Serial.println("Conexion Carriots fallo");
    wifiConnect();
  }


  String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE + "\",\"at\":\"now\",\"data\":{\"Corriente\":\"" + Corr + "\"}}";

  // Make an HTTP request
  client.println("POST /streams HTTP/1.1");
  client.println("Host: api.carriots.com");
  client.println("Accept: application/json");
  client.println("User-Agent: Arduino-Carriots");
  client.println("Content-Type: application/json");
  client.print("carriots.apikey: ");
  client.println(APIKEY);
  client.print("Content-Length: ");
  int thisLength = json.length();
  client.println(thisLength);
  client.println("Connection: close");
  client.println();
  client.println(json);
  // client.stop();

  digitalWrite(LED, HIGH); //flashing led
  delay(500);
  digitalWrite(LED, LOW);

  Serial.println(json);
  Serial.println("Connection closed");
}


