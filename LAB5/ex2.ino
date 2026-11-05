const int DATA_PIN = 11;
const int CLK_PIN  = 13;
const int LOAD_PIN = 10;

const byte REG_DIGIT_0       = 0x01;
const byte REG_DECODE_MODE   = 0x09;
const byte REG_INTENSITY     = 0x0A;
const byte REG_SCAN_LIMIT    = 0x0B;
const byte REG_SHUTDOWN      = 0x0C;
const byte REG_DISPLAY_TEST  = 0x0F;

void max7219Send(byte reg, byte value) {
    digitalWrite(LOAD_PIN, LOW);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, reg);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, value);
    digitalWrite(LOAD_PIN, HIGH);
}

//initializzre registri cip pt matrice leduri
void max7219Init() {
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLK_PIN, OUTPUT);
    pinMode(LOAD_PIN, OUTPUT);
    digitalWrite(LOAD_PIN, HIGH);

    max7219Send(REG_SHUTDOWN, 0x01);
    max7219Send(REG_DISPLAY_TEST, 0x00);
    max7219Send(REG_SCAN_LIMIT, 0x07);
    max7219Send(REG_DECODE_MODE, 0x00);
    max7219Send(REG_INTENSITY, 0x08);

    for (int i = 1; i <= 8; i++) {
        max7219Send(i, 0x00);
    }
}

void displayFrame64(uint64_t frame) {
    for (int r = 0; r < 8; r++) {
        uint8_t rowByte = (frame >> ((7 - r) * 8)) & 0xFF;
        max7219Send(REG_DIGIT_0 + r, rowByte);
    }
}

void setup() {
    Serial.begin(115200);
    max7219Init();
}

void loop() {
    //citeste 8 bytes serial si afiseaza ca un cadru uint64_t
    if (Serial.available() >= 8) {
        uint64_t frame = 0;
        for (int i = 0; i < 8; i++) {
            frame = (frame << 8) | Serial.read();
        }
        displayFrame64(frame);
    }
}