#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifdef _WIN32
#include <Windows.h>
#include <Wingdi.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <sys/timeb.h>
#include <ctime>
#include <cmath>
#include <math.h>
#include <string>

#include <list>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <pthread.h>
#include <semaphore.h>

#include "rendertarget.h"
#include "effect.h"
#include "utils.h"
#include "resource.h"
#include "rtgopro.h"

#define MAXF(a,b) ((a > b) ?( a ) : b)

struct TexturePositionVertex
{
	float u;
	float v;
	float x;
	float y;
	float z;
};

void *muiEngineRun(void*arg);

class Engine
{
private:

	HDC			hDC;
	HGLRC		hRC;
	HGLRC		hRCTH1;
	HWND		hWnd;
	HINSTANCE	hInstance;

	int mapping[4];

	float grd_dx[4];
	float grd_dy[4];

	float grd_bend_bottom_left[4];
	float grd_bend_bottom_right[4];

	float grd_bend_upper_left[4];
	float grd_bend_upper_right[4];

	float grd_FadeLeft[4];
	float grd_FadeLeft_Power[4];

	float grd_FadeRight[4];
	float grd_FadeRight_Power[4];

	float grd_Gamma[4];
	float grd_VerticalFade[4];

	float grd_BendHLeft[4];
	float grd_BendHRight[4];

	int img_width;
	int img_height;

	int grd_width;
	int grd_height;
	int mStepSize;

	int width;
	int height;

	int posY;
	int posX;

	bool FlipY;
	bool FlipRGB;
	bool stopped;
	bool UsePolar;
	bool CloseCycle;
	RotationType Rotate;

	GLuint grdVBO;
	GLuint grdIBO;
	GLuint texID;

	int PrimitiveSize;

	RenderTarget *Target;
	Effect *StitcherFilter;

	pthread_t procThread;
	pthread_mutex_t lock;
	sem_t sleepingProc;

	void *outputImage;

	std::list<void*> imgList;

	ICallback Response;

	IMsgCallback ErroResponse;

	void BuildBuffers();

	void InitGL();

	void load(std::string &filename);

	void processImage(void *source);

	Engine(void);	

	void process();

	friend void *muiEngineRun(void*arg);

	void destroyWindow();

	void CallError(const char *text)
	{
		if(ErroResponse != NULL)
		{
			ErroResponse(text);
		}
	}

	BOOL SetVSync( int interval )
	{
		if( strstr( (const char *)glGetString( GL_EXTENSIONS ), "WGL_EXT_swap_control" ) )
		{
			typedef BOOL (APIENTRY *PFN)( int );
			if ( PFN wglSwapIntervalEXT = (PFN)wglGetProcAddress( "wglSwapIntervalEXT" ) )
				return wglSwapIntervalEXT( interval );
		}
		return FALSE; // (Error: WGL_EXT_swap_control extension is not available.\n");
	}

public:	
	
	~Engine(void);	

	void addImage(void *source);	

	int getResultWidth() { return width; }

	int getResultHeight() { return height; }

	bool initialize(std::string &filename, int grd_width, int grd_height, int im_width, int im_height, bool flipY, bool flipRGB, bool usePolar, RotationType rotate90, bool closeCycle, ICallback response );

	void startEngine();

	void stopEngine();	

	void setMapping(MappingType firstPlane, MappingType secondPlane, MappingType thirdPlane, MappingType fourthPlane);	

	void setMessageCallback(IMsgCallback mes)
	{
		ErroResponse = mes;
	}

	static Engine& getInstance()
	{
		static Engine instance;

		return instance;
	}
};

#endif