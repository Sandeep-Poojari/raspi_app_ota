#include <iostream>
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>g

#define GPIO_PIN 17

std::atomic<bool> running(true);

void signalHandler(int signum) {
    std::cout << "Received signal " << signum << ", exiting..." << std::endl;
    running = false;
}

void blink(int delayMs) {
    while (running) {
        gpioWrite(GPIO_PIN, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        gpioWrite(GPIO_PIN, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    gpioWrite(GPIO_PIN, 0);  // Ensure LED is off before exit
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed\n";
        return 1;
    }

    gpioSetMode(GPIO_PIN, PI_OUTPUT);
    std::string version = "2.0.1";

    std::cout << "Running version: " << version << std::endl;

    if (version == "1.0.0") {
        blink(1000);
    } else if (version == "2.0.0") {
        blink(500);
    } else {
        blink(250);  // Default or "2.0.1"
    }

    gpioTerminate();
    return 0;
}
