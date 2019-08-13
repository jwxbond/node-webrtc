
#include <stdio.h>
#include <stdlib.h>
#include <Math.h> 
#include "src/interfaces/rtc_render/video_render_opengl.h"
#include "src/interfaces/rtc_render/render_frame_data.h"
#include "src/interfaces/rtc_render/video_render_gl_context.h"

#define PI 3.14159265f

#define R_GL_ERROR(func)  /*(printf("[VideoRenderOpenGL][%s],error=%d\n", func, glGetError()))*/

namespace node_webrtc {


VideoRenderOpenGL::VideoRenderOpenGL(const char* renderId):
    mRenderId(renderId), 
    mOpenGLInited(false)
{
    mWidth = 320;
    mHeight = 160;
}

VideoRenderOpenGL::VideoRenderOpenGL(const char* renderId, int width, int height):
    mRenderId(renderId), 
    mOpenGLInited(false),
    mWidth(width),
    mHeight(height)
{

}


VideoRenderOpenGL::~VideoRenderOpenGL()
{
    DestroyOpenGLEnvironment();
}

const char* VideoRenderOpenGL::CurrentRenderId()
{
    return mRenderId;
}


int32_t VideoRenderOpenGL::StartRender()
{
    printf("[VideoRenderOpenGL] StartRender..\n");
    bool ret = InitOpenGLEnvironment();
    if( ret )
    {
        ExecRenderTask(0);
        return 0;
    }
    return -1;
}


int32_t VideoRenderOpenGL::StopRender()
{
    printf("[VideoRenderOpenGL] StopRender..\n");
    return 0;
}



//for test index
static int staticTimeCount = 0;
webrtc::VideoFrame VideoRenderOpenGL::GetCurrentFrameBuffer()
{
    staticTimeCount++;
    if( staticTimeCount % 60 == 0  ){
        mCurrentTaskId++;
        int taskId = mCurrentTaskId % 4;
        ExecRenderTask(taskId);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO); 
    glBindTexture(GL_TEXTURE_2D, mTextureID); 

    size_t pixelLength = static_cast<size_t>(mWidth * mHeight * 4); 
    GLubyte *pixels = new GLubyte[pixelLength];
    glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);    

    GLenum error = glGetError();
    if ( error != 0 )
    {
        printf("[VideoRenderOpenGL] GetCurrentFrameBuffer glGetTexImage() error=%d\n", error);
    }
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    rtc::scoped_refptr<webrtc::I420Buffer> buffer = RenderFrameData::CreateI420FrameWithRenderBuffer(mWidth, mHeight, pixels, pixelLength);
    webrtc::VideoFrame::Builder builder;
    webrtc::VideoFrame frame = builder.set_video_frame_buffer(buffer).build();

    if( nullptr != pixels )
    {
        delete pixels;
        pixels = nullptr;
    }

    return frame;
}


//for test
bool VideoRenderOpenGL::InitOpenGLEnvironment()
{
    printf("[VideoRenderOpenGL] InitOpenGLEnvironment Start..\n");

    if( !mOpenGLInited )
    {
        mOpenGLInited = true;
    }

    printf("[VideoRenderOpenGL] Init GL context ..\n");
    RenderGLContext::Init();

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_POINT_SMOOTH,GL_NICEST);
    glEnable(GL_POINT_SMOOTH);

    glHint(GL_LINE_SMOOTH,GL_NICEST);
    glEnable(GL_LINE_SMOOTH);

    glHint(GL_POLYGON_SMOOTH,GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);


    glViewport(0, 0, mWidth, mHeight);
    printf("[VideoRenderOpenGL] glViewport(0,0,%d, %d)\n", mWidth, mHeight); R_GL_ERROR("glViewport");


    //texture
    glGenTextures(1, &mTextureID);
    printf("[VideoRenderOpenGL] glGenTextures=%d\n", mTextureID); R_GL_ERROR("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, mTextureID); R_GL_ERROR("glBindTexture");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mTextureID);

    printf("[VideoRenderOpenGL] glTexImage2D()\n"); R_GL_ERROR("glTexImage2D");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);R_GL_ERROR("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);R_GL_ERROR("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);R_GL_ERROR("glTexParameteri");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);R_GL_ERROR("glTexParameteri");

    //depthBuffer
    glGenRenderbuffers(1, &mDepthBuffer); R_GL_ERROR("glGenRenderbuffers");
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer); R_GL_ERROR("glBindRenderbuffer");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
    printf("[VideoRenderOpenGL] depthBuffer, glGenRenderbuffers()=%d\n", mDepthBuffer); R_GL_ERROR("glRenderbufferStorage");

    //frame buffer 
	glGenFramebuffers(1, &mFBO); R_GL_ERROR("glGenFramebuffers");
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	printf("[VideoRenderOpenGL] glGenFramebuffers()=%d\n", mFBO); R_GL_ERROR("glBindFramebuffer");
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0); R_GL_ERROR("glFramebufferTexture2D");
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);  R_GL_ERROR("glFramebufferRenderbuffer");
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); R_GL_ERROR("glCheckFramebufferStatus");
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
	    printf("[VideoRenderOpenGL] Error glCheckFramebufferStatus()=%d\n", status);
        return false;
    }
    
    printf("[VideoRenderOpenGL] InitOpenGLEnvironment Finished..\n");
    return true;
}

void VideoRenderOpenGL::DestroyOpenGLEnvironment()
{
    if( mTextureID > 0 )
    {
        glDeleteTextures(1, &mTextureID);
    }

    if( mDepthBuffer )
    {
        glDeleteFramebuffers(1, &mDepthBuffer); 
    }

    if( mFBO > 0 )
    {
        glDeleteFramebuffers(1, &mFBO);
    }
    
    RenderGLContext::Destroy();
}

void VideoRenderOpenGL::ExecRenderTask(int taskId)
{
    mCurrentTaskId = taskId;

    printf("[VideoRenderOpenGL] ExecRenderTask..taskId=%d\n", taskId);
    if( !mOpenGLInited )
    {
        printf("[VideoRenderOpenGL] mOpenGLInited = false\n");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO); R_GL_ERROR("glBindFramebuffer");
    glBindTexture(GL_TEXTURE_2D, mTextureID); R_GL_ERROR("glBindTexture");
    
    //for test
    //2. do task with taskId
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); R_GL_ERROR("glClearColor");
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); R_GL_ERROR("glClear");




    switch (taskId)
    {
    case 0: //quads
        glBegin(GL_QUADS); R_GL_ERROR("draw QUADS glBegin(GL_QUADS) ");
            glColor3f(1.0f, 0.0f, 0.0f); R_GL_ERROR("draw rectangle glColor3f() ");
            glVertex2f(-0.5f, -0.5f); R_GL_ERROR("draw rectangle glVertex2f(-0.5f, -0.5f) ");
            glVertex2f( 0.5f, -0.5f); R_GL_ERROR("draw rectangle glVertex2f(0.5f, -0.5f) ");
            glVertex2f( 0.5f,  0.5f); R_GL_ERROR("draw rectangle glVertex2f(0.5f,  0.5f) ");
            glVertex2f(-0.5f,  0.5f); R_GL_ERROR("draw rectangle glVertex2f(-0.5f,  0.5f) ");
        glEnd();  R_GL_ERROR("draw rectangle glEnd() ");
        break;
    case 1: //triangle
        glBegin(GL_TRIANGLES);           R_GL_ERROR("draw TRIANGLES glBegin(GL_TRIANGLES) ");
            glColor3f(0.0f, 0.0f, 1.0f); R_GL_ERROR("draw triangle glColor3f() "); // Blue
            glVertex2f(0.1f, -0.6f);     R_GL_ERROR("draw triangle glVertex2f(0.1f, -0.6f)");
            glVertex2f(0.7f, -0.6f);     R_GL_ERROR("draw triangle glVertex2f(0.7f, -0.6f)");
            glVertex2f(0.4f, -0.1f);     R_GL_ERROR("draw triangle glVertex2f(0.4f, -0.1f)");
        
            glColor3f(1.0f, 0.0f, 0.0f); R_GL_ERROR("draw triangle glColor3f(1.0f, 0.0f, 0.0f)");// Red
            glVertex2f(0.3f, -0.4f);     R_GL_ERROR("draw triangle glVertex2f(0.3f, -0.4f)");
            glColor3f(0.0f, 1.0f, 0.0f); R_GL_ERROR("draw triangle glColor3f(0.0f, 1.0f, 0.0f)");// Green
            glVertex2f(0.9f, -0.4f);     R_GL_ERROR("draw triangle glVertex2f(0.9f, -0.4f)");
            glColor3f(0.0f, 0.0f, 1.0f); R_GL_ERROR("draw triangle glColor3f(0.0f, 0.0f, 1.0f)");// Blue
            glVertex2f(0.6f, -0.9f);     R_GL_ERROR("draw triangle glVertex2f(0.6f, -0.9f)");
        glEnd();                         R_GL_ERROR("draw triangle glEnd()");
        printf("[VideoRenderOpenGL] draw triangle, error=%d\n", glGetError());

        break;

    case 2: //polygon
        glBegin(GL_POLYGON);
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
            glVertex2f(0.4f, 0.2f);
            glVertex2f(0.6f, 0.2f);
            glVertex2f(0.7f, 0.4f);
            glVertex2f(0.6f, 0.6f);
            glVertex2f(0.4f, 0.6f);
            glVertex2f(0.3f, 0.4f);
        glEnd();
        printf("[VideoRenderOpenGL] draw polygon, error=%d\n", glGetError());
        break;
    case 3: //circle
    {
        float ballRadius = 0.5;
        glBegin(GL_TRIANGLE_FAN);
            glColor3f(0.0f, 0.0f, 1.0f);  // Blue
            glVertex2f(0.0f, 0.0f);       // Center of circle
            int numSegments = 100;
            GLfloat angle;
            for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
                angle = i * 2.0f * PI / numSegments;  // 360 deg for all segments
                glVertex2f(cos(angle) * ballRadius, sin(angle) * ballRadius);
            }
        glEnd();
        printf("[VideoRenderOpenGL] draw circle, error=%d\n", glGetError());
        break;
    }
    case 4: //image
        break;

    default:

        break;
    }

    glFlush();
    printf("[VideoRenderOpenGL] glFlush(), error=%d\n", glGetError());
}

}
