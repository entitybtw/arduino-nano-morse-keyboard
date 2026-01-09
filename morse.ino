unsigned long lastMillis = 0;
#include <unoHID.h>
#include <NewTone.h>
unsigned long pressStart = 0;
bool lastState = false;
String currentSymbol = "";
byte layoutMode = 0;
bool layoutSignaled = false;

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
  {".-", "f"},   {"-...", ","}, {".--", "d"},  {"--.", "u"},  {"-..", "l"},
  {".", "t"},    {"...-", ";"}, {"--..", "p"}, {"..", "b"},   {".---", "q"},
  {"-.-", "r"},  {".-..", "k"}, {"--", "v"},   {"-.", "y"},   {"---", "j"},
  {".--.", "g"}, {".-.", "h"},  {"...", "c"},  {"-", "n"},    {"..-", "e"},
  {"..-.", "a"}, {"....", "["}, {"-.-.", "w"}, {"---.", "x"}, {"----", "i"},
  {"--.-", "o"}, {"--.--", "s"}, {"-.--", "m"}, {".-.-", "'"}, {"..-..", "."},
  {".-.-.", "z"}
};

MorseLetter morseMORSE[] = {
  {".-", ".-"},   {"-...", "-..."}, {"-.-.", "-.-."}, {"-..", "-.."},  {".", "."},
  {"..-.", "..-."}, {"--.", "--."},  {"....", "...."}, {"..", ".."},   {".---", ".---"},
  {"-.-", "-.-"},  {".-..", ".-.."}, {"--", "--"},   {"-.", "-."},   {"---", "---"},
  {".--.", ".--."}, {"--.-", "--.-"}, {".-.", ".-."},  {"...", "..."},  {"-", "-"},
  {"..-", "..-"},  {"...-", "...-"}, {".--", ".--"},  {"-..-", "-..-"}, {"-.--", "-.--"},
  {"--..", "--.."}
};

const char* decodeMorse(String s, byte mode) {
  if(mode == 0) {
    MorseLetter* table = morseEN;
    int len = sizeof(morseEN) / sizeof(MorseLetter);
    for (int i = 0; i < len; i++) {
      if (s == table[i].code) return table[i].letter;
    }
  } else if(mode == 1) {
    MorseLetter* table = morseRU;
    int len = sizeof(morseRU) / sizeof(MorseLetter);
    for (int i = 0; i < len; i++) {
      if (s == table[i].code) return table[i].letter;
    }
  } else if(mode == 2) {
    MorseLetter* table = morseMORSE;
    int len = sizeof(morseMORSE) / sizeof(MorseLetter);
    for (int i = 0; i < len; i++) {
      if (s == table[i].code) return table[i].letter;
    }
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
        layoutMode++;
        if(layoutMode > 2) layoutMode = 0;
        
        if(layoutMode == 0 || layoutMode == 1) {
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.releaseAll();
        } else {
          Keyboard.press(KEY_LEFT_GUI);
          Keyboard.press(' ');
          delay(100);
          Keyboard.releaseAll();
        }
        
        NewTone(8, 1000);
        delay(50);
        noNewTone(8);
        layoutSignaled = true;
      } else if (pressDuration >= 200) {
        currentSymbol += "-";
        lastMillis = now;
      } else if (pressDuration >= 80) {
        currentSymbol += ".";
        lastMillis = now;
      }
    }

    if (currentSymbol.length() && now - lastMillis > 600) {
      Keyboard.print(decodeMorse(currentSymbol, layoutMode));
      currentSymbol = "";
    }
  }

  lastState = isPressed;
}