// demo: CAN-BUS Shield, send data
// loovee@seeed.cc

#include <mcp_can.h>

#include <SPI.h>

#include <stdint.h>

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
unsigned char flagRecv = 0;

uint16_t bytesSent = 0;
uint16_t pauseTimer = 0;

MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

void setup() {
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) // init can bus : baudrate = 500k
    {
        Serial.println("E");
        delay(100);
    }
    //Serial.print("OK");
    Serial.setTimeout(2);
    //attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}

void MCP2515_ISR() {
    flagRecv = 1;
}

void loop() {
    /*
    if (flagRecv)
    {
      flagRecv = 0;
      */

    /*
  }
  */

    int c;
    while (Serial.available()) {
        c = Serial.read();
        if (c == 0xAA) // outside the printable chars
        {
            serial_receive();
        }
    }
    if (CAN_MSGAVAIL == CAN.checkReceive()) // check if data coming
    {
        serial_transmit();
    }
}

void serial_receive(void) {
    uint8_t header[9], msgbuf[8];
    const uint8_t * tsbuf, * idbuf;
    uint32_t timestamp;
    uint8_t dlc, eofr;
    uint32_t id;

    if (Serial.readBytes(header, 9) < 9) {
        return;
    }

    dlc = header[4];
    if (dlc > 8) {
        return;
    }

    if (dlc > 0) {
        if (Serial.readBytes(msgbuf, dlc) < dlc) {
            return;
        }
    }

    tsbuf = header;
    idbuf = header + 5;

    timestamp = ((uint32_t) tsbuf[3] << 24) | ((uint32_t) tsbuf[2] << 16) | ((uint32_t) tsbuf[1] << 8) | ((uint32_t) tsbuf[0]);
    id = ((uint32_t) idbuf[3] << 24) | ((uint32_t) idbuf[2] << 16) | ((uint32_t) idbuf[1] << 8) | ((uint32_t) idbuf[0]);
    if (id > 0x1FFFFFFF)
        return;

    // transmit
    CAN.sendMsgBuf(id, 1, dlc, msgbuf);
}

void serial_transmit(void) {
    uint8_t buf[8 + 11 + 3];
    uint32_t timestamp;
    uint8_t dlc;
    uint32_t id;

    CAN.readMsgBufID( & id, & dlc, buf + 10);
    timestamp = millis();

    // we need to take a breather occasionally or serial port buffers fill up and python-can crashes
    if (bytesSent > 200) {
        bytesSent = 0;
        pauseTimer = timestamp + 10;
    }
    if (pauseTimer > timestamp) {
        return;
    }

    if (dlc > 8)
        return;

    buf[0] = 0xAA;
    buf[1] = (timestamp) & 0xFF;
    buf[2] = (timestamp >> 8) & 0xFF;
    buf[3] = (timestamp >> 16) & 0xFF;
    buf[4] = (timestamp >> 24) & 0xFF;
    buf[5] = dlc;
    buf[6] = (id) & 0xFF;
    buf[7] = (id >> 8) & 0xFF;
    buf[8] = (id >> 16) & 0xFF;
    buf[9] = (id >> 24) & 0xFF;
    buf[10 + dlc] = 0xBB;
    buf[11 + dlc] = 0xBB;
    buf[12 + dlc] = '\r';
    buf[13 + dlc] = '\n';
    Serial.write(buf, 14 + dlc);
    bytesSent += 14 + dlc;
    //	Serial.write(0xAA);
    //	Serial.write((timestamp) & 0xFF);
    //	Serial.write((timestamp >> 8) & 0xFF);
    //	Serial.write((timestamp >> 16) & 0xFF);
    //	Serial.write((timestamp >> 24) & 0xFF);
    //	Serial.write(dlc);
    //	Serial.write((id) & 0xFF);
    //	Serial.write((id >> 8) & 0xFF);
    //	Serial.write((id >> 16) & 0xFF);
    //	Serial.write((id >> 24) & 0xFF);
    //	Serial.write(buf, dlc);
    //	Serial.write(0xBB);
}

// END FILE
