const int BUT1 = D7;
const int BUT2 = D8;
const int BUT3 = D9;
const int BUT4 = D10;

const int LED1 = D0;
const int LED2 = D1;
const int LED3 = D2;
const int LED4 = D3;

const int BRIGHTNESS = 30;

bool surrenderAvailable = true;
bool hitSoft17 = true;
bool doubleAvailable = true;
bool DASAvailable = true;
bool statusChanged = false;

// Stable debounced states (confirmed after debounce period)
bool lastBUT1State = HIGH;
bool lastBUT2State = HIGH;
bool lastBUT3State = HIGH;
bool lastBUT4State = HIGH;
bool lastBUT4StableState = HIGH;

// Raw readings from previous loop (used ONLY for triggering the debounce timer)
bool lastBUT1Reading = HIGH;
bool lastBUT2Reading = HIGH;
bool lastBUT3Reading = HIGH;
bool lastBUT4Reading = HIGH;

unsigned long BUT4PressStartTime = 0;
unsigned long BUT4PressDuration = 0;
const unsigned long longPressThreshold = 1000;

unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;
const unsigned long debounceDelay = 50;

unsigned int playerCard1 = 0;
unsigned int playerCard2 = 0;
unsigned int dealerCard = 0;
unsigned int playerTotal = 0;
unsigned int action = 0;

// Actions: 0=Stand, 1=Hit, 2=Double, 3=Split, 4=Surrender
// idx = (hitSoft17?1:0) | (doubleAvailable?2:0) | (DASAvailable?4:0) | (surrenderAvailable?8:0)
// Columns:  0=A  1=2  2=3  3=4  4=5  5=6  6=7  7=8  8=9  9=10
// Hard rows: total 8(0) 9(1) 10(2) 11(3) 12(4) 13(5) 14(6) 15(7) 16(8) 17(9)
// Soft rows: A2(0) A3(1) A4(2) A5(3) A6(4) A7(5) A8(6) A9(7)
// Pair rows: AA(0) 22(1) 33(2) 44(3) 55(4) 66(5) 77(6) 88(7) 99(8) TT(9)

// ── idx 0: S17, no Double, no DAS, no Surrender ─────────────────────────────
const char h0[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s0[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {0,0,0,0,0,0,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p0[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 1: H17, no Double, no DAS, no Surrender ─────────────────────────────
const char h1[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s1[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {1,0,0,0,0,0,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p1[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 2: S17, Double, no DAS, no Surrender ────────────────────────────────
const char h2[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11  S17: Double vs A
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s2[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {0,2,2,2,2,2,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p2[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 3: H17, Double, no DAS, no Surrender ────────────────────────────────
const char h3[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s3[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {1,2,2,2,2,2,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p3[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 4: S17, no Double, DAS, no Surrender ────────────────────────────────
const char h4[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s4[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {0,0,0,0,0,0,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p4[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 5: H17, no Double, DAS, no Surrender ────────────────────────────────
const char h5[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s5[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {1,0,0,0,0,0,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p5[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 6: S17, Double, DAS, no Surrender ───────────────────────────────────
const char h6[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11  S17: Double vs A
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s6[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {0,2,2,2,2,2,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p6[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 7: H17, Double, DAS, no Surrender ───────────────────────────────────
const char h7[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {1,0,0,0,0,0,1,1,1,1}, // 15
  {1,0,0,0,0,0,1,1,1,1}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s7[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {1,2,2,2,2,2,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p7[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {3,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 8: S17, no Double, no DAS, Surrender ────────────────────────────────
const char h8[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15  S17: Surrender vs A
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s8[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {0,0,0,0,0,0,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p8[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 9: H17, no Double, no DAS, Surrender ────────────────────────────────
const char h9[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {4,0,0,0,0,0,0,0,0,0}, // 17
};
const char s9[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {1,0,0,0,0,0,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p9[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 10: S17, Double, no DAS, Surrender ──────────────────────────────────
const char h10[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11  S17: Double vs A
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15  S17: Surrender vs A
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s10[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {0,2,2,2,2,2,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p10[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 11: H17, Double, no DAS, Surrender ──────────────────────────────────
const char h11[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {4,0,0,0,0,0,0,0,0,0}, // 17
};
const char s11[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {1,2,2,2,2,2,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p11[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,1,1,3,3,3,3,1,1,1}, // 2,2
  {1,1,1,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,1,1,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,1,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 12: S17, no Double, DAS, Surrender ──────────────────────────────────
const char h12[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15  S17: Surrender vs A
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s12[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {0,0,0,0,0,0,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p12[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 13: H17, no Double, DAS, Surrender ──────────────────────────────────
const char h13[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,1,1,1,1,1,1,1,1}, // 9
  {1,1,1,1,1,1,1,1,1,1}, // 10
  {1,1,1,1,1,1,1,1,1,1}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {4,0,0,0,0,0,0,0,0,0}, // 17
};
const char s13[8][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // A,2
  {1,1,1,1,1,1,1,1,1,1}, // A,3
  {1,1,1,1,1,1,1,1,1,1}, // A,4
  {1,1,1,1,1,1,1,1,1,1}, // A,5
  {1,1,1,1,1,1,1,1,1,1}, // A,6
  {1,0,0,0,0,0,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,0,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p13[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,1,1,1,1,1,1,1,1,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 14: S17, Double, DAS, Surrender ─────────────────────────────────────
const char h14[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11  S17: Double vs A
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15  S17: Surrender vs A
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {0,0,0,0,0,0,0,0,0,0}, // 17
};
const char s14[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {0,2,2,2,2,2,0,0,1,1}, // A,7  S17: Stand vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p14[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

// ── idx 15: H17, Double, DAS, Surrender ─────────────────────────────────────
const char h15[10][10] = {
  {1,1,1,1,1,1,1,1,1,1}, // 8
  {1,1,2,2,2,2,1,1,1,1}, // 9
  {1,2,2,2,2,2,2,2,2,1}, // 10
  {2,2,2,2,2,2,2,2,2,2}, // 11
  {1,1,1,0,0,0,1,1,1,1}, // 12
  {1,0,0,0,0,0,1,1,1,1}, // 13
  {1,0,0,0,0,0,1,1,1,1}, // 14
  {4,0,0,0,0,0,1,1,1,4}, // 15
  {4,0,0,0,0,0,1,1,4,4}, // 16
  {4,0,0,0,0,0,0,0,0,0}, // 17
};
const char s15[8][10] = {
  {1,1,1,1,2,2,1,1,1,1}, // A,2
  {1,1,1,1,2,2,1,1,1,1}, // A,3
  {1,1,1,2,2,2,1,1,1,1}, // A,4
  {1,1,1,2,2,2,1,1,1,1}, // A,5
  {1,1,2,2,2,2,1,1,1,1}, // A,6
  {1,2,2,2,2,2,0,0,1,1}, // A,7  H17: Hit vs A
  {0,0,0,0,0,2,0,0,0,0}, // A,8
  {0,0,0,0,0,0,0,0,0,0}, // A,9
};
const char p15[10][10] = {
  {3,3,3,3,3,3,3,3,3,3}, // A,A
  {1,3,3,3,3,3,3,1,1,1}, // 2,2
  {1,3,3,3,3,3,3,1,1,1}, // 3,3
  {1,1,1,1,3,3,1,1,1,1}, // 4,4
  {1,2,2,2,2,2,2,2,2,1}, // 5,5
  {1,3,3,3,3,3,1,1,1,1}, // 6,6
  {1,3,3,3,3,3,3,1,1,1}, // 7,7
  {4,3,3,3,3,3,3,3,3,3}, // 8,8
  {0,3,3,3,3,3,0,3,3,0}, // 9,9
  {0,0,0,0,0,0,0,0,0,0}, // T,T
};

const char (*hardTables[16])[10] = {h0,h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,h13,h14,h15};
const char (*softTables[16])[10] = {s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15};
const char (*pairTables[16])[10] = {p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15};

void printActionName(unsigned int act) {
  switch(act) {
    case 0: Serial.print("STAND");     break;
    case 1: Serial.print("HIT");       break;
    case 2: Serial.print("DOUBLE");    break;
    case 3: Serial.print("SPLIT");     break;
    case 4: Serial.print("SURRENDER"); break;
    default: Serial.print("UNKNOWN("); Serial.print(act); Serial.print(")"); break;
  }
}

void printCardName(unsigned int card) {
  if      (card == 0)  Serial.print("?");
  else if (card == 1)  Serial.print("A");
  else if (card == 10) Serial.print("T");
  else                 Serial.print(card);
}

void showAction(unsigned int act) {
  Serial.print("[LED] Action="); printActionName(act); Serial.println();
  analogWrite(LED1, 0);
  analogWrite(LED2, 0);
  analogWrite(LED3, 0);
  analogWrite(LED4, 0);
  if      (act == 0) analogWrite(LED1, BRIGHTNESS);
  else if (act == 1) analogWrite(LED2, BRIGHTNESS);
  else if (act == 2) analogWrite(LED3, BRIGHTNESS);
  else if (act == 3) analogWrite(LED4, BRIGHTNESS);
  else {
    analogWrite(LED1, BRIGHTNESS);
    analogWrite(LED2, BRIGHTNESS);
    analogWrite(LED3, BRIGHTNESS);
    analogWrite(LED4, BRIGHTNESS);
  }
}

void showRules() {
  analogWrite(LED1, surrenderAvailable ? BRIGHTNESS : 0);
  analogWrite(LED2, hitSoft17          ? BRIGHTNESS : 0);
  analogWrite(LED3, doubleAvailable    ? BRIGHTNESS : 0);
  analogWrite(LED4, DASAvailable       ? BRIGHTNESS : 0);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("=== Blackjack Calculator ===");
  Serial.println("Flags: Sur=Y H17=Y Dbl=Y DAS=Y  (all LEDs on is correct at startup)");
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(BUT3, INPUT_PULLUP);
  pinMode(BUT4, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  showRules();
}

void loop() {
  bool BUT1Reading = digitalRead(BUT1);
  bool BUT2Reading = digitalRead(BUT2);
  bool BUT3Reading = digitalRead(BUT3);
  bool BUT4Reading = digitalRead(BUT4);

  // ── Button 1: playerCard1 / toggle surrender ─────────────────────────────
  // Timer resets only when the RAW reading changes (lastBUT1Reading),
  // not compared against the stable state — this was the core bug.
  if (BUT1Reading != lastBUT1Reading) lastDebounceTime1 = millis();
  lastBUT1Reading = BUT1Reading;
  if ((millis() - lastDebounceTime1) > debounceDelay) {
    if (BUT1Reading == LOW && lastBUT1State == HIGH) {
      if (BUT4Reading == LOW) {
        surrenderAvailable = !surrenderAvailable;
        digitalWrite(LED1, surrenderAvailable ? HIGH : LOW);
        Serial.print("[BUT1+BUT4] surrender=");
        Serial.println(surrenderAvailable ? "Y" : "N");
        statusChanged = true;
      } else {
        playerCard1 = (playerCard1 % 10) + 1;
        Serial.print("[BUT1] card1="); printCardName(playerCard1); Serial.println();
      }
    }
    lastBUT1State = BUT1Reading;
  }

  // ── Button 2: playerCard2 / toggle hitSoft17 ─────────────────────────────
  if (BUT2Reading != lastBUT2Reading) lastDebounceTime2 = millis();
  lastBUT2Reading = BUT2Reading;
  if ((millis() - lastDebounceTime2) > debounceDelay) {
    if (BUT2Reading == LOW && lastBUT2State == HIGH) {
      if (BUT4Reading == LOW) {
        hitSoft17 = !hitSoft17;
        digitalWrite(LED2, hitSoft17 ? HIGH : LOW);
        Serial.print("[BUT2+BUT4] hitSoft17=");
        Serial.println(hitSoft17 ? "Y" : "N");
        statusChanged = true;
      } else {
        playerCard2 = (playerCard2 % 10) + 1;
        Serial.print("[BUT2] card2="); printCardName(playerCard2); Serial.println();
      }
    }
    lastBUT2State = BUT2Reading;
  }

  // ── Button 3: dealerCard / toggle doubleAvailable ────────────────────────
  if (BUT3Reading != lastBUT3Reading) lastDebounceTime3 = millis();
  lastBUT3Reading = BUT3Reading;
  if ((millis() - lastDebounceTime3) > debounceDelay) {
    if (BUT3Reading == LOW && lastBUT3State == HIGH) {
      if (BUT4Reading == LOW) {
        doubleAvailable = !doubleAvailable;
        digitalWrite(LED3, doubleAvailable ? HIGH : LOW);
        Serial.print("[BUT3+BUT4] double=");
        Serial.println(doubleAvailable ? "Y" : "N");
        statusChanged = true;
      } else {
        dealerCard = (dealerCard % 10) + 1;
        Serial.print("[BUT3] dealer="); printCardName(dealerCard); Serial.println();
      }
    }
    lastBUT3State = BUT3Reading;
  }

  // ── Button 4: calculate / toggle DAS (long press) ────────────────────────
  if (BUT4Reading != lastBUT4Reading) lastDebounceTime4 = millis();
  lastBUT4Reading = BUT4Reading;
  if ((millis() - lastDebounceTime4) > debounceDelay) {
    if (BUT4Reading != lastBUT4StableState) {
      lastBUT4StableState = BUT4Reading;

      if (BUT4Reading == LOW) {
        Serial.println("[BUT4] pressed");
        BUT4PressStartTime = millis();
      } else {
        BUT4PressDuration = millis() - BUT4PressStartTime;
        Serial.print("[BUT4] released duration="); Serial.print(BUT4PressDuration);
        Serial.print("ms statusChanged="); Serial.println(statusChanged ? "Y" : "N");

        if (BUT4PressDuration >= longPressThreshold && !statusChanged) {
          DASAvailable = !DASAvailable;
          digitalWrite(LED4, DASAvailable ? HIGH : LOW);
          Serial.print("[BUT4 long] DAS="); Serial.println(DASAvailable ? "Y" : "N");
          statusChanged = true;
        }

        if (!statusChanged) {
          Serial.print("--- CALC: c1="); printCardName(playerCard1);
          Serial.print(" c2=");          printCardName(playerCard2);
          Serial.print(" dealer=");      printCardName(dealerCard);
          Serial.print(" H17=");  Serial.print(hitSoft17          ? "Y" : "N");
          Serial.print(" Dbl=");  Serial.print(doubleAvailable     ? "Y" : "N");
          Serial.print(" DAS=");  Serial.print(DASAvailable        ? "Y" : "N");
          Serial.print(" Sur=");  Serial.println(surrenderAvailable ? "Y" : "N");

          if (playerCard1 == 0 || playerCard2 == 0 || dealerCard == 0) {
            Serial.println("ERROR: card(s) not set, aborting");
            statusChanged = false;
            showRules();
            lastBUT4State = BUT4Reading;
            return;
          }

          int idx = (hitSoft17          ? 1 : 0)
                  | (doubleAvailable    ? 2 : 0)
                  | (DASAvailable       ? 4 : 0)
                  | (surrenderAvailable ? 8 : 0);
          int col = dealerCard - 1;
          Serial.print("idx="); Serial.print(idx);
          Serial.print(" col="); Serial.println(col);

          if (playerCard1 == playerCard2) {
            int row = (playerCard1 == 1) ? 0 : playerCard1 - 1;
            Serial.print("PAIR row="); Serial.println(row);
            action = pairTables[idx][row][col];
          } else if (playerCard1 == 1 || playerCard2 == 1) {
            int otherCard = (playerCard1 == 1) ? playerCard2 : playerCard1;
            if (otherCard >= 10) {
              Serial.println("SOFT 21 -> Stand");
              action = 0;
            } else {
              int row = otherCard - 2;
              Serial.print("SOFT other="); Serial.print(otherCard);
              Serial.print(" row="); Serial.println(row);
              action = softTables[idx][row][col];
            }
          } else {
            playerTotal = playerCard1 + playerCard2;
            if (playerTotal <= 8) {
              Serial.println("HARD <=8 -> Hit");
              action = 1;
            } else if (playerTotal >= 18) {
              Serial.println("HARD >=18 -> Stand");
              action = 0;
            } else {
              int row = playerTotal - 8;
              Serial.print("HARD total="); Serial.print(playerTotal);
              Serial.print(" row="); Serial.println(row);
              action = hardTables[idx][row][col];
            }
          }

          Serial.print("Result: "); printActionName(action); Serial.println();
          showAction(action);

          Serial.println("Press BUT4 to continue...");
          while (digitalRead(BUT4) == HIGH) { delay(10); }
          while (digitalRead(BUT4) == LOW)  { delay(10); }
          Serial.println("Resetting hand.");

          playerCard1 = 0; playerCard2 = 0;
          dealerCard  = 0; playerTotal = 0; action = 0;
        }

        statusChanged = false;
        showRules();
      }
    }
    lastBUT4State = BUT4Reading;
  }
}