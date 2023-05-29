#include "pch.h"

DWORD findProcessId(const char* processName);
bool injectDll(DWORD processId, const char* dllPath);

int main()
{
    const char* processName = "notepad.exe";
    const char* dllPath = "C:\\Users\\test0\\OneDrive\\שולחן העבודה\\Magshimim\\שנה ב\\OperatingSystems\\lastProject\\mydll\\x64\\Debug\\mydll.dll";
    //HI Eliran please remember to change dllPath to your path to my dll.
    //and I also wanted to say thank you for this semester i really liked your course
    //it was fun and i learned so much so i have nothing else to say but thank you.

    // Find the process ID of the target process
    DWORD processId = findProcessId(processName);
    if (processId == 0) 
    {
        std::cout << "Could not find process: " << processName << std::endl;
        return 1;
    }

    // Inject the DLL into the target process
    if (!injectDll(processId, dllPath)) 
    {
        std::cout << "Failed to inject DLL." << std::endl;
        return 1;
    }

    std::cout << "DLL injected successfully." << std::endl;
    return 0;
}

DWORD findProcessId(const char* processName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 processEntry = { 0 };
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    DWORD processId = 0;

    if (Process32First(snapshot, &processEntry)) 
    {
        do 
        {
            bstr_t b(processEntry.szExeFile);
            const char* c = b;
            if (_stricmp(c, processName) == 0) 
            {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return processId;
}

bool injectDll(DWORD processId, const char* dllPath)
{
    // Open the target process
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (process == NULL) 
    {
        return false;
    }

    // Allocate memory in the target process for the DLL path
    LPVOID dllPathAddress = VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
    if (dllPathAddress == NULL) 
    {
        CloseHandle(process);
        return false;
    }

    // Write the DLL path into the target process
    if (!WriteProcessMemory(process, dllPathAddress, dllPath, strlen(dllPath) + 1, NULL)) 
    {
        VirtualFreeEx(process, dllPathAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    // Get the address of the LoadLibrary function in the target process
    LPVOID loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (loadLibraryAddress == NULL) 
    {
        VirtualFreeEx(process, dllPathAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    // Create a remote thread in the target process to load the DLL
    HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllPathAddress, 0, NULL);
    if (thread == NULL)
    {
        VirtualFreeEx(process, dllPathAddress, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    // Wait for the remote thread to finish
    WaitForSingleObject(thread, INFINITE);

    // Clean up
    CloseHandle(thread);
    VirtualFreeEx(process, dllPathAddress, 0, MEM_RELEASE);
    CloseHandle(process);

    return true;
}