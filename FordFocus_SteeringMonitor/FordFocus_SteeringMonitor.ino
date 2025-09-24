
/*
   Sketch for show Steering wheel alignment on Ford Focus MK3.5 BR

   CANBUS IDs:
   - Steering Wheel Position: 0x010 / HSCAN

*/

#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>
#include <LiquidCrystal.h>

/*
   CAN BUS
*/

// Can Bus General Variables
const int CAN0_SPI_CS_PIN = 10;
const int CAN0_INT_PIN = 2;
//mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

long unsigned int rxId;
MCP_CAN  CAN0(CAN0_SPI_CS_PIN); // Set CS pin

// Can Bus Control Read Variables
unsigned char len = 0;
unsigned char buf[8];
unsigned int can0_ID;

/*
    LCD
*/

const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Array simbolo grau
byte grau[8] = { B00001100, B00010010, B00010010, B00001100, B00000000, B00000000, B00000000, B00000000, };

int can_orientation;
int lcd_center;
String ori_message;


/*
   Angulation
*/
byte a, b;
unsigned int hex_combine_angle;
float angle;


void setup() {
  Serial.begin(115200);
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
    Serial.print("CAN0: Init OK!\r\n");

    // TODO - Improove the filters and masks to get just 0x010 PID
    //
    CAN0.init_Mask(0, 0, 0xFF0F0000);              // Init first mask...
    CAN0.init_Filt(0, 0, 0xFF100000);              // Init first filter...
    CAN0.init_Mask(1, 0, 0x0F1F0000);              // Init second mask...
    CAN0.init_Filt(2, 0, 0x00100000);              // Init third filter...
    
    CAN0.setMode(MCP_NORMAL);
  } else {
    Serial.print("CAN0: Init Fail!!!\r\n");
  }

  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, grau);
}

void hscan_checks() {


  CAN0.readMsgBuf(&rxId, &len, buf);
  can0_ID = rxId;

  /*
      Calculo de angulo
  */
  if (can0_ID == 0x010) {

    hex_combine_angle = word(buf[6], buf[7]);
    angle = ((hex_combine_angle - 32768) * 1.5); // Formula

    Serial.print("Angulo: ");
    Serial.println(angle);
    Serial.print("Valor Hex CanBus: ");
    Serial.println(hex_combine_angle, HEX);


    /*
       Car Orientation
    */
    byte receivedMessage = buf[4];
    can_orientation = receivedMessage >> 4;
  }
}

void lcd_actions() {
  lcd.clear();
  if (can_orientation == 0 && angle != 0 ) {
    ori_message =  "ESQUERDA";
    lcd.setCursor(0, 0);
    lcd.print(ori_message);
  }
  else if (can_orientation == 8 && angle != 0 ) {
    ori_message =  "DIREITA";
    lcd.setCursor(9, 0);
    lcd.print(ori_message);
  }
  else if (angle == 0 ) {
    ori_message =  "CENTRALIZADO";
    lcd.setCursor(2, 0);
    lcd.print(ori_message);
  }

  lcd_center = ((16 / 2) - (String(angle).length() / 2) - 1);
  lcd.setCursor(lcd_center, 1);
  lcd.print(angle);

  // POsition for angle symbol
  int len_can_angle = String(angle).length();
  int pos = (lcd_center + len_can_angle);

  lcd.setCursor(pos, 1);
  lcd.write((byte)0);
}

void loop() {

  // CAN Bus interations
  hscan_checks();

  // LCD updates
  lcd_actions();

  // Delay between LCD updates
  delay(20);

}
