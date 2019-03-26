
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//AP definitions

#define AP_SSID "Wireless-N"
#define AP_PASSWORD "z123456z"

#define  LED  5  //D1 es GPIO5
#define REPORT_INTERVAL 30 // in sec

WiFiClient client;

String writeAPIKey = "V0GX5WB22R9BWOKQ";
const char* host = "api.thingspeak.com";
int valor = 0;

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

//******************************************************************************************************************
float getVPP()
{
  float result;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(0);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 3.3) / 1024.0;

  return result;
}

//*************************************************************************************************************

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

  Voltage = getVPP();
  VRMS = (Voltage / 2.0) * 0.707;
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  Serial.print(AmpsRMS);
  Serial.println(" Amps RMS");


  if (AmpsRMS > 0.5F ) {
    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      String body = "field3=";
      body +=  String(AmpsRMS);


      Serial.println(body);

      http.begin(host, 80, "https://api.thingspeak.com/update?api_key=" + writeAPIKey + "&" + body);

      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {


        //String payload = http.getString();

        Serial.print("Resultado: ");
        Serial.println(http.getString());

        digitalWrite(LED, HIGH); //flashing led
        delay(500);
        digitalWrite(LED, LOW);


      }

      http.end();
    }//WL_CONNECTED


  }


  delay(20000);

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
/*
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
*/

