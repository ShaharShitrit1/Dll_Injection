// mydll.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#define DLL_EXPORT
#include "mydll.h"

HWND g_hWnd;

extern "C"
{
	DECLDIR void Share()
	{
		MessageBoxA(NULL, "HELLO THERE\nYou got DLL injected", "From Shahar's DLL", NULL);
	}
	void Keep()
	{
		printf("conected to the proc\n");
	}
}

BOOL APIENTRY DllMain(HANDLE hModule, // Handle to DLL module

	DWORD ul_reason_for_call,
	LPVOID lpReserved) // Reserved

{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// A process is loading the DLL.
		Share();
		break;
	case DLL_THREAD_ATTACH:
		// A process is creating a new thread.
		Share();
		break;
	case DLL_THREAD_DETACH:
		// A thread exits normally.
		break;
	case DLL_PROCESS_DETACH:
		// A process unloads the DLL.
		break;
	}
	return TRUE;
}

