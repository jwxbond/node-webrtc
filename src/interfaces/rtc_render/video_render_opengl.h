#pragma once

#ifndef RTC_RENDER_VIDEO_RENDER_OPENGL
#define RTC_RENDER_VIDEO_RENDER_OPENGL


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "src/interfaces/rtc_render/video_render.h"


namespace node_webrtc {

class VideoRenderOpenGL : public VideoRenderModule
{
public:
	VideoRenderOpenGL(const char* renderId);
	VideoRenderOpenGL(const char* renderId, int width, int height);
	~VideoRenderOpenGL();

	const char* CurrentRenderId();
	int32_t StartRender();
	int32_t StopRender();
	webrtc::VideoFrame GetCurrentFrameBuffer();

	//for test
	void ExecRenderTask(int taskId = 0);

protected:
	const char* mRenderId;

	//for test
	int mCurrentTaskId;
	bool InitOpenGLEnvironment();
	void DestroyOpenGLEnvironment();
	bool mOpenGLInited;
	int mWidth;
	int mHeight;

	GLuint mTextureID;
    GLuint mDepthBuffer;
    GLuint mFBO;
};

}

#endif
