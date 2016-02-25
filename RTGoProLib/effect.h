#ifndef _GLSLSHADER_
#define _GLSLSHADER_

#ifdef _WIN32
	#include <windows.h>
#endif

#include "rtgopro.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>


class Effect 
{

public:
    Effect();

    ~Effect();

    bool compileShaders( const char* vertexShaderFileName, const char* fragmentShaderFileName, bool isScript, IMsgCallback callback = NULL );
    void activate();
    void deactivate();
    void setUniformi( const GLcharARB* name, GLint value );
    void setUniformiv( const GLcharARB* name, GLsizei n, GLint* value );
    void setUniformf( const GLcharARB* name, GLfloat value );
    void setUniformfv( const GLcharARB* name, GLsizei n, GLfloat* value );
    void setUniform2f( const GLcharARB* name, GLfloat* vec );
    void setUniform4f( const GLcharARB* name, GLfloat* vec );
	void setUniformMatrix4fv( const GLcharARB* name, GLfloat* mat );
	void setUniformMatrix3fv( const GLcharARB* name, GLfloat* mat );

private:
	GLcharARB* loadShaderSource( const char* fileName );
    void printShaderInfoLog( GLuint shader );
    void printfProgramInfoLog( GLuint shader );
    GLuint compileShader( const GLcharARB* shaderSource, int shaderType, bool isScript, IMsgCallback callback);
    
private:
    GLuint _v;
    GLuint _f;
    GLuint _prog;
    bool _error;
};

#endif