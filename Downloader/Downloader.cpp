#include <windows.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    int fileNumber = 1;
    if (argc > 1) {
        fileNumber = std::stoi(argv[1]);
    }
    DWORD pid = GetCurrentProcessId();
    std::cout << "[PID: " << pid << "] Downloader started for file #" << fileNumber << "\n";
    Sleep(2000);
    std::cout << "[PID: " << pid << "] Downloader finished\n";
    return 0;
}