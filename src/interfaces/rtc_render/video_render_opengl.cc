#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


#include <stdio.h>
#include <Math.h> 
#include "src/interfaces/rtc_render/video_render_opengl.h"


#define PI 3.14159265f


namespace node_webrtc {


VideoRenderOpenGL::VideoRenderOpenGL(const char* renderId):
    mRenderId(renderId), 
    mOpenGLInited(false)
{

}


VideoRenderOpenGL::~VideoRenderOpenGL()
{

}

const char* VideoRenderOpenGL::CurrentRenderId()
{
    return mRenderId;
}


int32_t VideoRenderOpenGL::StartRender()
{
    printf("[VideoRenderOpenGL] StartRender..\n");
    //TODO启动线程OpenGL渲染
    InitOpenGLEnvironment(500, 500);
    ExecRenderTask(0);
    return -1;
}


int32_t VideoRenderOpenGL::StopRender()
{
    printf("[VideoRenderOpenGL] StopRender..\n");
    //TODO停止线程OpenGL渲染
    return -1;
}


webrtc::VideoFrameBuffer* VideoRenderOpenGL::GetCurrentFrameBuffer()
{

    return nullptr;
}



//for test
void VideoRenderOpenGL::InitOpenGLEnvironment(int width, int height)
{
    if( !mOpenGLInited )
    {
        mOpenGLInited = true;
    }

    mWidth = width;
    mHeight = height;


    glViewport(0, 0, mWidth, mHeight);

    //1. create frame buffer 
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	printf("[VideoRenderOpenGL] glBindFramebuffer()\n");
    

    //2. create texture as color attachment0
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	printf("[VideoRenderOpenGL] glTexImage2D()\n");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //3. Attach the texture to the framebuffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if( status != GL_FRAMEBUFFER_COMPLETE )
    {
	    printf("[VideoRenderOpenGL] Error glCheckFramebufferStatus()=%d\n", status);
        return;
    }

    glClearColor(0.0, 0, 0, 0.4);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void VideoRenderOpenGL::ExecRenderTask(int taskId)
{
    printf("[VideoRenderOpenGL] ExecRenderTask..taskId=%d\n", taskId);
    if( !mOpenGLInited )
    {
        printf("[VideoRenderOpenGL] mOpenGLInited = false");
        return;
    }

    //for test
    //2. do task with taskId

    switch (taskId)
    {
    case 0: //rectangle
        glBegin(GL_QUADS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(-0.5f, -0.5f);    // x, y
            glVertex2f( 0.5f, -0.5f);
            glVertex2f( 0.5f,  0.5f);
            glVertex2f(-0.5f,  0.5f);
        glEnd();
        break;
    
    case 1: //triangle
        glBegin(GL_TRIANGLES);// Each set of 3 vertices form a triangle
            glColor3f(0.0f, 0.0f, 1.0f); // Blue
            glVertex2f(0.1f, -0.6f);
            glVertex2f(0.7f, -0.6f);
            glVertex2f(0.4f, -0.1f);
        
            glColor3f(1.0f, 0.0f, 0.0f); // Red
            glVertex2f(0.3f, -0.4f);
            glColor3f(0.0f, 1.0f, 0.0f); // Green
            glVertex2f(0.9f, -0.4f);
            glColor3f(0.0f, 0.0f, 1.0f); // Blue
            glVertex2f(0.6f, -0.9f);
        glEnd();
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
        break;
    }
    case 4: //image
        break;

    default:

        break;
    }
}

}
