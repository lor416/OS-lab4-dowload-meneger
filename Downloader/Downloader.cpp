#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned>(std::time(nullptr)) ^ GetCurrentProcessId());

    int fileNumber = 1;
    if (argc > 1) {
        fileNumber = std::stoi(argv[1]);
    }

    DWORD pid = GetCurrentProcessId();

    HANDLE hSemaphore = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "DownloadSlots");
    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "LogAccessMutex");
    HANDLE hEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, "BrowserClosingEvent");

    std::cout << "[PID: " << pid << "] Waiting for slot...\n";

    HANDLE handles[2] = { hSemaphore, hEvent };
    DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

    if (result == WAIT_OBJECT_0 + 1) {
        std::cout << "[PID: " << pid << "] Cancelled\n";
        return 0;
    }

    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "[PID: " << pid << "] Starting download\n";
    ReleaseMutex(hMutex);

    Sleep((rand() % 2000) + 1000);

    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "[PID: " << pid << "] Finished\n";
    ReleaseMutex(hMutex);

    ReleaseSemaphore(hSemaphore, 1, NULL);

    CloseHandle(hSemaphore);
    CloseHandle(hMutex);
    CloseHandle(hEvent);

    return 0;
}