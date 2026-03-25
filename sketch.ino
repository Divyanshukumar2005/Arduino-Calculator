#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

String current = "";
String memory = "";
char operation = 0;
bool freshResult = false;
String history[3] = {"", "", ""};
int historyCount = 0;

void addHistory(String entry) {
  history[2] = history[1];
  history[1] = history[0];
  history[0] = entry;
  if (historyCount < 3) historyCount++;
}

void showSplashScreen() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Divyanshu's");
  lcd.setCursor(3, 1);
  String msg = "Calculator";
  for (byte i = 0; i < msg.length(); i++) {
    lcd.print(msg[i]);
    delay(80);
  }
  delay(1000);
  lcd.clear();
}

void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String expr = memory;
  if (operation) expr += operation;
  expr += current;
  if (expr.length() == 0) expr = "0";
  if (expr.length() > 16) {
    expr = expr.substring(expr.length() - 16);
  }
  lcd.print(expr);

  // Line 2
  lcd.setCursor(0, 1);
  if (current.length() == 0 && memory.length() == 0 && !operation) {
    lcd.print("     Ready...   ");
  } else {
    lcd.print("                ");
  }
}

double calculate(char op, double left, double right) {
  switch (op) {
    case '+': return left + right;
    case '-': return left - right;
    case '*': return left * right;
    case '/':
      if (right == 0) return NAN;
      return left / right;
  }
  return 0;
}

void processInput(char key) {

  // Clear All
  if (key == 'C') {
    current = "";
    memory = "";
    operation = 0;
    freshResult = false;
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("  Cleared!  ");
    delay(600);
    updateDisplay();
    return;
  }

  // Equals
  if (key == '=') {
    if (operation && memory.length() > 0 && current.length() > 0) {
      double left = memory.toDouble();
      double right = current.toDouble();
      double result = calculate(operation, left, right);

      lcd.clear();

      if (isnan(result)) {
        lcd.setCursor(0, 0);
        lcd.print(" Error: Div/0!");
        lcd.setCursor(0, 1);
        lcd.print(" Press C Clear ");
        delay(1500);
        current = "";
        memory = "";
        operation = 0;
        updateDisplay();
      } else if (abs(result) > 999999999.0) {
        lcd.setCursor(0, 0);
        lcd.print("Overflow Error!");
        lcd.setCursor(0, 1);
        lcd.print(" Press C Clear ");
        delay(1500);
        current = "";
        memory = "";
        operation = 0;
        updateDisplay();
      } else {
        String resultStr;
        if (result == (long)result) {
          resultStr = String((long)result);
        } else {
          resultStr = String(result, 3);
        }

        String entry = memory + String(operation) + current + "=" + resultStr;
        addHistory(entry);

        String expr = memory + String(operation) + current;
        if (expr.length() > 16) expr = expr.substring(expr.length() - 16);
        lcd.setCursor(0, 0);
        lcd.print(expr);
        lcd.setCursor(0, 1);
        lcd.print("= ");
        lcd.print(resultStr);

        memory = resultStr;
        current = "";
        operation = 0;
        freshResult = true;
      }
    }
    return;
  }

  // Operators
  if (key == '+' || key == '-' || key == '*' || key == '/') {
    if (current.length() > 0 || memory.length() > 0) {
      if (current.length() > 0) memory = current;
      current = "";
      operation = key;
      freshResult = false;
      updateDisplay();
    }
    return;
  }

  // Numbers
  if (freshResult) {
    current = "";
    memory = "";
    operation = 0;
    freshResult = false;
  }

  if (current.length() >= 9) return;
  current += String(key);
  updateDisplay();
}

void updateCursor() {
  if (millis() / 400 % 2 == 0) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  showSplashScreen();
  updateDisplay();
}

void loop() {
  updateCursor();
  char key = keypad.getKey();
  if (key) {
    processInput(key);
  }
}
