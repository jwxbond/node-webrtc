
#include "src/interfaces/rtc_render/render_frame_data.h"

#include <webrtc/api/video/i420_buffer.h>

#include <libyuv.h>
#include <memory.h>

namespace node_webrtc {


RenderFrameI420 RenderFrameData::toI420() const
{
  return RenderFrameI420::Create(*this);

}

RenderFrameRgba RenderFrameData::toRgba() const
{
  return RenderFrameRgba::Create(*this);
}


static bool RgbaToI420( RenderFrameRgba rgbaFrame, RenderFrameI420 i420Frame )
{
    if( rgbaFrame.width() != i420Frame.width() || rgbaFrame.height() != i420Frame.height() )
    {
        return false;
    }

    int ret = libyuv::ABGRToI420(
      rgbaFrame.dataRgba(),
      rgbaFrame.strideRgba(),
      i420Frame.dataY(),
      i420Frame.strideY(),
      i420Frame.dataU(),
      i420Frame.strideU(),
      i420Frame.dataV(),
      i420Frame.strideV(),
      rgbaFrame.width(),
      rgbaFrame.height()
    );
    return ret == 0;
}

static rtc::scoped_refptr<webrtc::I420Buffer> CreateI420Buffer(int width, int height, uint8_t* piexls, size_t length)
{
    
    RenderFrameData rgbaData = RenderFrameData::Create(width, height, piexls, length);
    RenderFrameRgba rgbaFrame = rgbaData.toRgba();

    size_t i420ByteLength= static_cast<size_t>(width * height * 1.5);
    uint8_t *i420Bytes = (uint8_t *)malloc(i420ByteLength);
    RenderFrameData i420Data = RenderFrameData::Create(width, height, i420Bytes, i420ByteLength);
    RenderFrameI420 i420Frame = i420Data.toI420();

    auto buffer = webrtc::I420Buffer::Create(i420Frame.width(), i420Frame.height());

    bool ret = RgbaToI420(rgbaFrame, i420Frame);
    if( ret )
    {
        memcpy(buffer->MutableDataY(), i420Frame.dataY(), i420Frame.sizeOfLuminancePlane());
        memcpy(buffer->MutableDataU(), i420Frame.dataU(), i420Frame.sizeOfChromaPlane());
        memcpy(buffer->MutableDataV(), i420Frame.dataV(), i420Frame.sizeOfChromaPlane());
    }
    return buffer;
}


}
