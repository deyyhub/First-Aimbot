#pragma once
#include "headers.h"

class Memory {
private:
    DWORD processId = 0;
    HANDLE hProcess = NULL;

public:

    HANDLE GetProcessHandle() { return hProcess; }

    Memory(const wchar_t* processName) {
        PROCESSENTRY32W entry; // Use the 'W' version
        entry.dwSize = sizeof(PROCESSENTRY32W);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        // Process32FirstW and Process32NextW for Unicode
        if (Process32FirstW(snapshot, &entry) == TRUE) {
            do {
                // Use _wcsicmp (Wide string compare)
                if (_wcsicmp(entry.szExeFile, processName) == 0) {
                    this->processId = entry.th32ProcessID;
                    this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->processId);
                    break;
                }
            } while (Process32NextW(snapshot, &entry) == TRUE);
        }
        CloseHandle(snapshot);
    }

    ~Memory() {
        if (hProcess) CloseHandle(hProcess);
    }

    // Get the base address of a specific module (e.g., "ac_client.exe")
    uintptr_t GetModuleBaseAddress(const wchar_t* moduleName) {
        uintptr_t dwModuleBaseAddress = 0;
        // Use TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32 to support both 32-bit and 64-bit processes
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->processId);

        if (hSnapshot != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W moduleEntry; // 'W' for Wide
            moduleEntry.dwSize = sizeof(MODULEENTRY32W);

            if (Module32FirstW(hSnapshot, &moduleEntry)) { // 'W' for Wide
                do {
                    // Use _wcsicmp (Wide String Compare) instead of _stricmp
                    if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
                        dwModuleBaseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                        break;
                    }
                } while (Module32NextW(hSnapshot, &moduleEntry));
            }
            CloseHandle(hSnapshot);
        }
        return dwModuleBaseAddress;
    }

    // Template for Reading Memory
    template <typename T>
    T Read(uintptr_t address) {
        T buffer;
        ReadProcessMemory(hProcess, (LPCVOID)address, &buffer, sizeof(T), NULL);
        return buffer;
    }

    // Template for Writing Memory
    template <typename T>
    void Write(uintptr_t address, T value) {
        WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(T), NULL);
    }
};