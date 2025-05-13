#pragma once
#include <Arduino.h>
#include <mCli.h>

class GPIOCLI {
public:
    GPIOCLI(MCLI& cliRef); 
    void begin(const uint8_t* protectPins, size_t protectCount,
               const uint8_t* analogPins, size_t analogCount);

private:
    MCLI& cli;

    const uint8_t* protectedPins = nullptr;
    size_t protectedPinCount = 0;

    const uint8_t* analogPins = nullptr;
    size_t analogPinCount = 0;

    bool isProtectedPin(uint8_t pin);
    bool isAnalogPin(uint8_t pin);

    static void cmd_Gse(uint8_t argc, char* argv[]);
    static void cmd_Grs(uint8_t argc, char* argv[]);
    static void cmd_Gst(uint8_t argc, char* argv[]);
};
