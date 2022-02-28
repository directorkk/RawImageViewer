#include "OGLApp.h"
#include "OGLShader.h"
#include "OGLFramebufferObject.h"

//#include "FFmpegWrapper.h"

#include <fstream>

#define SCR_SCALE 0.5f

class OGLRunAPP : public OGLApp
{
public:
	OGLRunAPP(int w, int h, int f, const char *title);
	~OGLRunAPP();

	bool Init() override;

protected:
	void Render(float dt) override;
	void IdleUpdate(float dt) override;
	void HandleMouseMsg(HWND hWnd, POINT pos) override;
	void HandleKeyboardUpMsg(HWND hWnd) override;

	void DrawQuad();
	void DrawPoints();

private:
	OGLShader *m_pQuadRender;
	OGLShader *m_pTexRender;
	OGLFramebufferObject *m_pFbo;
	std::vector<Vector2> m_aryPoints;

	unsigned int m_hDemoTex;
	unsigned int m_hPbo;

	Vector3 IntToRGB(int Val);
	std::vector<Vector2> GetLinesInPoints(unsigned char *image, int width, int height);
};

OGLRunAPP::OGLRunAPP(int w, int h, int f, const char *title)
	: OGLApp(w, h, f, title)
{
	m_pQuadRender = NULL;
	m_pTexRender = NULL;
	m_pFbo = NULL;

	m_hDemoTex = 0;
	m_hPbo = 0;

	Vector3 t = IntToRGB(8092539);
	int aa = 0;
}

OGLRunAPP::~OGLRunAPP()
{
	if (m_pQuadRender != NULL) {
		delete m_pQuadRender;
	}
	if (m_pTexRender != NULL) {
		delete m_pTexRender;
	}
	if (m_pFbo != NULL) {
		delete m_pFbo;
	}

	glDeleteBuffers(1, &m_hPbo);
}

bool OGLRunAPP::Init()
{
	if (!OGLApp::Init()) {
		return false;
	}

	glewInit();
	glEnable(GL_TEXTURE_2D);

	glOrtho(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 100.0f);

	m_pQuadRender = new OGLShader("QuadRender.vs", "QuadRender.frag");
	m_pTexRender = new OGLShader("QuadRender.vs", "TexRender.frag");
	m_pFbo = new OGLFramebufferObject(m_ClientWidth, m_ClientHeight);

	glGenBuffers(1, &m_hPbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_hPbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, m_ClientWidth * m_ClientHeight * 3, 0, GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pFbo->getFramebufferID());

	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(4);

	std::ifstream input;
	input.open("sample.bmp", std::ios::in | std::ios::binary | std::ios::ate);
	int length = input.tellg();
	int headerLen = 54;
	input.seekg(headerLen, std::ios::beg);
	
	unsigned char *image = new unsigned char[length - headerLen];
	input.read((char*)image, length - headerLen);
	input.close();

	int aa = 0;

	glGenTextures(1, &m_hDemoTex);
	glBindTexture(GL_TEXTURE_2D, m_hDemoTex);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void OGLRunAPP::Render(float dt)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	//DrawPoints();

// 	m_pFbo->useFbo();
// 	m_pQuadRender->useShader();
// 
// 	DrawQuad();
// 
// 	m_pQuadRender->delShader();
// 	m_pFbo->delFbo();
	
// 	m_pFbo->useFbo();
	m_pTexRender->useShader();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hDemoTex);
	glUniform1i(glGetUniformLocation(m_pTexRender->getProgramID(), "tex_0"), 0);

	DrawQuad();

	m_pTexRender->delShader();
// 	m_pFbo->delFbo();
}

void OGLRunAPP::IdleUpdate(float dt)
{
	bool *kbState = GetKeyboardState();
	if (kbState['t']) {

	}
	if (kbState['m']) {

		int bufferSize = m_ClientWidth * m_ClientHeight * 3;
		//BYTE* data = new BYTE[bufferSize];
		m_pFbo->useFbo();
		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_hPbo);
		glBufferData(GL_PIXEL_PACK_BUFFER, bufferSize, 0, GL_STREAM_READ);
		glReadPixels(0, 0, m_ClientWidth, m_ClientHeight, GL_BGR, GL_UNSIGNED_BYTE, 0);

		BYTE* data = (BYTE*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		unsigned char *header = new unsigned char[54];
		std::ifstream input("base.bmp", std::ios::binary);
		input.read((char *)header, 54);
		input.close();
		header[0x13] = m_ClientWidth / 256;
		header[0x12] = m_ClientWidth % 256;
		header[0x17] = m_ClientHeight / 256;
		header[0x16] = m_ClientHeight % 256;
		std::ofstream output("dump.bmp", std::ios::binary);
		output.write((char*)header, 54);
		output.write((char*)data, bufferSize);
		output.close();

		delete[] header;
		int aa = 0;

		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);     // release pointer to the mapped buffer
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		m_pFbo->delFbo();
	}
}

void OGLRunAPP::HandleMouseMsg(HWND hWnd, POINT pos)
{
	float pX = (float)pos.x / GetClientWidth();
	float pY = (float)pos.y / GetClientHeight();
	pX = pX * 2 - 1;
	pY = (pY * 2 - 1) * -1;

	switch (GetMouseLeft()) {
	case 1:
		std::cout << "Left click: (" << pX << ", " << pY << ")" << std::endl;
		break;
	case -1:
		std::cout << "Left click up: (" << pX << ", " << pY << ")" << std::endl;
		break;
	}
	if (MouseLeftPressed()) {
		Vector2 thisPoint = Vector2(pX, pY);
		if (m_aryPoints.size() > 0) {
			Vector2 previousPoint = m_aryPoints[m_aryPoints.size() - 1];
			float len = sqrtf(
				(thisPoint.x - previousPoint.x) * (thisPoint.x - previousPoint.x) +
				(thisPoint.y - previousPoint.y) * (thisPoint.y - previousPoint.y));
			if (len < 0.05) {
				return;
			}
		}
		m_aryPoints.push_back(thisPoint);
	}
}


void OGLRunAPP::HandleKeyboardUpMsg(HWND hWnd)
{
	if (stateKeyboard['a']) {
		std::ofstream output("points_log.txt");
		for (int i = 0; i < m_aryPoints.size(); i++) {
			Vector2 point = m_aryPoints[i];
			output << point.x << "," << point.y << std::endl;
		}
		output.close();
	}
	if (stateKeyboard['c']) {
		m_aryPoints.clear();
	}
}

void OGLRunAPP::DrawQuad()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();
}

void OGLRunAPP::DrawPoints()
{
	glBegin(GL_POINTS);
	for (int i = 0; i < m_aryPoints.size(); i++) {
		Vector2 point = m_aryPoints[i];
		Vector3 color = IntToRGB(i);
		glColor3f(color.x, color.y, color.z);
		glVertex2f(point.x, point.y);
	}
	glEnd();
}

Vector3 OGLRunAPP::IntToRGB(int Val)
{
	int r = Val % 256;
	int g = ((Val - r) / 256) % 256;
	int b = ((Val - 256 * g - r) / (256 * 256)) % 256;

	return Vector3(r/256.f, g/256.f, b/256.f);
}

std::vector<Vector2> OGLRunAPP::GetLinesInPoints(unsigned char *image, int width, int height)
{
	std::vector<Vector2> rtn;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			unsigned int pixel = image[i*width + j];


		}
	}

	return rtn;
}

int main(int argc, char **argv) {
	unsigned int w = GetSystemMetrics(SM_CXSCREEN),
		h = GetSystemMetrics(SM_CYSCREEN);

	OGLRunAPP OGLAppMain(500, 500, 30, "Demo");

	bool aa = !(0 ^ 0);

	
	if (OGLAppMain.Init())
		OGLAppMain.RunLiveLoop();

	return 0;
}
