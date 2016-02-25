#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

class RenderTarget
{
	int width;
	int height;	
	
	unsigned int uiFrameBuffer;
	unsigned int uiRenderTexture;
	unsigned int uiRenderBuffer;
	int format;

	bool hasDepth;

public:

	int getTextureId() const { return uiRenderTexture; }

	int getWidth() const { return width; }

	int getHeight() const { return height; }

	void SetCurrentTexture();
	
	void SetTarget();

	void getTexture(void *output);
	
	void UnsetCurrentTexture();

	void UnsetTarget();

	void Initialize();

	RenderTarget(int w, int h, bool useDepth, int form);

	~RenderTarget(void);
};

#endif