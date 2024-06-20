#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

unsigned long long PlayTime = 0;

bool ShouldCount = true;
bool Read = true;

const TCHAR* process = TEXT("HYP.exe");
const std::string FilePath = "PlayTime.txt";

void ReadFile() {
    if (!Read)
        return;

    std::ifstream file(FilePath);
    if (file.is_open()) {
        file >> PlayTime;
        file.close();
        std::cout << "Read PlayTime from file: " << PlayTime << '\n';
    }
    else {
        std::cout << "Unable to open file for reading: " << FilePath << '\n';
    }

    Read = false;
}

bool IsProcessRunning(const TCHAR* const executableName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        if (!_tcsicmp(entry.szExeFile, executableName)) {
            CloseHandle(snapshot);
            return true;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return false;
}

void WaitForProcessToStart(const TCHAR* const executableName) {
    std::cout << "Waiting for " << executableName << " to start...\n";
    while (!IsProcessRunning(executableName)) {
        Sleep(1000);
    }
    std::cout << executableName << " started.\n";
}

void Count() {
    ULONGLONG lastTick = GetTickCount64();

    while (IsProcessRunning(process)) {
        ULONGLONG now = GetTickCount64();
        ULONGLONG duration = now - lastTick;
        lastTick = now;

        PlayTime += duration / 1000;

        Sleep(1000);
        std::cout << PlayTime << '\n';
    }
}

void WriteFile() {
    std::ofstream file(FilePath);
    if (file.is_open()) {
        file << PlayTime;
        file.close();
        std::cout << "Wrote PlayTime to file: " << PlayTime << '\n';
    }
    else {
        std::cout << "Unable to open file for writing: " << FilePath << '\n';
    }
}

int main() {
    ReadFile();
    WaitForProcessToStart(process);
    Count();
    WriteFile();
    return 0;
}
