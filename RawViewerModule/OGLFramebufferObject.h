#ifndef _OPENGL_FRAMEBUFFEROBJECT_
#define _OPENGL_FRAMEBUFFEROBJECT_

#include <iostream>

#include "OGLHeader.h"
#include "Vector3d.h"

#include <vector>

class OGLFramebufferObject
{
	unsigned int FBO;
	unsigned int width, height;

	unsigned int createTexture( int w, int h, bool isDepth );
public:
	std::vector<unsigned int> renderTex;
	std::vector<unsigned int> depthTex;
	std::vector<Vector2> renderTexSize;
	std::vector<Vector2> depthTexSize;
	// * �إߤ@�s��framebuffer object
	OGLFramebufferObject();
	// * �إߤ@�s��framebuffer object
	// * �P�ɫإߦ�m�����`�ק���U�@
	OGLFramebufferObject( int originTexWidth, int originTexHeight );
	~OGLFramebufferObject();
	// * ��framebuffer object�K�[����
	void addTexture( int w, int h, bool isDepth );

	void useFbo();
	void useFbo( unsigned int rTexIndex, unsigned int dTexIndex );
	void delFbo();


	// * ���oFBO��handle
	unsigned int getFramebufferID();
};

#endif