#include "OGLFramebufferObject.h"



OGLFramebufferObject::OGLFramebufferObject()
{
	glGenFramebuffers( 1, &FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
	{
		int error = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( error != GL_FRAMEBUFFER_COMPLETE ){
			std::cout << "Framebuffer is not OK, status: " << error << std::endl;
		}
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

OGLFramebufferObject::OGLFramebufferObject( int originWidth, int originHeight )
{
	renderTex.push_back( createTexture( originWidth, originHeight, false) );
	depthTex.push_back( createTexture( originWidth, originHeight, true) );
	renderTexSize.push_back(Vector2(originWidth, originHeight));
	depthTexSize.push_back(Vector2(originWidth, originHeight));

	glGenFramebuffers( 1, &FBO );
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex[renderTex.size()-1], 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex[depthTex.size()-1], 0 );
	{
		int error = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		if( error != GL_FRAMEBUFFER_COMPLETE ){
			std::cout << "Framebuffer is not OK, status: " << error << std::endl;
		}
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

OGLFramebufferObject::~OGLFramebufferObject()
{
	glDeleteBuffers( 1, &FBO );
}

void OGLFramebufferObject::addTexture( int w, int h, bool isDepth )
{
	if( !isDepth ){
		renderTex.push_back( createTexture( w, h, false) );
		renderTexSize.push_back(Vector2(w, h));
	}
	else{
		depthTex.push_back( createTexture( w, h, true) );
		depthTexSize.push_back(Vector2(w, h));
	}

	glBindFramebuffer( GL_FRAMEBUFFER, FBO );

	if( !isDepth )
		glFramebufferTexture2D( GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex[renderTex.size()-1], 0 );
	else
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex[depthTex.size()-1], 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

unsigned int OGLFramebufferObject::getFramebufferID()
{
	return FBO;
}

void OGLFramebufferObject::useFbo()
{
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
}
void OGLFramebufferObject::useFbo( unsigned int rTexIndex, unsigned int dTexIndex )
{
	int tmpT=0, tmpDT=0;
	tmpT = rTexIndex;
	tmpDT = dTexIndex;
	glBindFramebuffer( GL_FRAMEBUFFER, FBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER,  GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex[tmpT], 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex[tmpDT], 0 );
}

void OGLFramebufferObject::delFbo()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

unsigned int OGLFramebufferObject::createTexture( int w, int h, bool isDepth )
{
	unsigned int textureID;

	glGenTextures( 1, &textureID );
	glBindTexture( GL_TEXTURE_2D, textureID );
	glTexImage2D( GL_TEXTURE_2D, 0,
		isDepth ? GL_DEPTH_COMPONENT : GL_RGBA8,
		w, h, 0,
		isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,
		GL_FLOAT, NULL );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

	int error;
	error = glGetError();
	if( glGetError() != 0 ){
		std::cout << "Error happened while loading the texture: " << gluErrorString(error) << std::endl;
	}
	glBindTexture( GL_TEXTURE_2D, 0 );

	return textureID;
}