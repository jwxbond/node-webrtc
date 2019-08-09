
#include "src/interfaces/rtc_render/render_frame_data.h"

#include <webrtc/api/video/i420_buffer.h>

#include <libyuv.h>
#include <memory.h>

namespace node_webrtc {


bool flag = false;
static bool RgbaToI420( RenderFrameRgba rgbaFrame, RenderFrameI420 i420Frame )
{
    if( rgbaFrame.width() != i420Frame.width() || rgbaFrame.height() != i420Frame.height() )
    {
      printf("[RgbaToI420] check width height error!!\n");
      return false;
    }

    // if( !flag ) {
    //   flag = true;
    //   printf("[RgbaToI420]\n");
    //   printf("[RgbaToI420] rgbaFrame.dataRgba=%p\n", rgbaFrame.dataRgba());
    //   printf("[RgbaToI420] rgbaFrame.strideRgba=%d\n", rgbaFrame.strideRgba());
    //   printf("[RgbaToI420] rgbaFrame.width=%d\n", rgbaFrame.width());
    //   printf("[RgbaToI420] rgbaFrame.height=%d\n", rgbaFrame.height());

    //   printf("[RgbaToI420] i420Frame.dataY=%p\n", i420Frame.dataY());
    //   printf("[RgbaToI420] i420Frame.strideY=%d\n", i420Frame.strideY());
    //   printf("[RgbaToI420] i420Frame.dataU=%p\n", i420Frame.dataU());
    //   printf("[RgbaToI420] i420Frame.strideU=%d\n", i420Frame.strideU());
    //   printf("[RgbaToI420] i420Frame.dataV=%p\n", i420Frame.dataV());
    //   printf("[RgbaToI420] i420Frame.strideV=%d\n", i420Frame.strideV());
    // }

    // int ret = libyuv::ABGRToI420(
    int ret = libyuv::RGBAToI420(
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

    // printf("[RgbaToI420] libyuv::ABGRToI420() ret=%d!!\n", ret);

    return ret == 0;
}

bool flag2 = false;
rtc::scoped_refptr<webrtc::I420Buffer> RenderFrameData::CreateI420FrameWithRenderBuffer(int width, int height, uint8_t* piexls, size_t length)
{
    RenderFrameData rgbaData = RenderFrameData::Create(width, height, piexls, length);
    RenderFrameRgba rgbaFrame = rgbaData.toRgba();

    FILE *file1 = fopen("/Users/jiangweixing/Documents/gitlab/kraken-server/test_rgbaData", "w");
    if( file1 != nullptr )
    {
      fwrite(rgbaData.bytes, length, 1, file1);
      fclose(file1);
    }

    size_t i420ByteLength= static_cast<size_t>(width * height * 1.5);
    uint8_t *i420Bytes = (uint8_t *)malloc(i420ByteLength);
    RenderFrameData i420Data = RenderFrameData::Create(width, height, i420Bytes, i420ByteLength);
    RenderFrameI420 i420Frame = i420Data.toI420();

    FILE *file2 = fopen("/Users/jiangweixing/Documents/gitlab/kraken-server/test_i420Data", "w");
    if( file2 != nullptr )
    {
      fwrite(i420Frame.dataY(), i420ByteLength, 1, file2);
      fclose(file2);
    }

    auto buffer = webrtc::I420Buffer::Create(i420Frame.width(), i420Frame.height());

    if( buffer )
    {
      if( !flag2 ){
        printf("[webrtc::I420Buffer::Create] create success!!!\n");
      }
    }

    bool ret = RgbaToI420(rgbaFrame, i420Frame);
    if( ret )
    {
      if( !flag2 ){
        flag2 = true;
        printf("[CreateI420FrameWithRenderBuffer]...to do copy");
        printf("[CreateI420FrameWithRenderBuffer], sizeOfLuminancePlane=%d", i420Frame.sizeOfLuminancePlane());
        printf("[CreateI420FrameWithRenderBuffer], sizeOfChromaPlane=%d\n", i420Frame.sizeOfChromaPlane());

      }
        memcpy(buffer->MutableDataY(), i420Frame.dataY(), i420Frame.sizeOfLuminancePlane());
        memcpy(buffer->MutableDataU(), i420Frame.dataU(), i420Frame.sizeOfChromaPlane());
        memcpy(buffer->MutableDataV(), i420Frame.dataV(), i420Frame.sizeOfChromaPlane());
    }
    return buffer;
}

RenderFrameI420 RenderFrameData::toI420() const
{
  return RenderFrameI420::Create(*this);

}

RenderFrameRgba RenderFrameData::toRgba() const
{
  return RenderFrameRgba::Create(*this);
}

}
