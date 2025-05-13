#pragma once
#include <Arduino.h>

typedef void (*CommandFunction)(uint8_t argc, char* argv[]);
struct CLICommand {
    const char* name;
    CommandFunction func;
    const char* description;
    const char* syntax; 
};

class MCLI {
public:
    MCLI(uint16_t sizeBuffer, uint16_t maxCommands);
    ~MCLI();

    void Input(void* parameter);
    void execute(const char* input);
    void addCmd(const char* name, CommandFunction func, const char* desc, const char* syntax);
    void Help(); 
    void Help(const char* command); 

private:
    uint16_t _sizeBuffer;
    uint16_t _maxCommands;
    CLICommand* commands;
    uint16_t commandCount;
};
