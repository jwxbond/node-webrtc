#include <stdio.h>
#include <stdint.h>

namespace rtc { template <typename T> class scoped_refptr; }
namespace webrtc { class I420Buffer; }
namespace webrtc { class I420BufferInterface; }
namespace webrtc { class VideoFrame; }


namespace node_webrtc {

    class RenderFrameI420;
    class RenderFrameRgba;

    //frameData
    class RenderFrameData{
    public:
        int width;
        int height;
        uint8_t *bytes;
        size_t byteLength;

        static RenderFrameData Create(int width, int height, uint8_t *bytes, size_t length)
        {
            RenderFrameData data;
            data.width = width;
            data.height = height;
            data.bytes = bytes;
            data.byteLength = length;
            return data;
            // return {width, height, bytes, length};
        }

        static rtc::scoped_refptr<webrtc::I420Buffer> CreateI420FrameWithRenderBuffer(int width, int height, uint8_t* piexls, size_t length);


        RenderFrameI420 toI420() const;
        RenderFrameRgba toRgba() const;
    };

    //i420Frame
    class RenderFrameI420{
    public:
        static RenderFrameI420 Create(RenderFrameData data)
        {
            auto expectedByteLength = static_cast<size_t>(data.width * data.height * 1.5);
            auto actualByteLength = data.byteLength;
            if (actualByteLength != expectedByteLength) { 
                //Error
            }
            RenderFrameI420 i420Frame(data);
            return i420Frame;
        }

        ~RenderFrameI420()
        {

        }

        size_t sizeOfLuminancePlane() const {
            return static_cast<size_t>(width() * height());
        }

        size_t sizeOfChromaPlane() const {
            return sizeOfLuminancePlane() / 4;
        }

        uint8_t* dataY() {
            return data.bytes;
        }

        int strideY() const {
            return width();
        }

        uint8_t* dataU() {
            return dataY() + sizeOfLuminancePlane();
        }

        int strideU() const {
            return width() / 2;
        }

        uint8_t* dataV() {
            return dataU() + sizeOfChromaPlane();
        }

        int strideV() const {
            return strideU();
        }

        int width() const {
            return data.width;
        }

        int height() const {
            return data.height;
        }

    private:
        explicit RenderFrameI420(RenderFrameData data): data(data) {}
        RenderFrameData data;
    };

    //rbgaFrame
    class RenderFrameRgba{
    
    public:
        static RenderFrameRgba Create(RenderFrameData data)
        {
            auto expectedByteLength = static_cast<size_t>(data.width * data.height * 4); 
            auto actualByteLength = data.byteLength;
            if (actualByteLength != expectedByteLength) {

            }
            RenderFrameRgba rbgaFrame(data);
            return rbgaFrame;
        }
        ~RenderFrameRgba()
        {

        }


        uint8_t* dataRgba() {
            return data.bytes;
        }

        int strideRgba() const {
            return width() * 4;
        }

        int width() const {
            return data.width;
        }

        int height() const {
            return data.height;
        }

    private:
        explicit RenderFrameRgba(RenderFrameData data): data(data) {}
        RenderFrameData data;
    };


    // static bool RgbaToI420( RenderFrameRgba rbgaFrame, RenderFrameI420 i420Frame );
    // static rtc::scoped_refptr<webrtc::I420Buffer> CreateI420Buffer(RenderFrameI420 i420Frame);

}
