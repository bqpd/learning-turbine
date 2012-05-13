#include <Wire.h>

// ADXL345
#define DEVICE ((byte)0x40)    //BMA180 device address

#define DATA_X0		0x02	//X-Axis Data 0
#define DATA_X1		0x03	//X-Axis Data 1
#define DATA_Y0		0x04	//Y-Axis Data 0
#define DATA_Y1		0x05	//Y-Axis Data 1
#define DATA_Z0		0x06	//Z-Axis Data 0
#define DATA_Z1		0x07	//Z-Axis Data 1

#define AXIS_SHIFT	2	//Amount to right shift data. The first 2 bits are status bits.

//#define DELAY_RATE	9990	//~100Hz
//#define DELAY_RATE	6600	//~150Hz
//#define DELAY_RATE	4993	//~200Hz
//#define DELAY_RATE	1000	//~1kHz
#define DELAY_RATE	1250	//~800Hz

//Writes val to address register on device
static void writeTo(byte address, byte val) {
  Wire.beginTransmission(DEVICE); //start transmission to device 
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}

//reads num bytes starting from address register on device in to buff array
static void readFrom(byte address, byte num, byte *buff) {
  Wire.beginTransmission(DEVICE); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.requestFrom(DEVICE, num);    // request num bytes from device
  num = Wire.available(); //device may send less than requested (abnormal)
  while(num-- > 0) {
    *(buff++) = Wire.read(); // receive a byte
  }
}

/**
 * Writes val to address register on device if it's different from
 * the current value. This decreases the wear and tear on the EEPROM.
 */
static void writeOptionallyTo(byte address, byte val, byte mask) {
  byte value = 0;
  readFrom(address, sizeof(value), &value);
  if ((value & mask) != (val & mask)) {
    // Keep the unmasked values, and changed the masked values.
    writeTo(address, (value & ~mask) | (val & mask));
  }
  else
    Serial.print("already true at ");
    Serial.println(address);
}


void setup()
{
  Wire.begin();          // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  Serial.flush();

  // Wait for readings to settle down.
  // 10ms Pause is required to write registers.
  delay(15);

  writeOptionallyTo(0x0D, 0x10, 0x10); // Enable image write
  writeOptionallyTo(0x35, 0x0A, 0x0E);
 /*writeOptionallyTo(0x0F, 0xF0, 0xF0); // unlock EE?
  writeOptionallyTo(0x54, 0x0A00, 0x0E00); it looks like writing the image is fine*/
  writeOptionallyTo(0x0D, 0x00, 0x10); // Disable image write for protection
}

void loop()
{
  // 2 byte endian marker`
  // 6 byte buffer for saving data read from the device
  // 2 byte checksum in case there is a reset in the middle of a packet.
  int axis[5] = {0x8081, 0, 0, 0, 0};

  // There are 1,000,000 microseconds per second,
  // and we want to sample about 200 per second.
  // This gives us about the right rate with the rest of the overhead.
  delayMicroseconds(DELAY_RATE - (int)(micros() % DELAY_RATE));
  
  // Each axis reading comes in 14 bit resolution (2 bytes little endian).
  readFrom(DATA_X0, 6, (byte*)(axis+1)); //read the acceleration data

  // Remove status and 0 bits
  axis[1] = axis[1] >> AXIS_SHIFT;
  axis[2] = axis[2] >> AXIS_SHIFT;
  axis[3] = axis[3] >> AXIS_SHIFT;


  // Calculate checksum.
  axis[4] = axis[1] + axis[2] + axis[3];
  // Write whole packet.
  //Serial.write((byte *)axis, sizeof(axis));
  //Serial.println();
  
  Serial.print(axis[1]);
  Serial.print("+");
  Serial.print(axis[2]);
  Serial.print("+");
  Serial.print(axis[3]);
  Serial.print("=");
  Serial.println(axis[4]);
}
