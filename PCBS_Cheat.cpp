#include <windows.h>
#include <psapi.h>
#include <iostream>

DWORD GetProcessIdByName(const wchar_t* processName){
    DWORD processIds[1024], bytesReturned;
    if(!EnumProcesses(processIds, sizeof(processIds), &bytesReturned)){
        std::cout << "Failed to enumerate processes." << std::endl;
        exit(-1);
    }

    DWORD processCount = bytesReturned / sizeof(DWORD);
    for(DWORD i = 0; i < processCount; i++){
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processIds[i]);
        if(hProcess){
            wchar_t name[MAX_PATH];
            DWORD nameLen = GetModuleBaseNameW(hProcess, NULL, name, MAX_PATH);
            if(nameLen > 0 && _wcsicmp(name, processName) == 0){
                CloseHandle(hProcess);
                return processIds[i];
            }
            CloseHandle(hProcess);
        }
    }
    return -69;
}

int wmain(int argc, wchar_t* argv[]){
    HANDLE pHandle;
    if(GetProcessIdByName(L"PCBS.exe") == -69) {
        std::cout << "Process PCBS.exe not found.";
        exit(-1);
    }

    if((pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(L"PCBS.exe"))) == INVALID_HANDLE_VALUE){
        std::wcout << L"Unable to open PCBS.exe." << std::endl;
        exit(-1);
    }

    int bytesRead;
    std::wcout << L"Process PCBS.exe was found." << std::endl;

    if(!ReadProcessMemory(pHandle, (void*)0x1ED3E1AFDC0, &bytesRead, 4, NULL)){
        std::cout << "Failed to read from process PCBS.exe at address 0x1ED3E1AFDC0." << std::endl;
    }

    bytesRead += 100000;

    if(!WriteProcessMemory(pHandle, (void*)0x1ED3E1AFDC0, &bytesRead, 4, NULL)){
        std::cout << "Couldnt write to process PCBS.exe at address 0x1ED3E1AFDC0" << std::endl;
    }

    CloseHandle(pHandle);
    return 0;
}