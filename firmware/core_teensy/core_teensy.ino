#include <Dynamixel2Arduino.h>
#include <FastLED.h>
#include <EEPROM.h>

/*---------------------- LEDS ---------------------*/
#define LED_PIN 0
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];


#define SWITCH_POWER 4
#define LED_DATA 0

#define SWITCH_POWER_SENSE 15
#define VCC_SENSE 16
#define VBAT_SENSE 17
#define VEXT_SENSE 18
#define VDXL_SENSE 19
#define CURRENT_SENSE 20

#define DXL_SERIAL   Serial2
#define DEBUG_SERIAL Serial
#define DEFAULT_BAUD 4 //2mbaud
#define DEFAULT_ID 42
const uint8_t DXL_DIR_PIN = 6; // DYNAMIXEL Shield DIR PIN

DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);

const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM = 0xabba; 
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);

#define ADDR_CONTROL_ITEM_BAUD 8

#define ADDR_CONTROL_ITEM_LED0_R 10
#define ADDR_CONTROL_ITEM_LED0_G 11
#define ADDR_CONTROL_ITEM_LED0_B 12
#define ADDR_CONTROL_ITEM_LED1_R 14
#define ADDR_CONTROL_ITEM_LED1_G 15
#define ADDR_CONTROL_ITEM_LED1_B 16
#define ADDR_CONTROL_ITEM_LED2_R 18
#define ADDR_CONTROL_ITEM_LED2_G 19
#define ADDR_CONTROL_ITEM_LED2_B 20


#define ADDR_CONTROL_ITEM_LED 22
#define ADDR_CONTROL_ITEM_POWER 23

#define ADDR_CONTROL_ITEM_VBAT 26
#define ADDR_CONTROL_ITEM_VEXT 28
#define ADDR_CONTROL_ITEM_VCC 30
#define ADDR_CONTROL_ITEM_VDXL 32
#define ADDR_CONTROL_ITEM_CURRENT 34
#define ADDR_CONTROL_ITEM_POWERON 36

uint8_t control_item_led, control_item_power;
uint32_t control_item_led_rgb1, control_item_led_rgb2, control_item_led_rgb3;

uint16_t control_item_vbat, control_item_vext, control_item_vcc, control_item_vdxl, control_item_current, control_item_poweron;

uint8_t baud, id;

uint32_t dxl_to_real_baud(uint8_t baud)
{
  int real_baud = 57600;
  switch(baud)
  {
    case 0: real_baud = 9600; break;
    case 1: real_baud = 57600; break;
    case 2: real_baud = 115200; break;
    case 3: real_baud = 1000000; break;
    case 4: real_baud = 2000000; break;
    case 5: real_baud = 3000000; break;
    case 6: real_baud = 4000000; break;
    case 7: real_baud = 5000000; break;
  }
  return real_baud;
}

void setup() {
  EEPROM.write(0, 42);
  EEPROM.write(1, 6);
  FastLED.addLeds<1, WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Blue;
  leds[2] = CRGB::Green;
  FastLED.show();
  delay(200);
  leds[0] = CRGB::Black;
  leds[1] = CRGB::Black;
  leds[2] = CRGB::Black;
  FastLED.show();

  
  id = EEPROM.read(0);
  baud = EEPROM.read(1);
  dxl_port.begin(dxl_to_real_baud(baud));

  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_2_0);
  dxl.setFirmwareVersion(1);
  dxl.setID(id);

  // Add control items.
  dxl.addControlItem(ADDR_CONTROL_ITEM_BAUD, baud);
  
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED, control_item_led);
  dxl.addControlItem(ADDR_CONTROL_ITEM_POWER, control_item_power);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED0_R, leds[0].r);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED0_G, leds[0].g);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED0_B, leds[0].b);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED1_R, leds[1].r);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED1_G, leds[1].g);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED1_B, leds[1].b);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED2_R, leds[2].r);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED2_G, leds[2].g);
  dxl.addControlItem(ADDR_CONTROL_ITEM_LED2_B, leds[2].b);
  
  dxl.addControlItem(ADDR_CONTROL_ITEM_VBAT, control_item_vbat);
  dxl.addControlItem(ADDR_CONTROL_ITEM_VEXT, control_item_vext);
  dxl.addControlItem(ADDR_CONTROL_ITEM_VCC, control_item_vcc);
  dxl.addControlItem(ADDR_CONTROL_ITEM_VDXL, control_item_vdxl);
  dxl.addControlItem(ADDR_CONTROL_ITEM_CURRENT, control_item_current);
  dxl.addControlItem(ADDR_CONTROL_ITEM_POWERON, control_item_poweron);

  // Add interrupt callback functions to process read/write packet.
  dxl.setWriteCallbackFunc(write_callback_func);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_POWER, OUTPUT);
  pinMode(LED_DATA, OUTPUT);

  digitalWrite(SWITCH_POWER, HIGH);
  
  pinMode(SWITCH_POWER_SENSE, INPUT);
  pinMode(VCC_SENSE, INPUT_PULLDOWN);
  pinMode(VBAT_SENSE, INPUT_PULLDOWN);
  pinMode(VEXT_SENSE, INPUT_PULLDOWN);
  pinMode(VDXL_SENSE, INPUT_PULLDOWN);
  pinMode(CURRENT_SENSE, INPUT);
}

void loop() {
  control_item_poweron = analogRead(SWITCH_POWER_SENSE);
  control_item_vcc = analogRead(VCC_SENSE);
  control_item_vbat = analogRead(VBAT_SENSE);
  control_item_vext = analogRead(VEXT_SENSE);
  control_item_vdxl = analogRead(VDXL_SENSE);
  control_item_current = analogRead(CURRENT_SENSE);
  /*if(sample == 0)
  {
    DEBUG_SERIAL.print(control_item_poweron);
    DEBUG_SERIAL.print("\t");
    DEBUG_SERIAL.print(control_item_vcc);
    DEBUG_SERIAL.print("\t");
    DEBUG_SERIAL.print(control_item_vbat);
    DEBUG_SERIAL.print("\t");
    DEBUG_SERIAL.print(control_item_vext);
    DEBUG_SERIAL.print("\t");
    DEBUG_SERIAL.print(control_item_vdxl);
    DEBUG_SERIAL.print("\t");
    DEBUG_SERIAL.print(control_item_current);
    DEBUG_SERIAL.print("\n");
  }
  sample = (sample + 1)%100;*/
  
  dxl.processPacket();
  if(dxl.getID() != id) // since we cant add the id as a control item, we need to check if it has been updated manually
  {
    id = dxl.getID();
    EEPROM.write(0, id);
  }
}

void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == ADDR_CONTROL_ITEM_BAUD)
  {
    dxl_port.end();
    dxl_port.begin(dxl_to_real_baud(baud));
    EEPROM.write(1, baud);
  }else if(item_addr == ADDR_CONTROL_ITEM_LED){
    digitalWrite(LED_BUILTIN, control_item_led);
    //DEBUG_SERIAL.println("LED");
  }
  else if (item_addr == ADDR_CONTROL_ITEM_POWER){
    digitalWrite(SWITCH_POWER, control_item_power);
    //DEBUG_SERIAL.print("POWER SET TO");
    //DEBUG_SERIAL.println(control_item_power);
  }
  else if(item_addr >= ADDR_CONTROL_ITEM_LED0_R && item_addr <= ADDR_CONTROL_ITEM_LED2_B)
  {
    FastLED.show();
  }
}
