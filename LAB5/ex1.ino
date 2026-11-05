
const int ROWS = 8;
const int COLS = 8;

const int DATA_PIN = 11;
const int CLK_PIN  = 13;
const int LOAD_PIN = 10;

const byte REG_DIGIT_0       = 0x01;
const byte REG_DECODE_MODE   = 0x09;
const byte REG_INTENSITY     = 0x0A;
const byte REG_SCAN_LIMIT    = 0x0B;
const byte REG_SHUTDOWN      = 0x0C;
const byte REG_DISPLAY_TEST  = 0x0F;

//matrice stari
uint8_t grid[ROWS][COLS] = {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

uint8_t nextGrid[ROWS][COLS];

void max7219Send(byte reg, byte value) {
    digitalWrite(LOAD_PIN, LOW);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, reg);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, value);
    digitalWrite(LOAD_PIN, HIGH);
}

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

void renderToLEDMatrix() {
    for (int r = 0; r < ROWS; r++) {
        byte rowByte = 0;
        for (int c = 0; c < COLS; c++) {
            if (grid[r][c] == 1) {
                rowByte |= (1 << (7 - c));
            }
        }
        max7219Send(REG_DIGIT_0 + r, rowByte);
    }
}

// numara vecinii vii si verifica sa nu iasa din matrice
int countNeighbors(int r, int c) {
    int count = 0;
    
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;

            int nr = r + dr;
            int nc = c + dc;

            //daca vecinul e in matrice
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                count += grid[nr][nc];
            }
        }
    }
    return count;
}

/*
 * 1. celula cu <2 vecini vii moare
 * 2. celula vie cu 2 sau 3 vecini traieste
 * 3. celula cu >3 vecini vii moare
 * 4. celula moarta cu 3 vecini invie
 */
void updateGrid() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int neighbors = countNeighbors(r, c);

            if (grid[r][c] == 1) {
                //celula vie
                if (neighbors < 2 || neighbors > 3) {
                    nextGrid[r][c] = 0;
                } else {
                    nextGrid[r][c] = 1;
                }
            } else {
                //celula moarta
                if (neighbors == 3) {
                    nextGrid[r][c] = 1;
                } else {
                    nextGrid[r][c] = 0;
                }
            }
        }
    }

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            grid[r][c] = nextGrid[r][c];
        }
    }
}

void setup() {
    Serial.begin(9600);
    max7219Init();
}

void loop() {
    renderToLEDMatrix();

    updateGrid();      
    delay(500);        
}