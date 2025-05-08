// driveLed.cpp
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <pigpio.h>

const int LED_GPIO = 17;
const std::string VERSION_FILE = "/usr/myapps/appOta/.version";

int readVersion() {
    std::ifstream versionFile(VERSION_FILE);
    int version = 0;
    if (versionFile.is_open()) {
        versionFile >> version;
        versionFile.close();
    } else {
        std::cerr << "Failed to read version file\n";
    }
    return version;
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed\n";
        return 1;
    }

    gpioSetMode(LED_GPIO, PI_OUTPUT);

    int version = readVersion();
    std::cout << "Running LED app, version: " << version << std::endl;

    while (true) {
        gpioWrite(LED_GPIO, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * (version+1)));
        gpioWrite(LED_GPIO, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * (version+1)));
    }

    gpioTerminate();
    return 0;
}
