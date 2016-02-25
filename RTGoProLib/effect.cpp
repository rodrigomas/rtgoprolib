#include "Effect.h"
#include <cstdio>
#include <cstdlib>

Effect::Effect()
{
    _error = false;
}

Effect::~Effect()
{
	if(_error)
		return;

    glDetachShader( _prog, _v );
    glDetachShader( _prog, _f );

    glDeleteShader( _v );
    glDeleteShader( _f );
    glDeleteProgram( _prog );
}

void Effect::printShaderInfoLog( GLuint shader )
{
    int logLenght = 0;
    glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLenght );

    if( logLenght > 1 )
    {
        int n;
        char* log = new char[logLenght];
        glGetShaderInfoLog( shader, logLenght, &n, log );
        printf( "%s\n", log );
        delete[] log;
    }
}

void Effect::printfProgramInfoLog( GLuint shader )
{
    int logLenght = 0;
    glGetProgramiv( shader, GL_INFO_LOG_LENGTH, &logLenght );

    if( logLenght > 1 )
    {
        int n;
        char* log = new char[logLenght];
        glGetProgramInfoLog( shader, logLenght, &n, log );
        printf( "%s\n", log );
        delete[] log;
    }
}

GLcharARB* Effect::loadShaderSource( const char* fileName )
{
    FILE *fp;
    if ( !( fp = fopen( fileName,"rb" ) ) )
    {
        _error = true;
        printf( "Error opening shader.\n" );
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    GLcharARB* shaderSource = new GLcharARB[length + 1];
    fread( shaderSource, 1, length, fp);
    shaderSource[length] = '\0';
    return shaderSource; 
}

GLuint Effect::compileShader( const char* fileName, int shaderType, bool isScript, IMsgCallback callback)
{
	const GLcharARB* shaderSource = NULL;
	
	if (isScript)
	{
		shaderSource = fileName;
	}
	else
	{
		shaderSource = loadShaderSource(fileName);
	}		

    if( _error == false )
    {
        GLuint shader = glCreateShader( shaderType );
        glShaderSource( shader, 1, &shaderSource, NULL );

        glCompileShader( shader );
        printShaderInfoLog( shader );
        GLint hasCompiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &hasCompiled );
        if( !hasCompiled)
        {
			if(callback != NULL)
			{
				char Temp[3000];

				_error = true;
				sprintf(Temp, "Error compiling shader %s.\nDetails:\n", fileName );
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				char *data = new char[maxLength + 2 + strlen(Temp)];

				glGetShaderInfoLog(shader, maxLength, &maxLength, data);

				strcat(Temp, data);

				delete[] data;

				callback(data);
			}
        }
		
		if (!isScript)
			delete[] shaderSource;

        return shader;
    }
    else
    {
        return 0;
    }
}

bool Effect::compileShaders( const char* vertexShaderFileName, const char* fragmentShaderFileName, bool isScript, IMsgCallback callback)
{
    bool progOk;

    _v = compileShader( vertexShaderFileName, GL_VERTEX_SHADER, isScript, callback);
    _f = compileShader( fragmentShaderFileName, GL_FRAGMENT_SHADER, isScript, callback);

    if( _error == false )
    {
        _prog = glCreateProgram();
        glAttachShader( _prog, _v );
        glAttachShader( _prog, _f );
        glLinkProgram( _prog );

        printfProgramInfoLog( _prog );
        GLint hasLinked;
        glGetProgramiv( _prog, GL_LINK_STATUS, &hasLinked );
        if( !hasLinked )
        {
            _error = true;
			
			if(callback != NULL)
			{
				callback( "Error linking shaders.\n" );
			}

            progOk = false;
        }
        else
        {
            progOk = true;
        }
    }
    else
    {
        progOk = false;
    }

    //if( progOk == false )
    //{
    //    printf( "Error compiling %s and %s.\n", vertexShaderFileName, fragmentShaderFileName );
    //    printf( "---------------------------------------------------------\n\n" );
    //}
	_error = !progOk;
    return progOk;

}

void Effect::activate()
{
    if( _error == false )
        glUseProgram( _prog );
}

void Effect::deactivate()
{
    glUseProgram( 0 );
}

void Effect::setUniformi( const GLcharARB* name, GLint value )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform1i( loc, value );
}

void Effect::setUniformiv( const GLcharARB* name, GLsizei n, GLint* value )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform1iv( loc, n, value );
}

void Effect::setUniformf( const GLcharARB* name, GLfloat value )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform1f( loc , value );
}

void Effect::setUniformfv( const GLcharARB* name, GLsizei n, GLfloat* value )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform1fv( loc, n, value );
}

void Effect::setUniform2f( const GLcharARB* name, GLfloat* vec )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform2f( loc, vec[0], vec[1] );
}

void Effect::setUniform4f( const GLcharARB* name, GLfloat* vec )
{
    int loc = glGetUniformLocation( _prog, name );
    glUniform4f( loc, vec[0], vec[1], vec[2], vec[3] );
}

void Effect::setUniformMatrix4fv( const GLcharARB* name, GLfloat* mat )
{
	int loc = glGetUniformLocation( _prog, name );
	glUniformMatrix4fv( loc, 16, false, mat );
}

void Effect::setUniformMatrix3fv( const GLcharARB* name, GLfloat* mat )
{
	int loc = glGetUniformLocation( _prog, name );
	glUniformMatrix3fv( loc, 9, false, mat );
}
