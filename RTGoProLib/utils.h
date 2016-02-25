#ifndef _UTILS_
#define _UTILS_

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define DLL_MODULE_NAME "RTGoProLib.dll"

class Utils
{	
public:
	static HINSTANCE hDllHandle;
	static void LoadTextResource(int recid, char** data, int *size);
};

#endif
