#include <stdint.h>
#include <stdio.h>

namespace node_webrtc {


    class RenderFrameI420;
    class RenderFrameRGBA;

    class RenderFrameData{
    public:
        int width;
        int height;
        uint8_t *bytes;

        static RenderFrameData Create(int width, int height, uint8_t *bytes = nullptr)
        {
            RenderFrameData *imageData = new RenderFrameData();
            imageData->width = width;
            imageData->height = height;
            imageData->bytes = bytes;
            return *imageData;
        }

        RenderFrameI420* toI420();
        RenderFrameRGBA* toRBGA();
    };


    class RenderFrameI420{
    public:
        static RenderFrameI420* Create(RenderFrameData* imageData)
        {
            return new RenderFrameI420(imageData);
        }

        ~RenderFrameI420()
        {
            if( data != nullptr ) {
                delete data;
                data = nullptr;
            }
        }

        size_t sizeOfLuminancePlane() const {
            return static_cast<size_t>(width() * height());
        }

        size_t sizeOfChromaPlane() const {
            return sizeOfLuminancePlane() / 4;
        }

        uint8_t* dataY() {
            return data->bytes;
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
            return data->width;
        }

        int height() const {
            return data->height;
        }

    private:
        explicit RenderFrameI420(RenderFrameData* data): data(data) {}
        RenderFrameData* data;
    };

    class RenderFrameRGBA{
    
    public:
        ~RenderFrameRGBA()
        {
            if( data != nullptr ) {
                delete data;
                data = nullptr;
            }
        }

        static RenderFrameRGBA* Create(RenderFrameData* imageData)
        {
            return new RenderFrameRGBA(imageData);
        }

        uint8_t* dataRgba() {
            return data->bytes;
        }

        int strideRgba() const {
            return width() * 4;
        }

        int width() const {
            return data->width;
        }

        int height() const {
            return data->height;
        }

    private:
        explicit RenderFrameRGBA(RenderFrameData* data): data(data) {}
        RenderFrameData* data;
    };
}
