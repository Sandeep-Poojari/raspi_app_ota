#include <iostream>
#include <fstream>
#include <string>
#include <pigpio.h>

const std::string VERSION_FILE = "/home/pi/LEDControlApp/.version";
const int LED_PIN = 17;  // Modify as per your setup

// Read version info from file
std::string getVersion() {
    std::ifstream file(VERSION_FILE);
    std::string version;
    if (file >> version) return version;
    return "0.0.0";  // Default version if not found
}

void setupLED() {
    if (gpioInitialise() < 0) {
        std::cerr << "GPIO Initialization Failed\n";
        exit(1);
    }
    gpioSetMode(LED_PIN, PI_OUTPUT);
}

void blinkLED() {
    while (true) {
        gpioWrite(LED_PIN, 1);  // Turn LED on
        time_sleep(1);
        gpioWrite(LED_PIN, 0);  // Turn LED off
        time_sleep(1);
    }
}

int main() {
    std::string version = getVersion();
    std::cout << "Current version: " << version << "\n";

    setupLED();
    blinkLED();

    return 0;
}
