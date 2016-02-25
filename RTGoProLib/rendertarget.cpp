#include "RenderTarget.h"

RenderTarget::RenderTarget(int w, int h, bool useDepth, int form)
{
	width = w;
	height = h;
	hasDepth = useDepth;
	format = form;

	Initialize();
}

RenderTarget::~RenderTarget(void)
{
	if (uiFrameBuffer != 0)
		glDeleteFramebuffersEXT(1, &uiFrameBuffer);

	if (uiRenderTexture != 0)
		glDeleteTextures(1, &uiRenderTexture);

	if (uiRenderBuffer != 0)
		glDeleteRenderbuffersEXT(1, &uiRenderBuffer);
}

void RenderTarget::SetCurrentTexture()
{
	glBindTexture(GL_TEXTURE_2D, uiRenderTexture);
}

void RenderTarget::UnsetCurrentTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTarget::SetTarget()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, uiFrameBuffer);

	if (hasDepth)
	{
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, uiRenderBuffer);
	}
}

void RenderTarget::UnsetTarget()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	if (hasDepth)
	{
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	}
}

void RenderTarget::Initialize()
{
	glGenTextures(1, &uiRenderTexture);
	glBindTexture(GL_TEXTURE_2D, uiRenderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (format == GL_FLOAT)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI, width, height, 0, GL_RGBA, format, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, format, NULL);

	glGenFramebuffersEXT(1, &uiFrameBuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, uiFrameBuffer);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, uiRenderTexture, 0);

	int status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	//status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);	

	if (hasDepth)
	{
		glGenRenderbuffersEXT(1, &uiRenderBuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, uiRenderBuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, uiRenderBuffer);
	}

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	bool t = status == GL_FRAMEBUFFER_COMPLETE_EXT;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void RenderTarget::getTexture( void *output )
{
	glBindTexture(GL_TEXTURE_2D, uiRenderTexture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, format, output );
	glBindTexture(GL_TEXTURE_2D, 0);
}