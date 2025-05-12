#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>

const std::string VERSION_FILE = "/usr/myapps/appOta/version.txt";
const std::string REMOTE_URL = "http://192.168.0.195:8080/version.txt"; // Replace <mac_ip>
const std::string REMOTE_APP_URL = "http://192.168.0.195:8080/driveLed"; // Replace with your app binary URL
const std::string LOCAL_APP_PATH = "/usr/myapps/appOta/driveLed"; // Path where you want to save the app binary

std::string readLocalVersion() {
    std::ifstream file(VERSION_FILE);
    std::string version;
    if (file.is_open()) {
        std::getline(file, version);
        file.close();
    } else {
        version = "0.0.0";
    }
    return version;
}

std::string fetchRemoteVersion() {
    std::string tmpFile = "/tmp/remote_version.txt";
    std::string command = "curl -s --max-time 5 " + REMOTE_URL + " -o " + tmpFile;
    int result = std::system(command.c_str());
    if (result != 0) return ""; // curl failed

    std::ifstream file(tmpFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();

    // Basic sanity check: reject HTML responses (like 404 pages)
    if (content.find("<!DOCTYPE") != std::string::npos || content.find("<html") != std::string::npos) {
        return "";
    }

    // Trim newline
    content.erase(content.find_last_not_of(" \n\r\t") + 1);
    return content;
}

bool downloadApp() {
    // Stop the driveLed service before attempting to overwrite the binary
    std::cout << "Stopping driveLed service...\n";
    int stopResult = std::system("sudo systemctl stop driveLed.service");
    if (stopResult != 0) {
        std::cerr << "Failed to stop driveLed service. Aborting update.\n";
        return false;
    }

    // Proceed to download
    std::cout << "Downloading new driveLed binary...\n";
    std::string command = "curl -s --max-time 5 " + REMOTE_APP_URL + " -o " + LOCAL_APP_PATH;
    int result = std::system(command.c_str());
    std::cout << "Download Result: " << result << std::endl;

    // Restart the service regardless of success/failure
    std::cout << "Restarting driveLed service...\n";
    std::system("sudo systemctl start driveLed.service");

    // Return true only if download was successful
    return result == 0;
}


int main() {
    std::string localVersion = readLocalVersion();
    std::string remoteVersion = fetchRemoteVersion();

    std::cout << "Local Version: " << localVersion << std::endl;

    if (remoteVersion.empty()) {
        std::cerr << "Failed to fetch valid remote version." << std::endl;
        return 1;
    }

    std::cout << "Remote Version: " << remoteVersion << std::endl;

    if (remoteVersion != localVersion) {
        std::cout << "Update available. Proceeding with update..." << std::endl;
        
        if (downloadApp()) {
            std::cout << "App downloaded successfully. Replacing the old app..." << std::endl;
            // Update the version file to the new version
            std::ofstream outFile(VERSION_FILE);
            if (outFile.is_open()) {
                outFile << remoteVersion;
                outFile.close();
            } else {
                std::cerr << "Failed to write to version file." << std::endl;
            }
        } else {
            std::cerr << "Failed to download new app." << std::endl;
        }
    } else {
        std::cout << "No update required." << std::endl;
    }

    return 0;
}
