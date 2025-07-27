const int BUT1 = D7; // Card 1, change surrender availability
const int BUT2 = D8; // Card 2, change dealer hit/stand on soft 17
const int BUT3 = D9; // dealerCard, change double availability
const int BUT4 = D10; // Finish hand(quick press), alteration key(press other key while holding), Change DAS Available(hold then release with no alteration)

const int LED1 = D0; // Surrender availability, on = available 
const int LED2 = D1; // Dealer hit/stand soft 17, on = hit
const int LED3 = D2; // Double availability, on = available
const int LED4 = D3; // DAS availability, on = available

bool surrenderAvailable = True;
bool hitSoft17 = True;
bool doubleAvailable = True;
bool DASAvailable = True;
bool statusChanged = False;

char table[100];

bool lastBUT1State = LOW;
bool lastBUT2State = LOW;
bool lastBUT3State = LOW;
bool lastBUT4State = LOW;

bool lastBUT4StableState = LOW;
unsigned long BUT4PressStartTime = 0;
unsigned long BUT4PressDuration = 0;

const unsigned long longPressThreshold = 1000; // 1 sec

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

const char Hard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {4, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char Soft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char Pair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {4, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noDoubleHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {4, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDoubleSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDoublePair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {4, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  10,10
};


const char noDASHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {4, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDASSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDASPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {4, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noDoublenoDASHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {4, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {4, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDoublenoDASSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDoublenoDASPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {4, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  10,10
};


const char noDoublenoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDoublenoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDoublenoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  10,10
};


const char noDASnoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDASnoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDASnoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noDoublenoDASnoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noDoublenoDASnoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 1, 1, 1, 1, 1, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noDoublenoDASnoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 11  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};

// -------------------------------------------------------------------

const char noHitHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitPair[10][10] = { // not necessary
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoDoubleHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDoubleSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 0, 0, 0, 0, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDoublePair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoDASHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDASSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDASPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoDoublenoDASHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 4}, // 15
  {4, 0, 0, 0, 0, 0, 1, 1, 4, 4}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDoublenoDASSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 0, 0, 0, 0, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDoublenoDASPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoDoublenoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDoublenoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 1, 1, 1, 1, 1, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDoublenoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 1   A,A
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 3, 3, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 3, 3, 3, 3, 3, 1, 1, 1, 1}, // 11  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHitnoDASnoSurHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 9
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 2}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDASnoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 2, 2, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 2, 2, 2, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 2, 2, 2, 2, 1, 1, 1, 1}, // 7   A,6
  {1, 2, 2, 2, 2, 2, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDASnoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 12  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20  10,10
};


const char noHinoDoublenoDASnoSurtHard[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 9
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 11
  {1, 1, 1, 0, 0, 0, 1, 1, 1, 1}, // 12
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 13
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 14
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 15
  {1, 0, 0, 0, 0, 0, 1, 1, 1, 1}, // 16
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
};
const char noHitnoDoublenoDASnoSurSoft[10][8] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 3   A,2
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 4   A,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 5   A,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 6   A,5
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 7   A,6
  {1, 1, 1, 1, 1, 1, 0, 0, 1, 1}, // 8   A,7
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 9   A,8
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  A,9
};
const char noHitnoDoublenoDASnoSurPair[10][10] = {
  // 0 = Stand, 1 = Hit, 2 = Double, 3 = Split, 4 = Surrender
// A  2  3  4  5  6  7  8  9  10 Dealer
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 2   A,A
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 4   2,2
  {1, 1, 1, 3, 3, 3, 3, 1, 1, 1}, // 6   3,3
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 8   4,4
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 10  5,5
  {1, 1, 3, 3, 3, 3, 1, 1, 1, 1}, // 11  6,6
  {1, 3, 3, 3, 3, 3, 3, 1, 1, 1}, // 14  7,7
  {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}, // 16  8,8
  {0, 3, 3, 3, 3, 3, 0, 3, 3, 0}, // 18  9,9
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 10  10,10
};



void setup() {
  // Set pin modes for switches
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(BUT3, INPUT_PULLUP);
  pinMode(BUT4, INPUT_PULLUP);

  // Set pin modes for LEDs
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // Set initial gamerule LEDs
  digitalWrite(LED1, High);
  digitalWrite(LED2, High);
  digitalWrite(LED3, High);
  digitalWrite(LED4, High);
}

void loop() {
  // Gets all button readings
  bool BUT1Reading = digitalRead(BUT1);
  bool BUT2Reading = digitalRead(BUT2);
  bool BUT3Reading = digitalRead(BUT3);
  bool BUT4Reading = digitalRead(BUT4);

  //Button 1 ---------------------------------------------
  // If the state changes, start the debounce timer
  if (BUT1Reading != lastBUT1State) {
    lastDebounceTime1 = millis();
  }

  // Runs code only if debounce timer has passed
  if ((millis() - lastDebounceTime1) > debounceDelay) {
    // Checks if button is being depressed so code doesn't run multiple times
    if (BUT1Reading == HIGH && lastBUT1State == LOW) {
      // Checks if Button 4 is being held, and does alterations instead of card changes if so
      if (BUT4Reading == HIGH) {
        if surrenderAvailable {
          surrenderAvailable = False;
          digitalWrite(LED1, Low);
        } else {
          surrenderAvailable = True;
          digitalWrite(LED1, High);
        }
        statusChanged = True; // Used for checking if changes were made for surrender alterations
      }
      playerCard1 += 1;
    }
    lastBUT1State = BUT1Reading; // Save state of button to check if it's changed
  }


  // Button 2 --------------------------------
  // If the state changes, start the debounce timer
  if (BUT2Reading != lastBUT2State) {
    lastDebounceTime2 = millis();
  }

  // Runs code only if debounce timer has passed
  if ((millis() - lastDebounceTime2) > debounceDelay) {
    // Checks if button is being depressed so code doesn't run multiple times
    if (BUT2Reading == HIGH && lastBUT2State == LOW) {
      // Checks if Button 4 is being held, and does alterations instead of card changes if so
      if (BUT4Reading == HIGH) {
        if hitSoft17 {
          hitSoft17 = False;
          digitalWrite(LED2, Low);
        } else {
          hitSoft17 = True;
          digitalWrite(LED2, High);
        }
        statusChanged = True; // Used for checking if changes were made for surrender alterations
      }
      playerCard2 += 1;
    }
    lastBUT2State = BUT2Reading; // Save state of button to check if it's changed
  }

  // Button 3 ---------------------------------
  // If the state changes, start the debounce timer
  if (BUT3Reading != lastBUT3State) {
    lastDebounceTime3 = millis();
  }

  // Runs code only if debounce timer has passed
  if ((millis() - lastDebounceTime3) > debounceDelay) {
    // Checks if button is being depressed so code doesn't run multiple times
    if (BUT3Reading == High && lastBUT3State == Low) {
      // Checks if Button 4 is being held, and does alterations instead of card changes if so
      if (BUT4Reading == HIGH) {
        if doubleAvailable {
          doubleAvailable = False;
          digitalWrite(LED3, Low);
        } else {
          doubleAvailable = True;
          digitalWrite(LED3, High);
        }
        statusChanged = True; // Used for checking if changes were made for surrender alterations
      }
      dealerCard += 1;
    }
    lastBUT3State = BUT3Reading; // Save state of button to check if it's changed
  }

  // Button 4 ---------------------------------
  // If the state changes, start the debounce timer
  if (BUT4Reading != lastBUT4State) {
    lastDebounceTime4 = millis();
  }
  // Runs code only if debounce timer has passed
  if ((millis() - lastDebounceTime4) > debounceDelay) {
    // Runs only when the stable state has changed
    if (BUT4Reading != lastBut4StableState) {
      lastBUT4StableState = BUT4Reading; // Sets the last stable state, the state known not to be a debounce error, used for checking if the state has changed without being influenced by bouncing

      if (BUT4Reading == High) {
        // Runs when button is pressed
        BUT4pressStartTime = millis(); // Assigns the start time when the button is first depressed
      } else {
        // Runs when button is released
        BUT4PressDuration = millis() - but4PressStartTime; // Calculates the press duration based off the press time and current time

        // Changes the DAS status if it's a long press and no other changes were made
        if ((BUT4PressDuration >= longPressThreshold) && statusChanged == False) {
          if DASAvailable {
            DASAvailable = False;
            digitalWrite(LED4, Low);
          } else {
            DASAvailable = True;
            digitalWrite(LED4, High);
          }
        }
        playerTotal = (playerCard1 + playerCard2); // Calculate the total of the player's 2 cards

        // Check the correct array for handtime and get action
        if !hitSoft17 {
          strcat(table, "noHit");
        }
        if !doubleAvailable {
          strcat(table, "noDouble");
        }
        if !DASAvailable {
          strcat(table, "noCAT");
        }
        if !surrenderAvailable {
          strcat(table, "noSur");
        }
        if (playerCard1 == playerCard2) {
          strcat(table, "Pair");
          action = table[(playerTotal / 2) - 1][dealerCard - 1];
        } else if ((playerCard1 == 1) || (playerCard2 == 1)) {
          strcat(table, "Soft");
          action = table[playerTotal - 3][dealerCard - 1];
        } else {
          strcat(table, "Hard");
          action = table[playerTotal - 8][dealerCard - 1];
        }

        // Turn off LEDs before showing action
        digitalWrite(LED1, Low);
        digitalWrite(LED2, Low);
        digitalWrite(LED3, Low);
        digitalWrite(LED4, Low);

        // Light up corrosponding LED to action
        if (action == 0) {
          digitalWrite(LED1, High);
        } else if (action == 1) {
          digitalWrite(LED2, High);
        } else if (action == 2) {
          digitalWrite(LED3, High);
        } else if (action == 3) {
          digitalWrite(LED4, High);
        } else {
          digitalWrite(LED1, High);
          digitalWrite(LED2, High);
          digitalWrite(LED3, High);
          digitalWrite(LED4, High);
        }

        while (digitalRead(BUT4) == LOW){
          // Wait until button is pressed again to continue
        }

        // Resetting all values
        playerCard1 = 0;
        playerCard2 = 0;
        dealerCard = 0;
        playerTotal = 0;
        action = 0;
        table[0] = "/0";

        // Reset LEDs
        digitalWrite(LED1, Low);
        digitalWrite(LED2, Low);
        digitalWrite(LED3, Low);
        digitalWrite(LED4, Low);

        // Set correct indicators for game rules
        if surrenderAvailable {
          digitalWrite(LED1, High);
        }
        if hitSoft17 {
          digitalWrite(LED2, High);
        }
        if doubleAvailable {
          digitalWrite(LED3, High);
        }
        if DASAvailable {
          digitalWrite(LED4, High);
        }
      }
    }
    lastBut4State = BUT4Reading; // Save state of button to check if it's changed
  }
}
