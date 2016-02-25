#ifdef _WIN32
#include <Windows.h>
#include <Wingdi.h>
#endif

#include "utils.h"

BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle,  IN DWORD     nReason,  IN LPVOID    Reserved )
{
	BOOLEAN bSuccess = TRUE;

	Utils::hDllHandle = hDllHandle;

	switch ( nReason )
	{
		case DLL_PROCESS_ATTACH:			
		break;

		case DLL_PROCESS_DETACH:	
		break;
	}

	return bSuccess;
}