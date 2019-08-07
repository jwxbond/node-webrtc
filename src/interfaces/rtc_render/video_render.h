#pragma once

// #include "src/converters.h"
// #include "src/converters/napi.h"

// namespace rtc { template <typename T> class scoped_refptr; }
// namespace webrtc { class I420Buffer; }
// namespace webrtc { class I420BufferInterface; }
namespace webrtc { class VideoFrameBuffer; }

// namespace node_webrtc {


// DECLARE_CONVERTER(I420ImageData, rtc::scoped_refptr<webrtc::I420Buffer>)

// DECLARE_FROM_NAPI(rtc::scoped_refptr<webrtc::I420Buffer>)
// DECLARE_TO_NAPI(rtc::scoped_refptr<webrtc::I420BufferInterface>)
// DECLARE_TO_NAPI(rtc::scoped_refptr<webrtc::VideoFrameBuffer>)

// }  // namespace node_webrtc



namespace node_webrtc {

class VideoRenderModule
{
public:
	virtual void StartRender() = 0;
	virtual void StopRender() = 0;
	virtual webrtc::VideoFrameBuffer* GetCurrentFrameBuffer() = 0;

protected:
	virtual ~VideoRenderModule() {}

};

}
