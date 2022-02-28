#include "OGLShader.h"




OGLShader::OGLShader( const char* vname, const char* fname )
{
	std::string tmp; 

	vs = loadShader( tmp, GL_VERTEX_SHADER, vname );   // �sĶshader�åB��id�Ǧ^vs 
	tmp = ""; 
	fs = loadShader( tmp, GL_FRAGMENT_SHADER, fname ); 

	program = glCreateProgram();   // �Ыؤ@��program 
	glAttachShader( program, vs );   // ��vertex shader��program�s���W 
	glAttachShader( program, fs );   // ��fragment shader��program�s���W 

	glLinkProgram( program );      // �ھڳQ�s���W��shader, link�X�U��processor 
}
OGLShader::~OGLShader()
{
	glDetachShader( program, vs ); 
	glDetachShader( program, fs ); 
	glDeleteShader( vs ); 
	glDeleteShader( fs ); 
	glDeleteProgram( program ); 
}

void OGLShader::loadFile( const char* filename, std::string &string )
{
	std::ifstream fp(filename); 
	if( !fp.is_open() ){ 
		std::cout << "Open <" << filename << "> error." << std::endl; 
		return; 
	} 

	char temp[300]; 
	while( !fp.eof() ){ 
		fp.getline( temp, 300 ); 
		string += temp; 
		string += '\n'; 
	} 

	fp.close(); 
}

unsigned int OGLShader::loadShader( std::string &source, int type, const char* filename )
{
	loadFile( filename, source );      // ��{���XŪ�isource 

	unsigned int ShaderID; 
	ShaderID = glCreateShader( type );      // �i�DOpenGL�ڭ̭n�Ъ��O����shader 

	const char* csource = source.c_str();   // ��std::string���c�ഫ��const char* 

	glShaderSource( ShaderID, 1, &csource, NULL );      // ��{���X��i�h���Ыت�shader object�� 
	glCompileShader( ShaderID );                  // �sĶshader 
	char error[1000] = ""; 
	glGetShaderInfoLog( ShaderID, 1000, NULL, error );   // �o�O�sĶ�L�{���T��, ���~���򪺧�L���error�̭� 
	std::cout << "File: <" << filename << "> Complie status: \n" << error << std::endl;   // �M���X�X�� 

	return ShaderID; 
}

void OGLShader::useShader()
{
	glUseProgram( program );
}

void OGLShader::delShader()
{
	glUseProgram( 0 ); 
}

unsigned int OGLShader::getProgramID()
{
	return program;
}