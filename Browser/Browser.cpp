#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int N, M;
    std::cout << "Enter N: ";
    std::cin >> N;
    std::cout << "Enter M: ";
    std::cin >> M;

    if (M <= N) {
        std::cout << "Error: M must be greater than N!\n";
        return 1;
    }

    HANDLE hSemaphore = CreateSemaphoreA(NULL, N, N, "DownloadSlots");
    HANDLE hMutex = CreateMutexA(NULL, FALSE, "LogAccessMutex");
    HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, "BrowserClosingEvent");

    std::cout << "\nSync objects created\n";

    std::vector<HANDLE> processes;

    std::cout << "Launching " << M << " downloader processes...\n";

    for (int i = 0; i < M; i++) {
        std::string cmd = "Downloader.exe " + std::to_string(i + 1);

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()),
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            processes.push_back(pi.hProcess);
            CloseHandle(pi.hThread);
            std::cout << "Started #" << (i + 1) << " PID: " << pi.dwProcessId << "\n";
        }
    }

    std::cout << "\nBrowser running. Press Enter to close...\n";
    std::cin.ignore();
    std::cin.get();

    std::cout << "\nClosing browser...\n";
    SetEvent(hEvent);

    for (HANDLE h : processes) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }

    CloseHandle(hSemaphore);
    CloseHandle(hMutex);
    CloseHandle(hEvent);

    std::cout << "Browser exiting.\n";
    return 0;
}