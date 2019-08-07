#pragma once

#ifndef RTC_RENDER_VIDEO_RENDER_OPENGL
#define RTC_RENDER_VIDEO_RENDER_OPENGL

#include "src/interfaces/rtc_render/video_render.h"

namespace node_webrtc {

class VideoRenderOpenGL : public VideoRenderModule
{
public:
	VideoRenderOpenGL(const char* renderId);
	~VideoRenderOpenGL();

	const char* CurrentRenderId();
	int32_t StartRender();
	int32_t StopRender();
	webrtc::VideoFrameBuffer* GetCurrentFrameBuffer();

	//for test
	void ExecRenderTask(int taskId = 0);

protected:
	const char* mRenderId;

	//for test
	void InitOpenGLEnvironment(int w, int h);
	bool mOpenGLInited;
	int mWidth;
	int mHeight;

};

}

#endif
