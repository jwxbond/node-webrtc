/* Copyright (c) 2019 The node-webrtc project authors. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be found
 * in the LICENSE.md file in the root of the source tree. All contributing
 * project authors may be found in the AUTHORS file in the root of the source
 * tree.
 */
#include "src/interfaces/rtc_render/rtc_render_video_source.h"

#include <webrtc/api/peer_connection_interface.h>
#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/api/video/video_frame.h>
#include <webrtc/rtc_base/ref_counted_object.h>

#include <libyuv.h> //rbga <-> i420

#include "src/converters.h"
#include "src/converters/absl.h"
#include "src/converters/arguments.h"
#include "src/converters/napi.h"
#include "src/dictionaries/webrtc/video_frame_buffer.h"
#include "src/functional/maybe.h"
#include "src/interfaces/media_stream_track.h"


#include "src/interfaces/rtc_render/render_frame_data.h"


namespace node_webrtc {

Napi::FunctionReference& RTCRenderVideoSource::constructor() {
  static Napi::FunctionReference constructor;
  return constructor;
}

RTCRenderVideoSource::RTCRenderVideoSource(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<RTCRenderVideoSource>(info) {
  New(info);
}

Napi::Value RTCRenderVideoSource::New(const Napi::CallbackInfo& info) {
  auto env = info.Env();

  if (!info.IsConstructCall()) {
    Napi::TypeError::New(env, "Use the new operator to construct an RTCRenderVideoSource.").ThrowAsJavaScriptException();
    return info.Env().Undefined();
  }

  CONVERT_ARGS_OR_THROW_AND_RETURN_NAPI(info, maybeInit, Maybe<RTCVideoSourceInit>)
  auto init = maybeInit.FromMaybe(RTCVideoSourceInit());

  auto needsDenoising = init.needsDenoising
  .Map([](auto needsDenoising) { return absl::optional<bool>(needsDenoising); })
  .FromMaybe(absl::optional<bool>());

  _source = new rtc::RefCountedObject<RTCRenderVideoTrackSource>(init.isScreencast, needsDenoising);

  return info.Env().Undefined();
}

Napi::Value RTCRenderVideoSource::CreateTrack(const Napi::CallbackInfo&) {
  // TODO(mroberts): Again, we have some implicit factory we are threading around. How to handle?
  auto factory = PeerConnectionFactory::GetOrCreateDefault();
  auto track = factory->factory()->CreateVideoTrack(rtc::CreateRandomUuid(), _source);
  return MediaStreamTrack::wrap()->GetOrCreate(factory, track)->Value();
}

Napi::Value RTCRenderVideoSource::OnFrame(const Napi::CallbackInfo& info) {
  // CONVERT_ARGS_OR_THROW_AND_RETURN_NAPI(info, buffer, rtc::scoped_refptr<webrtc::I420Buffer>)
  // webrtc::VideoFrame::Builder builder;
  // auto frame = builder.set_video_frame_buffer(buffer).build();

  //TODO frame I420Buffer format



  //fetch I420Buffer
  //   libyuv::ABGRToI420(
  //     rgbaFrame.dataRgba(),
  //     rgbaFrame.strideRgba(),
  //     i420Frame.dataY(),
  //     i420Frame.strideY(),
  //     i420Frame.dataU(),
  //     i420Frame.strideU(),
  //     i420Frame.dataV(),
  //     i420Frame.strideV(),
  //     rgbaFrame.width(),
  //     rgbaFrame.height()
  // );

  // webrtc::VideoFrame::Builder builder;
  // webrtc::VideoFrame frame = builder.set_video_frame_buffer(buffer).build();

  // _source->PushFrame(frame);
  return info.Env().Undefined();
}

//Exec Input Operation
Napi::Value RTCRenderVideoSource::ExecOperation(const Napi::CallbackInfo& info) {

  return info.Env().Undefined();
}

Napi::Value RTCRenderVideoSource::GetNeedsDenoising(const Napi::CallbackInfo& info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), _source->needs_denoising(), result, Napi::Value)
  return result;
}

Napi::Value RTCRenderVideoSource::GetIsScreencast(const Napi::CallbackInfo& info) {
  CONVERT_OR_THROW_AND_RETURN_NAPI(info.Env(), _source->is_screencast(), result, Napi::Value)
  return result;
}

void RTCRenderVideoSource::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "RTCRenderVideoSource", {
    InstanceMethod("createTrack", &RTCRenderVideoSource::CreateTrack),
    InstanceMethod("onFrame", &RTCRenderVideoSource::OnFrame),
    //TODO执行命令的操作暂时放这里
    InstanceMethod("execOperation", &RTCRenderVideoSource::ExecOperation),
    InstanceAccessor("needsDenoising", &RTCRenderVideoSource::GetNeedsDenoising, nullptr),
    InstanceAccessor("isScreencast", &RTCRenderVideoSource::GetIsScreencast, nullptr)
  });

  constructor() = Napi::Persistent(func);
  constructor().SuppressDestruct();

  exports.Set("RTCRenderVideoSource", func);
}

}  // namespace node_webrtc
