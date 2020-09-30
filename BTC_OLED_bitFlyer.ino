#include <SPI.h>
#include <Ethernet.h>
#include "U8glib.h"
#include <ArduinoJson.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);

//char *insComma(const char *num) {
//    static char BUF[32];
//    int i=0, j=0;
//    int len = strlen(num);
//    while (i < len) {
//        BUF[j++] = num[i++];
//        if ((len-i)%3 == 0 && i<len-1 && BUF[j-1] != '-' && BUF[j-1] != '+') 
//            BUF[j++] = ',';
//    }
//    BUF[j] = '\0';
//    return BUF;
//}
//
//char *xitoa(int n) {
//    char buf[32];
//    return insComma(itoa(n, buf, 10));  
//}

void parseOLED(long e1, long e2, long e3) {
 u8g.begin();
 u8g.firstPage();
  do {
    u8g.setColorIndex(1);
    u8g.drawBox(0, 0, 128, 64); //塗りつぶし
    u8g.setColorIndex(0);

    u8g.setPrintPos(8, 16);
    u8g.setFont(u8g_font_9x18);
    u8g.print("BTC/JPY");

    u8g.setFont(u8g_font_fur17r);
    u8g.setPrintPos(32, 40);
    u8g.print(e1);
//    String Ee1 = String(e1);
//    Ee1 += 'aaa';
//Serial.println(Ee1);

    u8g.setFont(u8g_font_6x10);

    u8g.setPrintPos(3, 56);
    u8g.print("JSON from");
    
    u8g.setPrintPos(2, 64);
    u8g.print("https://bitflyer.com/api/echo/price");
    
//    u8g.setPrintPos(32, 48);
//    u8g.print(e2);
//    u8g.setPrintPos(32, 64);
//    u8g.print(e3);
  } while (u8g.nextPage());
}

void setup() {

  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;
  
  byte mac[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000);

  Serial.println(F("Connecting..."));

  // Connect to HTTP server
  EthernetClient client;
  client.setTimeout(10000);
  if (!client.connect("bitflyer.com", 80)) {
    Serial.println(F("Connection failed"));
    return;
  }

  Serial.println(F("Connected!"));

  // Send HTTP request
  client.println(F("GET /api/echo/price HTTP/1.0"));
  client.println(F("Host: bitflyer.com"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
  // Allocate the JSON document
  // Use arduinojson.org/v6/assistant to compute the capacity.
  const size_t capacity = JSON_OBJECT_SIZE(3) + 60;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Disconnect
  client.stop();
  
  // Fetch values.
  //
  // Most of the time, you can rely on the implicit casts.
  // In other case, you can do doc["time"].as<long>();
  long mid = doc["mid"];
  long ask = doc["ask"];
  long bid = doc["bid"];

  // Print values.
  Serial.println(mid);
  Serial.println(ask);
  Serial.println(bid);

  parseOLED(mid, ask, bid);
}

void loop() {
}
