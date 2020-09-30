#include "qrcode.h" //https://github.com/ricmoo/qrcode/
#include "U8glib.h"

char *btcAddress = "bc1q8dxaxhnnq2nhjnsdz5pqkjhe36c380whkxp44v";
int AddrLength = strlen(btcAddress);

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);   // I2C / TWI
 
void display_qrcode(char *text) {   
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(4)]; //バージョン4のQRコード　114バイトデータ 33x33ドット
    qrcode_initText(&qrcode, qrcodeData, 4, ECC_LOW, text);

    String Addr = btcAddress;
 
    u8g.firstPage();
    do {
        u8g.setColorIndex(1);
        u8g.drawBox(0, 0, 128, 64); //塗りつぶし
        u8g.setColorIndex(0);
        for (uint8_t y = 0; y < qrcode.size; y++) {
            for (uint8_t x = 0; x < qrcode.size; x++) {
                if (qrcode_getModule(&qrcode, x, y))
                    u8g.drawBox(15 + x, 15 + y, 1, 1);
            }
        }
        u8g.setPrintPos(0,0);
        u8g.print("BTC Address");
        for (int i = 0; AddrLength - i*10 >= 0; i++){
            u8g.setPrintPos(60, AddrLength-30+12*i);
            u8g.print(Addr.substring(i*10,i*10+11));
            Serial.println(i);
        }

    } while ( u8g.nextPage() );
}
 
void setup() {
    u8g.begin();
    Serial.begin(9600);
    u8g.setFont(u8g_font_fixed_v0);
    display_qrcode(btcAddress);
}
 
void loop() {
}
