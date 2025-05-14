#include <mCli.h> 

MCLI cli(128, 20); // sizeBuffer, Max commands

void echo_cmd(uint8_t argc, char* argv[]) {
  String text = "";
  for (uint8_t i = 1; i < argc; i++) {
    text += String(argv[i]);
    if (i < argc - 1) text += " ";
  }
  Serial.println(text);
}

void setup() {
  Serial.begin(115200);
  addCmd("echo", echo_cmd, "Teks print to display", "echo <text>") // call command, name of function, description, syntax
}

void loop() {
    cli.input();
}
