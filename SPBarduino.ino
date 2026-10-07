#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── PINS ─────────────────────────────────────────
const int LED_MATIN  = 9;
const int LED_MIDI   = 10;
const int LED_SOIR   = 11;
const int REED_PIN   = 2;
const int BUZZER_PIN = 8;

// ── SCHEDULE ─────────────────────────────────────
const int SCHEDULE[3][2] = {
  {9,21},
  {22,04},
  {9,03}
};
const int LED_PINS[3]     = {LED_MATIN, LED_MIDI, LED_SOIR};
const char* SLOT_NAMES[3] = {"Matin", "Midi ", "Soir "};

// ── ETAT ─────────────────────────────────────────
bool ledActive[3]          = {false, false, false};
bool pillTaken[3]          = {false, false, false};
bool alertSent[3]          = {false, false, false};
unsigned long ledOnTime[3] = {0, 0, 0};
const unsigned long ALERT_DELAY =30000UL;

// ── HEURE ET DATE ────────────────────────────────
int currentH     = 0;
int currentM     = 0;
int currentS     = 0;
int currentDay   = 1;
int currentMonth = 1;
int currentYear  = 2026;
char currentDayName[4] = "Lun";
unsigned long lastSyncMillis = 0;
bool timeReceived = false;

// ─────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Pill Box");
  lcd.setCursor(0, 1);
  lcd.print("Attente heure...");

  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }
  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("NEED_TIME");
}

// ── Recevoir heure et date depuis PC ─────────────
void receiveTime() {
  String data = Serial.readStringUntil('\n');
  data.trim();
  if (data.length() >= 17) {
    currentH     = data.substring(0, 2).toInt();
    currentM     = data.substring(3, 5).toInt();
    currentS     = data.substring(6, 8).toInt();
    currentDay   = data.substring(9, 11).toInt();
    currentMonth = data.substring(12, 14).toInt();
    currentYear  = data.substring(15, 19).toInt();
    String dayName = data.substring(20, 23);
    dayName.toCharArray(currentDayName, 4);
    lastSyncMillis = millis();
    timeReceived   = true;
    Serial.println("TIME_OK");
  }
}

// ── Mettre a jour heure ───────────────────────────
void updateTime() {
  if (!timeReceived) return;

  unsigned long now     = millis();
  unsigned long elapsed = (now - lastSyncMillis) / 1000UL;

  if (elapsed > 0) {
    lastSyncMillis += elapsed * 1000UL;
    currentS += elapsed;
    if (currentS >= 60) {
      currentM += currentS / 60;
      currentS  = currentS % 60;
    }
    if (currentM >= 60) {
      currentH += currentM / 60;
      currentM  = currentM % 60;
    }
    if (currentH >= 24) {
      currentH = 0;
    }
  }
}

// ── Buzzer ────────────────────────────────────────
void buzzReminder() {
  unsigned long start = millis();
  while (millis() - start < 10000) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}

void buzzAlert() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(800);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(800);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzConfirm() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}

// ── LCD ───────────────────────────────────────────
void updateLCD() {
  // Ligne 0 : date
  lcd.setCursor(0, 0);
  if (currentDay < 10) lcd.print("0");
  lcd.print(currentDay);
  lcd.print("/");
  if (currentMonth < 10) lcd.print("0");
  lcd.print(currentMonth);
  lcd.print("/");
  lcd.print(currentYear);
  lcd.print(" ");
  lcd.print(currentDayName);
  lcd.print(" ");

  // Ligne 1 : heure + statut
  lcd.setCursor(0, 1);
  if (currentH < 10) lcd.print("0");
  lcd.print(currentH);
  lcd.print(":");
  if (currentM < 10) lcd.print("0");
  lcd.print(currentM);
  lcd.print(":");
  if (currentS < 10) lcd.print("0");
  lcd.print(currentS);

  bool anyActive = false;
  for (int i = 0; i < 3; i++) {
    if (ledActive[i] && !pillTaken[i]) {
      lcd.print(" ");
      lcd.print(SLOT_NAMES[i]);
      lcd.print("!");
      anyActive = true;
      break;
    }
  }
  if (!anyActive) lcd.print("  OK  ");
}

// ── LOOP ─────────────────────────────────────────
void loop() {
  if (Serial.available() > 0) {
    receiveTime();
  }

  updateTime();

  if (!timeReceived) {
    delay(500);
    Serial.println("NEED_TIME");
    return;
  }

  // 1. Verifier planning
  for (int i = 0; i < 3; i++) {
    if (currentH == SCHEDULE[i][0] &&
        currentM == SCHEDULE[i][1] &&
        currentS == 0) {
      if (!ledActive[i] && !pillTaken[i]) {
        ledActive[i] = true;
        ledOnTime[i] = millis();
        alertSent[i] = false;
        digitalWrite(LED_PINS[i], HIGH);
        Serial.print("[");
        Serial.print(SLOT_NAMES[i]);
        Serial.println("] PRENDRE LE MEDICAMENT!");
        buzzReminder();
      }
    }
  }

  // 2. Detecter ouverture porte (reed switch)
  // 2. Detecter ouverture porte (reed switch)
  if (digitalRead(REED_PIN) == HIGH) {
    for (int i = 0; i < 3; i++) {
      if (ledActive[i] && !pillTaken[i]) {
        pillTaken[i] = true;
        ledActive[i] = false;
        digitalWrite(LED_PINS[i], LOW);
        Serial.print("[");
        Serial.print(SLOT_NAMES[i]);
        Serial.println("] Medicament pris!");
        buzzConfirm();
        delay(300);
      }
    }
  }

  // 3. Alerte 30 min
  for (int i = 0; i < 3; i++) {
    if (ledActive[i] && !pillTaken[i] && !alertSent[i]) {
      if (millis() - ledOnTime[i] >= ALERT_DELAY) {
        alertSent[i] = true;
        Serial.print("ALERTE:");
        Serial.print(SLOT_NAMES[i]);
        Serial.println(":NON_PRIS");
        buzzAlert();
      }
    }
  }

  // 4. Reset minuit
  if (currentH == 0 && currentM == 0 && currentS == 0) {
    for (int i = 0; i < 3; i++) {
      pillTaken[i] = false;
      alertSent[i] = false;
      ledActive[i] = false;
      digitalWrite(LED_PINS[i], LOW);
    }
  }

  updateLCD();
  delay(200);
}