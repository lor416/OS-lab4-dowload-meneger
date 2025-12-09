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
    std::cout << "Enter M ";
    std::cin >> M;

    if (M <= N) {
        std::cout << "Error: M must be greater than N\n";
        return 1;
    }


    HANDLE hSemaphore = CreateSemaphore(
        NULL,
        N,
        N,
        TEXT("DownloadSlots")
    );
    if (hSemaphore == NULL) {
        std::cout << "Error creating semaphore: " << GetLastError() << std::endl;
        return 1;
    }



    HANDLE hMutex = CreateMutex(
        NULL,
        FALSE,
        TEXT("LogAccessMutex")
    );
    if (hMutex == NULL) {
        std::cout << "Error creating mutex: " << GetLastError() << std::endl;
        CloseHandle(hSemaphore);
        return 1;
    }


    HANDLE hEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        TEXT("BrowserClosingEvent")
    );
    if (hEvent == NULL) {
        std::cout << "Error creating event: " << GetLastError() << std::endl;
        CloseHandle(hSemaphore);
        CloseHandle(hMutex);
        return 1;
    }
    std::cout << std::endl;



    std::vector<HANDLE> processHandles;
    std::vector<PROCESS_INFORMATION> processInfos;
    std::cout << "Launching " << M << " downloader processes...\n";
    for (int i = 0; i < M; i++) {
        std::string commandLine = "Downloader.exe " + std::to_string(i + 1);
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        std::vector<char> cmdLine(commandLine.begin(), commandLine.end());
        cmdLine.push_back('\0');
        if (CreateProcessA(
            NULL,
            cmdLine.data(),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            processHandles.push_back(pi.hProcess);
            processInfos.push_back(pi);
        }
        else {
            std::cout << "Failed to start process #" << (i + 1)
                << ", error: " << GetLastError() << std::endl;
        }
    }
    std::cout << "Browser is running. Press Enter to close...\n";
    std::cin.ignore();
    std::cin.get();

    std::cout << "\nBrowser is closing. Sending termination signal to all downloads...\n";


    SetEvent(hEvent);

    if (!processHandles.empty()) {
        std::cout << "Wait finishing \n\n";
        WaitForMultipleObjects(
            static_cast<DWORD>(processHandles.size()),
            processHandles.data(),
            TRUE,
            INFINITE
        );
    }



    for (size_t i = 0; i < processInfos.size(); i++) {
        CloseHandle(processInfos[i].hProcess);
        CloseHandle(processInfos[i].hThread);
    }
    CloseHandle(hSemaphore);
    CloseHandle(hMutex);
    CloseHandle(hEvent);
    std::cout << "\n\nAll processes completed. Browser exiting.\n";
    system("pause");
    return 0;
}