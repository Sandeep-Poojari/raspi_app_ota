#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <curl/curl.h>

const std::string LOCAL_VERSION_FILE = "/home/sandeep/LEDControlApp/.version";
const std::string REMOTE_VERSION_URL = "http://192.168.0.195:8080/version.txt";
const std::string REMOTE_BINARY_URL_PREFIX = "http://192.168.0.195:8080/driveLed_";
const std::string BINARY_PATH = "/home/pi/LEDControlApp/driveLed";

// Helper for CURL response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total = size * nmemb;
    output->append((char*)contents, total);
    return total;
}

std::string getRemoteVersion() {
    CURL* curl = curl_easy_init();
    std::string version;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, REMOTE_VERSION_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &version);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) return "";
    }
    return version;
}

std::string getLocalVersion() {
    std::ifstream file(LOCAL_VERSION_FILE);
    std::string version;
    if (file >> version) return version;
    return "0.0.0";  // Default version if not found
}

bool downloadBinary(const std::string& version) {
    std::string url = REMOTE_BINARY_URL_PREFIX + version;
    std::string command = "curl -o " + BINARY_PATH + " " + url;
    return system(command.c_str()) == 0;
}

bool updateVersionFile(const std::string& version) {
    std::ofstream out(LOCAL_VERSION_FILE);
    if (!out) return false;
    out << version;
    return true;
}

int main() {
    std::string localVersion = getLocalVersion();
    std::string remoteVersion = getRemoteVersion();

    if (remoteVersion.empty()) {
        std::cerr << "Failed to fetch remote version.\n";
        return 1;
    }

    std::cout << "Local Version: " << localVersion << "\n";
    std::cout << "Remote Version: " << remoteVersion << "\n";

    if (remoteVersion != localVersion) {
        std::cout << "New version available: " << remoteVersion << "\n";
        if (downloadBinary(remoteVersion)) {
            system(("chmod +x " + BINARY_PATH).c_str());
            updateVersionFile(remoteVersion);
            std::cout << "Update successful.\n";
        } else {
            std::cerr << "Failed to download binary.\n";
            return 1;
        }
    } else {
        std::cout << "Already up to date (version " << localVersion << ")\n";
    }

    return 0;
}
