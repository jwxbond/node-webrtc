#pragma once

#ifndef RTC_RENDER_VIDEO_RENDER_H
#define RTC_RENDER_VIDEO_RENDER_H

#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/api/video/video_frame.h>
#include <webrtc/rtc_base/ref_counted_object.h>


namespace webrtc { class VideoFrame; }

namespace node_webrtc {

class VideoRenderModule
{
public:
	virtual const char* CurrentRenderId() = 0;
	virtual int32_t StartRender() = 0;
	virtual int32_t StopRender() = 0;
	virtual webrtc::VideoFrame GetCurrentFrameBuffer() = 0;

protected:
	virtual ~VideoRenderModule();
};

}

#endif
