#include "mCli.h"

MCLI::MCLI(uint16_t sizeBuffer, uint16_t maxCommands) {
    _sizeBuffer = sizeBuffer;
    _maxCommands = maxCommands;
    commands = new CLICommand[maxCommands];
    commandCount = 0;
}

MCLI::~MCLI() {
    delete[] commands;
}

void MCLI::Input() {
    static char buffer[_sizeBuffer];           
    static size_t index = 0;
    static unsigned long lastCheck = 0;

    if (millis() - lastCheck >= 10) {
        lastCheck = millis();

        while (Serial.available()) {
            char c = Serial.read();
            if (c == '\r' || c == '\n') {
                buffer[index] = '\0';
                if (index > 0) {

                    if (strncmp(buffer, "help", 4) == 0) {
                        char* arg = strchr(buffer, ' ');
                        if (arg) {
                            arg++; 
                            Help(arg);
                        } else {
                            Help();
                        }
                    } else {
                        execute(buffer);
                    }

                    index = 0;
                }
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = c;
            }
        }
    }
}

void MCLI::addCmd(const char* name, CommandFunction func, const char* desc, const char* syntax) {
    if (commandCount < _maxCommands) {
        commands[commandCount++] = { name, func, desc, syntax };
    }
}

void MCLI::execute(const char* input) {
    char inputCopy[_sizeBuffer];
    strncpy(inputCopy, input, sizeof(inputCopy));
    inputCopy[sizeof(inputCopy) - 1] = '\0';

    char* argv[10];
    int argc = 0;

    char* token = strtok(inputCopy, " ");
    while (token && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0) return;

    for (int i = 0; i < commandCount; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }

    Serial.print(F("Command not found: "));
    Serial.println(argv[0]);
    Serial.println(F("Send 'help' to show available commands."));
}

void MCLI::Help() {
    Serial.println(F("=== List of CLI Commands ==="));
    for (int i = 0; i < commandCount; i++) {
        Serial.print("- ");
        Serial.print(commands[i].name);
        if (commands[i].description) {
            Serial.print(": ");
            Serial.println(commands[i].description);
        } else {
            Serial.println("");
        }
        if (commands[i].syntax) {
            Serial.print(F("  Usage: "));
            Serial.println(commands[i].syntax);
        }
    }
}

void MCLI::Help(const char* command) {
    for (int i = 0; i < commandCount; i++) {
        if (strcmp(command, commands[i].name) == 0) {
            Serial.print(F("Command: "));
            Serial.println(commands[i].name);
            if (commands[i].description) {
                Serial.print(F("Description: "));
                Serial.println(commands[i].description);
            }
            if (commands[i].syntax) {
                Serial.print(F("Usage: "));
                Serial.println(commands[i].syntax);
            }
            return;
        }
    }
    Serial.print(F("Command '"));
    Serial.print(command);
    Serial.println(F("' not found."));
}

