#include "Engine.h"

unsigned int GetTime() 
{
	struct _timeb timebuffer;

	_ftime(&timebuffer);

	return (unsigned int)timebuffer.time * 1000 + timebuffer.millitm;
}

Engine::Engine(void)
{
	grd_dx[0] = grd_dx[1] = grd_dx[2] = grd_dx[3] = 0;
	grd_dy[0] = grd_dy[1] = grd_dy[2] = grd_dy[3] = 0;

	grd_bend_bottom_left[0] = grd_bend_bottom_left[1] = grd_bend_bottom_left[2] = grd_bend_bottom_left[3] = 0;
	grd_bend_bottom_right[0] = grd_bend_bottom_right[1] = grd_bend_bottom_right[2] = grd_bend_bottom_right[3] = 0;

	grd_bend_upper_left[0] = grd_bend_upper_left[1] = grd_bend_upper_left[2] = grd_bend_upper_left[3] = 0;
	grd_bend_upper_right[0] = grd_bend_upper_right[1] = grd_bend_upper_right[2] = grd_bend_upper_right[3] = 0;

	grd_FadeLeft[0] = grd_FadeLeft[1] = grd_FadeLeft[2] = grd_FadeLeft[3] = 0;
	grd_FadeLeft_Power[0] = grd_FadeLeft_Power[1] = grd_FadeLeft_Power[2] = grd_FadeLeft_Power[3] = 0;

	grd_FadeRight[0] = grd_FadeRight[1] = grd_FadeRight[2] = grd_FadeRight[3] = 0;
	grd_FadeRight_Power[0] = grd_FadeRight_Power[1] = grd_FadeRight_Power[2] = grd_FadeRight_Power[3] = 0;

	grd_Gamma[0] = grd_Gamma[1] = grd_Gamma[2] = grd_Gamma[3] = 0;
	grd_VerticalFade[0] = grd_VerticalFade[1] = grd_VerticalFade[2] = grd_VerticalFade[3] = 0;

	grd_BendHLeft[0] = grd_BendHLeft[1] = grd_BendHLeft[2] = grd_BendHLeft[3] = 0;
	grd_BendHRight[0] = grd_BendHRight[1] = grd_BendHRight[2] = grd_BendHRight[3] = 0;

	mapping[0] = 0;
	mapping[1] = 1;
	mapping[2] = 2;
	mapping[3] = 3;

	posY = 0;
	posX = 0;

	img_width = 960;
	img_height = 540;

	grd_width = 64;
	grd_height = 64;
	mStepSize = 0;

	PrimitiveSize = 1;

	FlipY = false;
	FlipRGB = false;
	stopped = false;
	UsePolar = false;

	Response = NULL;
	ErroResponse = NULL;
}

void Engine::setMapping(MappingType firstPlane, MappingType secondPlane, MappingType thirdPlane, MappingType fourthPlane)
{
	mapping[0] = (int)firstPlane;
	mapping[1] = (int)secondPlane;
	mapping[2] = (int)thirdPlane;
	mapping[3] = (int)fourthPlane;
}

void Engine::BuildBuffers()
{
	// Gerando Array dos vértices
	int WIDTH = (grd_width - mStepSize);
	int HEIGHT = (grd_height - mStepSize);

	TexturePositionVertex *mVertexArray = new TexturePositionVertex[WIDTH * HEIGHT];

	for (int x = 0; x < WIDTH; x++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			mVertexArray[x + y * WIDTH].x = x / (float)(WIDTH-1);
			mVertexArray[x + y * WIDTH].y = y / (float)(HEIGHT-1);
			mVertexArray[x + y * WIDTH].z = 0;
			mVertexArray[x + y * WIDTH].u = x / (float)(WIDTH - 1);
			mVertexArray[x + y * WIDTH].v = y / (float)(HEIGHT - 1);
		}
	}

	// Criando os Triângulos
	// A   B
	// *—--*
	// | \ |
	// *—--*
	// C   D
	int *mIndexArray = new int[(WIDTH - 1) * (HEIGHT - 1) * 6];
	int counter = 0;
	for (int y = 0; y < HEIGHT - 1; y++)
	{
		for (int x = 0; x < WIDTH - 1; x++)
		{
			int lowerLeft = x + y * WIDTH;
			int lowerRight = (x + 1) + y * WIDTH;
			int topLeft = x + (y + 1) * WIDTH;
			int topRight = (x + 1) + (y + 1) * WIDTH;

			// Triângulo ADC (Anti Horário)
			mIndexArray[counter++] = topLeft;
			mIndexArray[counter++] = lowerRight;
			mIndexArray[counter++] = lowerLeft;

			// Triângulo ABD (Anti Horário)
			mIndexArray[counter++] = topLeft;
			mIndexArray[counter++] = topRight;
			mIndexArray[counter++] = lowerRight;
		}
	}

	glGenBuffers(1, &grdVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grdVBO);
	glBufferData(GL_ARRAY_BUFFER, WIDTH * HEIGHT * sizeof(TexturePositionVertex), mVertexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &grdIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grdIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (WIDTH - 1) * (HEIGHT - 1) * 6 * sizeof(int), mIndexArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	PrimitiveSize = (WIDTH - 1) * (HEIGHT - 1) * 6;

	delete[] mIndexArray;
	delete[] mVertexArray;
}

void Engine::load(std::string &filename)
{
	FILE *fp = fopen(filename.c_str(), "r");	

	char Temp[100];

	fscanf(fp, "%s", Temp);

	if(strcmp("RTGOPROV02", Temp) != 0)
	{
		CallError("Invalid RTX File or Incompatible Version. Only RTGOPROV02 are accepted!");

		fclose(fp);
		return;
	}

	for (register int i = 0; i < 4; i++)
	{
		int p = 0;

		fscanf(fp, "%*s %d", &p);
		fscanf(fp, "%*s %f", &grd_dx[p]);
		fscanf(fp, "%*s %f", &grd_dy[p]);
		fscanf(fp, "%*s %f %f %f %f", &grd_bend_bottom_left[p], &grd_bend_bottom_right[p], &grd_bend_upper_left[p], &grd_bend_upper_right[p]);
		fscanf(fp, "%*s %f %f %f", &grd_FadeLeft[p], &grd_FadeRight[p], &grd_VerticalFade[p]);
		fscanf(fp, "%*s %*s %f %f", &grd_FadeLeft_Power[p], &grd_FadeRight_Power[p]);
		fscanf(fp, "%*s %f", &grd_Gamma[p]);
		fscanf(fp, "%*s %f %f", &grd_BendHLeft[p], &grd_BendHRight[p]);
	}

	fclose(fp);
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

bool Engine::initialize(std::string &filename, int grd_width, int grd_height, int im_width, int im_height, bool flipY, bool flipRGB, bool usePolar, RotationType rotate90, bool closeCycle, ICallback response)
{
	
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	
	load(filename);

	float maxbendtop = 0;
	float maxbendbottom = 0;

	for (int i = 0; i < 4; i++)
	{
		maxbendtop = (maxbendtop < grd_bend_upper_right[i]) ? grd_bend_upper_right[i] : maxbendtop;
		maxbendtop = (maxbendtop < grd_bend_upper_left[i]) ? grd_bend_upper_left[i] : maxbendtop;

		maxbendbottom = (maxbendbottom < grd_bend_bottom_right[i]) ? grd_bend_bottom_right[i] : maxbendbottom;
		maxbendbottom = (maxbendbottom < grd_bend_bottom_left[i]) ? grd_bend_bottom_left[i] : maxbendbottom;
	}	
	posX = 0;

	if(rotate90 == rtNone)
	{
		if(closeCycle)
		{
			width = (int)( (im_width * 2 + 10 * ((grd_dx[0]))) );
		} else {
			width = (int)( (im_width * 2 + 10 * ((grd_dx[3]))) + MAXF(grd_BendHLeft[0] * im_width * 0.25f, 0.0f) + MAXF(grd_BendHRight[3] * im_width * 0.25f, 0.0f));
			posX = (int)(MAXF(grd_BendHLeft[0] * im_width * 0.25f, 0.0f));
		}

		posY = (int)(MAXF(maxbendtop * im_height * 0.25f, 0.0f));

		height = im_height / 2 +  (int)( ((maxbendtop > 0.0f) ?(maxbendtop * im_height * 0.25f) : 0.0f) + ((maxbendbottom > 0.0f) ?(maxbendbottom * im_height * 0.25f) : 0.0f));
	} else 
	{
		if(closeCycle)
		{
			width = (int)( (im_height * 2 + 10 * ((grd_dx[0]))) );
		} else {
			width = (int)( (im_height * 2 + 10 * ((grd_dx[3]))) + MAXF(grd_BendHLeft[0] * im_height * 0.25f, 0.0f) + MAXF(grd_BendHRight[3] * im_height * 0.25f, 0.0f));
			posX = (int)(MAXF(grd_BendHLeft[0] * im_height * 0.25f, 0.0f));
		}

		posY = (int)(MAXF(maxbendtop * im_width * 0.25f, 0.0f));

		height = im_width / 2 +  (int)( ((maxbendtop > 0.0f) ?(maxbendtop * im_width * 0.25f) : 0.0f) + ((maxbendbottom > 0.0f) ?(maxbendbottom * im_width * 0.25f) : 0.0f));
	}

	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height


	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "RTGOPROLIB";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		CallError("Failed To Register The Window Class.");
		return false;
	}

	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"RTGOPROLIB",						// Class Name
								"",									// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
	    destroyWindow();
		CallError("Window Creation Error.");
		return false;
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,										    // Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		destroyWindow();					// Reset The Display
		CallError("Can't Create A GL Device Context.");
		return false;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		destroyWindow();					// Reset The Display
		CallError("Can't Find A Suitable PixelFormat.");
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		destroyWindow();					// Reset The Display
		CallError("Can't Set The PixelFormat.");
		return false;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		destroyWindow();						// Reset The Display
		CallError("Can't Create A GL Rendering Context.");
		return false;								// Return FALSE
	}

	//if (!(hRCTH1=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	//{
	//	destroyWindow();					// Reset The Display
	//	CallError("Can't Create A GL Rendering Context.");
	//	return false;								// Return FALSE
	//}

	// BOOL error = wglShareLists(hRC, hRCTH1);

	//if(error == FALSE)
	//{
	//	 DWORD errorCode = GetLastError();
	//	LPVOID lpMsgBuf;
	//	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	//		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf, 0, NULL);
	//	
	//	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	//	CallError((LPCTSTR)lpMsgBuf);

	//	LocalFree(lpMsgBuf);
	//	
	//	//Destroy the GL context and just use 1 GL context		
	//	wglDeleteContext(hRCTH1);
	//	return false;		
	//}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		destroyWindow();	
		CallError("Can't Activate The GL Rendering Context.");
		return false;								// Return FALSE
	}

	//ShowWindow(hWnd,SW_SHOW);						// Show The Window
	ShowWindow(hWnd,SW_HIDE);						// Show The Window	

	img_width = im_width / 2;
	img_height = im_height / 2;

	grd_width = grd_width;
	grd_height = grd_height;

	FlipY = flipY;
	FlipRGB = flipRGB;
	UsePolar = usePolar;
	Rotate = rotate90;
	CloseCycle = closeCycle;

	Response = response;

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		CallError("Can't Activate The Glew Shader Engine");
		CallError((const char*)glewGetErrorString(err));
		return false;				
	}

	if( SetVSync(0) == FALSE )
	{
		CallError("Can't disable VSYNC, please disable it at Driver Options.");
	}

	BuildBuffers();	

	Target = new RenderTarget(width, height, true, GL_UNSIGNED_BYTE);
	Target->Initialize();

	outputImage = new unsigned char[width * height * 4];
	memset(outputImage, 0, width * height * 4 * sizeof(unsigned char)); 	

	glEnable(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*) outputImage);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	char *vert = NULL;
	int vert_size = 0;

	char *frag = NULL;
	int frag_size = 0;

	Utils::LoadTextResource(IDR_DEFAULT_VERT, &vert, &vert_size);
	vert[vert_size - 1] = '\0';

	Utils::LoadTextResource(IDR_DEFAULT_FRAG, &frag, &frag_size);
	frag[frag_size - 1] = '\0';

	StitcherFilter = new Effect();
	StitcherFilter->compileShaders(vert, frag, true, ErroResponse);

	delete [] vert;
	delete [] frag;

	CallError("Stitcher Shader Loaded");	

	wglMakeCurrent(NULL,NULL);

	return true;				
}

void *muiEngineRun(void*arg)
{		
	Engine *engine = (Engine*)arg;

	if(engine == NULL) return NULL;

	engine->process();

	return NULL;
}

void Engine::process()
{
	void *source = NULL;

	while(!stopped)
	{
		sem_wait(&sleepingProc); 

		if(stopped == true)
			break;

		pthread_mutex_lock(&lock);
	
		source = (void*)imgList.front();

		imgList.pop_front();

		pthread_mutex_unlock(&lock);

		processImage(source);
	}	
}

void Engine::startEngine()
{
	stopped = false;

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		CallError("Mutex init failed\n");
	}

	if (sem_init(&sleepingProc, 0, 0) != 0)
	{
		CallError("Semaphore init failed\n");
	}	

	if (pthread_create(&procThread, NULL, muiEngineRun, (void*)this))
	{
		CallError("Error creating thread\n");
	}
}

void Engine::stopEngine()
{
	stopped = true;

	sem_post(&sleepingProc);

	if (pthread_join(procThread, NULL)) 
	{
		CallError("Error joining thread\n");
	}

	sem_destroy(&sleepingProc);

	pthread_mutex_destroy(&lock);
}

void Engine::addImage(void *source)
{
	pthread_mutex_lock(&lock);
	
	imgList.push_back(source);

	pthread_mutex_unlock(&lock);

	sem_post(&sleepingProc); 

	//processImage(source);
}

void Engine::processImage(void *source)
{
	unsigned int t0, t1;
	static char Temp[100];
	
	//wglMakeCurrent(hDC,hRCTH1);
	wglMakeCurrent(hDC,hRC);

	t0 = GetTime();

	glDisable(GL_DEPTH_TEST);

	glShadeModel(GL_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDrawBuffer(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Target->SetTarget();

	glViewport(0,0,width,height);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glPushAttrib(GL_TRANSFORM_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

	int w = img_width;
	int h = img_height;

	if(Rotate != rtNone)
	{
		h = img_width;
		w = img_height;
	}

	if(CloseCycle)
	{
		glTranslatef(-w / 2.0f + (float)posX, (float)posY, 0.0f);
	} else 
	{
		glTranslatef((float)posX, (float)posY, 0.0f);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE);
	glActiveTexture(GL_TEXTURE0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width * 2, img_height * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*) source);		

	StitcherFilter->activate();
	StitcherFilter->setUniformi("renderedTexture", 0);	

	glBindBuffer(GL_ARRAY_BUFFER, grdVBO);
	glInterleavedArrays(GL_T2F_V3F, sizeof(TexturePositionVertex), NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grdIBO);

	StitcherFilter->setUniformf("w", (float)w);
	StitcherFilter->setUniformf("h", (float)h);

	StitcherFilter->setUniformf("UsePolar", (UsePolar) ? 0.5f : -0.5f);
	StitcherFilter->setUniformf("FlipY", (FlipY) ? 0.5f : -0.5f );
	StitcherFilter->setUniformf("FlipRGB", (FlipRGB) ? 0.5f : -0.5f);

	if(Rotate == rtNone)
	{
		StitcherFilter->setUniformf("RotateImg", 0.0f);	
	} else if(Rotate == rt90Degrees)
	{
		StitcherFilter->setUniformf("RotateImg", 90.0f);	
	} else if(Rotate == rtMinus90Degrees)
	{
		StitcherFilter->setUniformf("RotateImg", -90.0f);	
	}	

	glBindTexture(GL_TEXTURE_2D, texID);

	int cnt = (CloseCycle) ? 5 : 4;

	for (register int i = 0; i < cnt; i++)
	{		
		int p = i;

		if (i == 4) p = 0;

		//if(Rotate == rtNone)
		//{
		//	StitcherFilter->setUniformf("OffsetTX", ((p+1) % 2 == 0) ? 0.5f : 0.0f);
		//	StitcherFilter->setUniformf("OffsetTY", (p>=2) ? 0.5f : 0.0f);			
		//}
		//else
		//{
			StitcherFilter->setUniformf("OffsetTX", ((mapping[p] + 1) % 2 == 0) ? 0.5f : 0.0f);
			StitcherFilter->setUniformf("OffsetTY", (mapping[p] >= 2) ? 0.0f : 0.5f);			
//		}		


		float dx = (i != 0) ? (grd_dx[p] * 10) : (0.0f);
		float dy = (i != 0) ? (grd_dy[p] * 10) : (0.0f);

		StitcherFilter->setUniformf("dx", dx + i * w);
		StitcherFilter->setUniformf("dy", dy);
		StitcherFilter->setUniformf("bend_ur", grd_bend_upper_right[p]);
		StitcherFilter->setUniformf("bend_dr", grd_bend_bottom_right[p]);
		StitcherFilter->setUniformf("bend_dl", grd_bend_bottom_left[p]);
		StitcherFilter->setUniformf("bend_ul", grd_bend_upper_left[p]);

		StitcherFilter->setUniformf("bend_left", grd_BendHLeft[p]);
		StitcherFilter->setUniformf("bend_right", grd_BendHRight[p]);

		StitcherFilter->setUniformf("FadeLeft", grd_FadeLeft[p] * 10.0f / (float)w);
		StitcherFilter->setUniformf("FadeRight", grd_FadeRight[p] * 10.0f / (float)w);
		StitcherFilter->setUniformf("FadeLeftPower", grd_FadeLeft_Power[p] * 0.1f);
		StitcherFilter->setUniformf("FadeRightPower", grd_FadeRight_Power[p] * 0.1f);
		
		StitcherFilter->setUniformf("Gamma", grd_Gamma[p] * 0.01f);
		StitcherFilter->setUniformf("VerticalFade", grd_VerticalFade[p] * 0.01f);

		glDrawElements(GL_TRIANGLES, PrimitiveSize, GL_UNSIGNED_INT, NULL);		
	}
	

	glFlush();

	SwapBuffers(hDC);

	StitcherFilter->deactivate();
	Target->UnsetTarget();

	Target->getTexture(outputImage);		

	t1 = GetTime();

	sprintf(Temp, "Processing Time : %d ms\n", (t1 - t0));

	CallError(Temp);

	glPopAttrib();			

	Response(outputImage, source);
}

void Engine::destroyWindow()
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			CallError("Release Of DC And RC Failed.");
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			CallError("Release Rendering Context Failed.");
		}

		//if (!wglDeleteContext(hRCTH1))						// Are We Able To Delete The RC?
		//{
		//	CallError("Release Rendering Context Failed.");
		//}

		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		CallError("Release Device Context Failed.");
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		CallError("Could Not Release hWnd.");
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("RTGOPROLIB",hInstance))			// Are We Able To Unregister Class
	{
		CallError("Could Not Unregister Class.");
		hInstance=NULL;									// Set hInstance To NULL
	}
}

Engine::~Engine(void)
{	
	if(stopped == false)
		stopEngine();

	wglMakeCurrent(hDC,hRC);

	glDeleteTextures(1, (GLuint*) &texID);
	glDeleteBuffers(1, (GLuint*) &grdVBO);
	glDeleteBuffers(1, (GLuint*) &grdIBO);

	if(outputImage != NULL)
		delete [] outputImage;

	if(Target != NULL)
		delete Target;

	if(StitcherFilter != NULL)
		delete StitcherFilter;

	destroyWindow();
}
