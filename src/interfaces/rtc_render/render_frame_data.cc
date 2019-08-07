

#include "src/interfaces/rtc_render/render_frame_data.h"

namespace node_webrtc {

RenderFrameI420* RenderFrameData::toI420()
{
    return RenderFrameI420::Create(this);
}


 RenderFrameRGBA* RenderFrameData::toRBGA()
 {
    return RenderFrameRGBA::Create(this);
 }

}
