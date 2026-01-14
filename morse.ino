unsigned long lastMillis = 0;
#include <unoHID.h>
#include <NewTone.h>
unsigned long pressStart = 0;
bool lastState = false;
String currentSymbol = "";
bool isRussian = false;
bool layoutSignaled = false;

enum Layout { EN, RU, MORSE };
Layout currentLayout = EN;

struct MorseLetter {
  const char* code;
  const char* letter;
};

MorseLetter morseEN[] = {
  {".-", "a"},   {"-...", "b"}, {"-.-.", "c"}, {"-..", "d"},  {".", "e"},
  {"..-.", "f"}, {"--.", "g"},  {"....", "h"}, {"..", "i"},   {".---", "j"},
  {"-.-", "k"},  {".-..", "l"}, {"--", "m"},   {"-.", "n"},   {"---", "o"},
  {".--.", "p"}, {"--.-", "q"}, {".-.", "r"},  {"...", "s"},  {"-", "t"},
  {"..-", "u"},  {"...-", "v"}, {".--", "w"},  {"-..-", "x"}, {"-.--", "y"},
  {"--..", "z"}
};

MorseLetter morseRU[] = {
  {".-", "f"}, {"-...", ","}, {".--", "d"}, {"--.", "u"}, {"--..", "p"}, {"-..", "l"}, {".", "t"},  
  {"..", "b"}, {".---", "q"}, {"-.-", "r"}, {".-..", "k"}, {"...-", ";"},
  {"--", "v"}, {"-.", "y"}, {"---", "j"}, {".--.", "g"}, {".-.", "h"}, {"...", "c"}, {"-", "n"}, {"..-", "e"}, {"..-.", "a"},  
  {"....", "["}, {"-.-.", "w"}, {"---.", "x"}, {"----", "i"}, {"--.-", "o"}, {".--.-.", "]"},  
  {"-.--", "s"}, {"-..-", "m"}, {"...-...", "'"}, {"..--", "."}, {".-.-", "z"}
};

MorseLetter morseMORSE[] = {
  {".", "/"},
  {"-", "-"}
};

const char* decodeMorse(String s) {
  MorseLetter* table;
  int len;

  if (currentLayout == RU) {
    table = morseRU;
    len = sizeof(morseRU) / sizeof(MorseLetter);
  } else if (currentLayout == MORSE) {
    table = morseMORSE;
    len = sizeof(morseMORSE) / sizeof(MorseLetter);
  } else {
    table = morseEN;
    len = sizeof(morseEN) / sizeof(MorseLetter);
  }

  for (int i = 0; i < len; i++) {
    if (s == table[i].code) return table[i].letter;
  }
  return "?";
}

void setup() {
  Keyboard.begin();
  pinMode(3, INPUT_PULLUP);
}

void loop() {
  bool isPressed = !digitalRead(3);
  unsigned long now = millis();

  if (isPressed) {
    NewTone(8, 2000);
    if (!lastState) pressStart = now;
    layoutSignaled = false;
  } else {
    noNewTone(8);
    if (lastState) {
      unsigned long pressDuration = now - pressStart;

      if (!layoutSignaled && pressDuration >= 600) {
        if (currentLayout == EN) currentLayout = RU;
        else if (currentLayout == RU) currentLayout = MORSE;
        else currentLayout = EN;

        if (currentLayout != MORSE) {
        //  note: надо чтобы предварительно на устройстве стояла en раскладка
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.releaseAll();
        }

        noNewTone(8);
        NewTone(8, currentLayout == MORSE ? 1500 : 1000);
        delay(80);
        noNewTone(8);

        layoutSignaled = true;
      }
      else if (pressDuration >= 200) {
        currentSymbol += "-";
        lastMillis = now;
      }
      else if (pressDuration >= 80) {
        currentSymbol += ".";
        lastMillis = now;
      }
    }

    if (currentSymbol.length() && now - lastMillis > 600) {
      Keyboard.print(decodeMorse(currentSymbol));
      currentSymbol = "";
    }
  }

  lastState = isPressed;
}
