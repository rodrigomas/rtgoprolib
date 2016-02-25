#include "utils.h"
#include <string>


HINSTANCE Utils::hDllHandle = NULL;

void Utils::LoadTextResource(int recid, char** data, int *size)
{	
	HMODULE m_hInstance = GetModuleHandle((LPCSTR)DLL_MODULE_NAME);

	HRSRC hResource = FindResource(m_hInstance, MAKEINTRESOURCE(recid), "TEXT");

	if (*data == NULL) delete [] (*data);

	*size = 0;

	if (hResource)
	{
		HGLOBAL hLoadedResource = LoadResource(m_hInstance, hResource);

		if (hLoadedResource)
		{
			LPVOID pLockedResource = LockResource(hLoadedResource);

			if (pLockedResource)
			{
				DWORD dwResourceSize = SizeofResource(m_hInstance, hResource);

				if (0 != dwResourceSize)
				{
					*data = new char[dwResourceSize];

					memcpy(*data, pLockedResource, dwResourceSize);

					*size = dwResourceSize;				
				}
			}
		}
	}
}