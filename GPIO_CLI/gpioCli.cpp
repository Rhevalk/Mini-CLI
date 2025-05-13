
#include "gpioCli.h"

GPIOCLI::GPIOCLI(MCLI& cliRef) : cli(cliRef) {}

void GPIOCLI::begin(const uint8_t* protectPins, size_t protectCount,
                    const uint8_t* analogPins, size_t analogCount) {
    this->protectedPins = protectPins;
    this->protectedPinCount = protectCount;

    this->analogPins = analogPins;
    this->analogPinCount = analogCount;

    cli.addCmd("gset", cmd_Gse, "Gpio set value", "gset [pin(#list)] {on|off|0-255|input|inputpull}");
    cli.addCmd("grst", cmd_Grs, "Gpio reset", "grst {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*");
    cli.addCmd("gsts", cmd_Gst, "Gpio show status", "gsts {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*");
}

bool GPIOCLI::isProtectedPin(uint8_t pin) {
    for (size_t i = 0; i < protectedPinCount; i++) {
        if (protectedPins[i] == pin) return true;
    }
    return false;
}

bool GPIOCLI::isAnalogPin(uint8_t pin) {
    for (size_t i = 0; i < analogPinCount; i++) {
        if (analogPins[i] == pin) return true;
    }
    return false;
}

bool isInt(const char* str) {
  if (str == NULL) return false;

  if (*str == '-' || *str == '+') str++;
  if (*str == '\0') return false;

  while (*str) {
    if (!isdigit(*str)) return false;
    str++;
  }

  return true;
}

int getMode(const char* mode) {
  if (mode == NULL) return -1;

  if (isInt(mode) || (strncmp(mode, "on", 2) == 0 && strlen(mode) == 2) || (strncmp(mode, "off", 3) == 0 && strlen(mode) == 3) ) return OUTPUT;
  else if (strcmp(mode, "in") == 0) return INPUT;
  else if (strcmp(mode, "inpull") == 0) return INPUT_PULLUP;
  else return -1;
}

bool isValidPinList(char* str) {
  while (*str) {
    if (!isdigit(*str) && *str != ',') {
      return false;
    }
    str++;
  }
  return true;
}

void split(char* input, uint8_t* output, uint8_t& size, char splitter) {
  char* token;
  int index = 0;

  if (strchr(input, splitter) == NULL) {
    output[0] = atoi(input);  
    size = 1;
    return;
  }

  token = strtok(input, &splitter);

  while (token != NULL) {
    output[index++] = atoi(token);
    token = strtok(NULL, &splitter);
  }

  size = index;
}

/*===============GPIO SET===============*/
void GPIOCLI::cmd_Gse(uint8_t argc, char* argv[]) {
    if (argc < 3) {
      Serial.println(F("cmd: gset [pin] < [1|0] | in | inpull > "));
      return;
    }
    // mode
    int mode = getMode(argv[2]);


    uint8_t pins[30];
    uint8_t sizeOfPins = 0;

    split(argv[1], pins, sizeOfPins, ',');

    if (mode != -1) {

      if (mode == OUTPUT) {

        for (uint8_t i = 0; i < sizeOfPins; i++) {
          
          if (isProtectedPin(pins[i])) {
            Serial.print(F("Pin "));
            Serial.print(String(pins[i]));
            Serial.println(F(" protected and cannot be changed!"));
            return;
          }

          pinMode(pins[i], mode);

          if (isInt(argv[2])) {
            uint8_t value = atoi(argv[2]);
            analogWrite(pins[i], value);
            Serial.println("Write analog pin: " + String(pins[i]) +" -> "+ String(value));
          } 

          else {
            
            if (strncmp(argv[2], "on", 2) == 0 && strlen(argv[2]) == 2) {
              digitalWrite(pins[i], HIGH);
              Serial.println("Write digital pin: " + String(pins[i]) +" -> "+ String(argv[2]));
            }
            else if (strncmp(argv[2], "off", 3) == 0 && strlen(argv[2]) == 3) {
              digitalWrite(pins[i], LOW);
              Serial.println("Write digital pin: " + String(pins[i]) +" -> "+ String(argv[2]));
            }

          }
          
        }

        return;
      }  

      else if (mode == INPUT || mode == INPUT_PULLUP) {

        for (uint8_t i = 0; i < sizeOfPins; i++) {

          if (isProtectedPin(pins[i])) {
            Serial.print(F("Pin "));
            Serial.print(String(pins[i]));
            Serial.println(F(" protected and cannot be changed!"));
            return;
          }

          pinMode(pins[i], mode);

          if (isAnalogPin(pins[i])) {
            int valA = analogRead(pins[i]);
            Serial.println("Read analog pin: " + String(pins[i]) + " = " + String(valA));
          }

          else {
            int valD = digitalRead(pins[i]);
            Serial.println("Read pin: " + String(pins[i]) + " = " + String(valD));
          }

        }
      }

      else {
        Serial.println(F("Error value"));
        return;
      }

    } 
    
    else {
      Serial.println(F("Wrong mode"));
      return;
    }

}

/*===============GPIO RST===============*/
void GPIOCLI::cmd_Grs(uint8_t argc, char* argv[]) {

    if (argc < 2) {
      Serial.println("cmd: grst <[pin] | rn");
      return;
    }

    if (strcmp(argv[1], "r") == 0) {
      if (argc < 3) {
        Serial.println(F("cmd: grst rn <from pin> <to pin>"));
        return;
      }
      if ( isInt(argv[2]) && isInt(argv[3]) ) {

        Serial.print(F("rst pin"));

        for (uint8_t i = atoi(argv[2]); i < atoi(argv[3]) + 1; i++) {
          if (!isProtectedPin(i)) {
            pinMode(i, INPUT);
            digitalWrite(i, LOW);
            Serial.print(F("."));
          }
        }

        Serial.println(F("RST Pin done!"));
        return;
      }
    } 

    else if (isValidPinList(argv[1])) {

      uint8_t pins[30];
      uint8_t sizeOfPins = 0;

      split(argv[1], pins, sizeOfPins, ',');

      for (uint8_t i = 0; i < sizeOfPins; i++) {

      if (isProtectedPin(pins[i])) {
        Serial.print("Pin ");
        Serial.print(String(pins[i]));
        Serial.println(" diproteksi dan tidak bisa diubah!");
        return;
      }

      pinMode(pins[i], INPUT);
      digitalWrite(pins[i], LOW);
      Serial.println("RST Pin: " + String(pins[i]));

      }
    }

    else {
        Serial.println(F("Wrong arguments"));
    }
}

/*===============GPIO STS===============*/
void GPIOCLI::cmd_Gst(uint8_t argc, char* argv[]) {

    if (argc < 2) {
      Serial.println("cmd: gsts <[pin] | rn>");
      return;
    }

    if (strcmp(argv[1], "rn") == 0) {
        if (argc < 3) {
          Serial.println("cmd: gsts rn <from pin> <to pin>");
          return;
        }

      if ( isInt(argv[2]) && isInt(argv[3]) ) {

      for (int pin = atoi(argv[2]); pin < atoi(argv[3]) + 1; pin++) {
        bool isOutput = (sio_hw->gpio_oe & (1u << pin)) != 0;
        bool level = digitalRead(pin);
        Serial.printf("Pin %02d: %s, Level: %s\n",
                      pin,
                      isOutput ? "OUTPUT" : "INPUT",
                      level ? "HIGH" : "LOW");
        delayMicroseconds(100); 
      }

      return;
      }
      else {
        Serial.println(F("Arguments not int"));
      }
    } 
  
    else if (isValidPinList(argv[1])) {


      uint8_t pins[30];
      uint8_t sizeOfPins = 0;

      split(argv[1], pins, sizeOfPins, ',');

      for (uint8_t i = 0; i < sizeOfPins; i++) {
        bool isOutput = (sio_hw->gpio_oe & (1u << pins[i])) != 0;
        bool level = digitalRead(pins[i]);
        Serial.printf("Pin %02d: %s, Level: %s\n",
                      pins[i],
                      isOutput ? "OUTPUT" : "INPUT",
                      level ? "HIGH" : "LOW");
        delayMicroseconds(100); 
      }
    }

    else {
      Serial.println(F("Wrong arguments"));
    }
}