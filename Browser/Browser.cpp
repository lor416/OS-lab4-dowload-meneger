#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
int main() {
    int N, M;
    std::cout << "Enter N: ";
    std::cin >> N;
    std::cout << "Enter M: ";
    std::cin >> M;
    if (M <= N) {
        std::cout << "Error: M must be greater than N!\n";
        return 1;
    }
    std::vector<HANDLE> processes;
    for (int i = 0; i < M; i++) {
        std::string cmd = "Downloader.exe " + std::to_string(i + 1);
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()),
            NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            processes.push_back(pi.hProcess);
            CloseHandle(pi.hThread);
            std::cout << "process " << (i + 1) << "\n";
        }
    }
    std::cout << "\nenter \n";
    std::cin.ignore();
    std::cin.get();
    for (HANDLE h : processes) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }
    return 0;
}