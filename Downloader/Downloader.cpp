#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
std::vector<double> generateRandomArray(int size) {
    std::vector<double> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % 1000 + 1;
    }
    return arr;
}
double calculateMean(const std::vector<double>& arr) {
    double sum = 0;
    for (double num : arr) {
        sum += num;
    }
    return sum / arr.size();
}
double calculateStandardDeviation(const std::vector<double>& arr) {
    double mean = calculateMean(arr);
    double sumSquaredDifferences = 0;
    for (double num : arr) {
        double diff = num - mean;
        sumSquaredDifferences += diff * diff;
    }
    double variance = sumSquaredDifferences / arr.size();
    return std::sqrt(variance);
}


void processFileTask(int fileNumber, HANDLE hMutex) {
    std::vector<double> data = generateRandomArray(200);
    double mean = calculateMean(data);
    double stdDev = calculateStandardDeviation(data);
    double minValue = *std::min_element(data.begin(), data.end());
    double maxValue = *std::max_element(data.begin(), data.end());
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "    - Standard Deviation: " << stdDev << "\n";
    ReleaseMutex(hMutex);

}

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned>(std::time(nullptr)) ^ GetCurrentProcessId());
    int fileNumber = 1;
    if (argc > 1) {
        fileNumber = std::stoi(argv[1]);
    }
    std::string fileName = "file_" + std::to_string(fileNumber) + ".dat";
    DWORD pid = GetCurrentProcessId();
    HANDLE hSemaphore = OpenSemaphore(
        SEMAPHORE_ALL_ACCESS,
        FALSE,
        TEXT("DownloadSlots")
    );
    HANDLE hMutex = OpenMutex(
        MUTEX_ALL_ACCESS,
        FALSE,
        TEXT("LogAccessMutex")
    );
    HANDLE hEvent = OpenEvent(
        EVENT_ALL_ACCESS,
        FALSE,
        TEXT("BrowserClosingEvent")
    );
    if (hSemaphore == NULL || hMutex == NULL || hEvent == NULL) {
        std::cout << "[PID: " << pid << "] Error opening synchronization objects!\n";
        std::cout << "Last error: " << GetLastError() << std::endl;
        return 1;
    }
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "[PID: " << pid << "] Waiting for download slot...\n";
    ReleaseMutex(hMutex);

    HANDLE waitHandles[2];
    waitHandles[0] = hSemaphore;
    waitHandles[1] = hEvent;
    DWORD waitResult = WaitForMultipleObjects(
        2,
        waitHandles,
        FALSE,
        INFINITE
    );
    if (waitResult == WAIT_OBJECT_0 + 1) {
        WaitForSingleObject(hMutex, INFINITE);
        std::cout << "[PID: " << pid << "] Download cancelled (browser is closing).\n";
        ReleaseMutex(hMutex);
        CloseHandle(hSemaphore);
        CloseHandle(hMutex);
        CloseHandle(hEvent);
        return 0;
    }
    else if (waitResult != WAIT_OBJECT_0) {
        std::cout << "[PID: " << pid << "] Wait error: " << GetLastError() << std::endl;
        CloseHandle(hSemaphore);
        CloseHandle(hMutex);
        CloseHandle(hEvent);
        return 1;
    }


    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "[PID: " << pid << "] Connection established. ";
    std::cout << "Starting download of '" << fileName << "'...\n";
    ReleaseMutex(hMutex);
    processFileTask(fileNumber, hMutex); 


    int sleepTime = (rand() % 2000) + 1000;  
    Sleep(sleepTime);



    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "[PID: " << pid << "] File '" << fileName << "' processed successfully.\n";
    ReleaseMutex(hMutex);
    if (!ReleaseSemaphore(hSemaphore, 1, NULL)) {
        std::cout << "[PID: " << pid << "] Error releasing semaphore: "
            << GetLastError() << std::endl;
    }
    CloseHandle(hSemaphore);
    CloseHandle(hMutex);
    CloseHandle(hEvent);

    return 0;
}