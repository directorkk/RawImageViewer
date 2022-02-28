#ifndef _OPENGL_SHADER_
#define _OPENGL_SHADER_

#include <iostream>

#include "OGLHeader.h"
#include "Util.h"

#include <string>
#include <fstream>

class OGLShader
{
	unsigned int vs, fs, program;
	void loadFile( const char* filename, std::string &string );
	unsigned int loadShader( std::string &source, int type, const char* filename ) ;
public:
	OGLShader( const char* vname, const char* fname );
	~OGLShader();

	void useShader();
	void delShader();
	unsigned int getProgramID();
};

#endif