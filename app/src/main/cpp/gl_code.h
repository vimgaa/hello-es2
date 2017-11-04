
#ifndef HELLO_GL2_GL_CODE_H
#define HELLO_GL2_GL_CODE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define PI 3.1415926F
#define toRadius(angle) (angle * PI * 2.0f/360.0f)
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if(p) delete p; p = NULL;}
#endif
#define VERTEX_BUFFER_SIZE 3
#define TEX_BUFFER_SIZE 5
GLuint gProgram;
GLuint gvPositionHandle;
GLuint gvTexturehandle;
GLuint guTextureHandle;
GLuint guMVPMatrixHandle;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

float gWidth,gHeight;
int vBallCount = 0;// 顶点数量
int vCylinderCount = 0;
int vPlaneCount = 0;

const int geo_angleStep =10;
const int geo_vrange =180;
const int geo_hrange =180;
const int VERTEX_PER_SQUARE = 6;

float percentageX = 0.5f;
float percentageY = 0.5f;
float offX = 0.5f;
float offY = 0.5f;

#define calR(t) { 1 - t}
void* CylinderVertexBuffer = NULL;
void* CylinderTexcoordBuffer = NULL;
void* CylinderTexcoordBufferOut = NULL;
void* BallVertexBuffer = NULL;
void* BallTexcoordBuffer = NULL;
void* BallTexcoordBufferOut = NULL;
void* PlaneVertexBuffer = NULL;
void* PlaneTexcoordBuffer = NULL;

size_t BallVertexBufferSize;
size_t BallTexcoordBufferSize;
size_t CylinderVertexBufferSize;
size_t CylinderTexcoordBufferSize;
size_t PlanePlaneVertexBufferSize;
size_t PlaneTexcoordBufferSize;
GLuint _vertexBuffer[VERTEX_BUFFER_SIZE];
GLuint _textureBuffer[TEX_BUFFER_SIZE];
#define GEO_COORDS_PER_VERTEX 3
#define TEX_COORDS_PER_VERTEX 2
const int SIZE_OF_LOAT = sizeof(float);

struct Vertex{
    float x,y,z;
    float u,v;
    Vertex(){
        x=y=z=0;
    };
    Vertex(float _x,float _y ,float _z){
        x=_x;
        y=_y;
        z=_z;
    };

    Vertex(float phi, float theta)//Ball
    {
        float r=1.0f;
        x = (float)(r * sin(toRadius(theta)) * cos(toRadius(phi)));
        y = (float)(r * sin(toRadius(theta)) * sin(toRadius(phi)));
        z = (float)(r * cos(toRadius(theta)));
        float phi2 = (float)acos(y / r);//
        float theta2 = (float)atan2(z,x);

        u = (float)(0.5f + phi2 / PI * cos(theta2));
        v = (float)(0.5f - phi2 / PI * sin(theta2));
    };
    Vertex(float r,float h, float phi, float theta)//Cylinder
    {
        x = (float)(r * sin(toRadius(theta)) * cos(toRadius(phi)));
        y = (float)(r * sin(toRadius(theta)) * sin(toRadius(phi)));
        z = (float)(r * cos(toRadius(theta)));
        float phi2 = (float)acos(y / r);
        float theta2 = (float)atan2(z , x);

        u = (float)(0.5f + phi2 / PI * cos(theta2));
        v = (float)(0.5f - phi2 / PI * sin(theta2));
    }
    Vertex(float width)//Plane
    {
//        x = (float)(r * sin(toRadius(theta)) * cos(toRadius(phi)));
//        y = (float)(r * sin(toRadius(theta)) * sin(toRadius(phi)));
//        z = (float)(r * cos(toRadius(theta)));
//        float phi2 = (float)acos(y / x);
//        float theta2 = (float)atan2(z,x);
//
//        u = (float)(0.5f + phi2 / PI * cos(theta2));
//        v = (float)(0.5f - phi2 / PI * sin(theta2));
    }
    void push_back_vertex(float *buffer,int index, int stride)
    {
        buffer[index * stride + 0] = x;
        buffer[index * stride + 1] = y;
        buffer[index * stride + 2] = z;
    }
    void push_back_texcoord(float *buffer,int index, int stride)
    {
        buffer[index * stride + 0] = u;
        buffer[index * stride + 1] = v;
    }
    Vertex normalize() {
        float norm = (float) sqrt(x * x + y * y + z * z);
        if(norm == 0.0f) norm = 1.0f;
        return Vertex(x / norm, y / norm, z / norm);
    }
//    float* getFloatArray()
//    {
//        float array[3];
//        array[0] = x;
//        array[1] = y;
//        array[2] = z;
//        return array;
//    }
};

struct Ball{
    unsigned  int verterCount;
    Vertex point;
};

struct  Cylinder{
    unsigned  int verterCount;
    Vertex point;
};
inline  Vertex cross(Vertex u, Vertex v) {
    Vertex uv;
    uv.x = u.y * v.z - v.y * u.z;
    uv.y = u.z * v.x - v.z * u.x;
    uv.z = u.x * v.y - v.x * u.y;
    return uv;
}

inline Vertex add(Vertex u, Vertex v) {
    Vertex uv;
    uv.x = u.x + v.x;
    uv.y = u.y + v.y;
    uv.z = u.z + v.z;
    return uv;
}

inline Vertex mul(Vertex u, float k) {
    Vertex ku;
    ku.x = u.x * k;
    ku.y = u.y * k;
    ku.z = u.z * k;
    return ku;
}

inline void NormalizeVectorFLOAT_2(float *pvector)
{
    float normalizingConstant;
    normalizingConstant=1.0/sqrtf(pvector[0]*pvector[0]+pvector[1]*pvector[1]+pvector[2]*pvector[2]);
    pvector[0]*=normalizingConstant;
    pvector[1]*=normalizingConstant;
    pvector[2]*=normalizingConstant;
}
inline void ComputeNormalOfPlaneFLOAT_2(float *normal, const float *pvector1, const float *pvector2)
{
    normal[0]=(pvector1[1]*pvector2[2])-(pvector1[2]*pvector2[1]);
    normal[1]=(pvector1[2]*pvector2[0])-(pvector1[0]*pvector2[2]);
    normal[2]=(pvector1[0]*pvector2[1])-(pvector1[1]*pvector2[0]);
    //VC++ is able to better optimize (when it is turned on of course)
}
inline void MultiplyMatrices4by4OpenGL_FLOAT(float *result, float *matrix1, float *matrix2)
{
    result[0]=matrix1[0]*matrix2[0]+
              matrix1[4]*matrix2[1]+
              matrix1[8]*matrix2[2]+
              matrix1[12]*matrix2[3];
    result[4]=matrix1[0]*matrix2[4]+
              matrix1[4]*matrix2[5]+
              matrix1[8]*matrix2[6]+
              matrix1[12]*matrix2[7];
    result[8]=matrix1[0]*matrix2[8]+
              matrix1[4]*matrix2[9]+
              matrix1[8]*matrix2[10]+
              matrix1[12]*matrix2[11];
    result[12]=matrix1[0]*matrix2[12]+
               matrix1[4]*matrix2[13]+
               matrix1[8]*matrix2[14]+
               matrix1[12]*matrix2[15];

    result[1]=matrix1[1]*matrix2[0]+
              matrix1[5]*matrix2[1]+
              matrix1[9]*matrix2[2]+
              matrix1[13]*matrix2[3];
    result[5]=matrix1[1]*matrix2[4]+
              matrix1[5]*matrix2[5]+
              matrix1[9]*matrix2[6]+
              matrix1[13]*matrix2[7];
    result[9]=matrix1[1]*matrix2[8]+
              matrix1[5]*matrix2[9]+
              matrix1[9]*matrix2[10]+
              matrix1[13]*matrix2[11];
    result[13]=matrix1[1]*matrix2[12]+
               matrix1[5]*matrix2[13]+
               matrix1[9]*matrix2[14]+
               matrix1[13]*matrix2[15];

    result[2]=matrix1[2]*matrix2[0]+
              matrix1[6]*matrix2[1]+
              matrix1[10]*matrix2[2]+
              matrix1[14]*matrix2[3];
    result[6]=matrix1[2]*matrix2[4]+
              matrix1[6]*matrix2[5]+
              matrix1[10]*matrix2[6]+
              matrix1[14]*matrix2[7];
    result[10]=matrix1[2]*matrix2[8]+
               matrix1[6]*matrix2[9]+
               matrix1[10]*matrix2[10]+
               matrix1[14]*matrix2[11];
    result[14]=matrix1[2]*matrix2[12]+
               matrix1[6]*matrix2[13]+
               matrix1[10]*matrix2[14]+
               matrix1[14]*matrix2[15];

    result[3]=matrix1[3]*matrix2[0]+
              matrix1[7]*matrix2[1]+
              matrix1[11]*matrix2[2]+
              matrix1[15]*matrix2[3];
    result[7]=matrix1[3]*matrix2[4]+
              matrix1[7]*matrix2[5]+
              matrix1[11]*matrix2[6]+
              matrix1[15]*matrix2[7];
    result[11]=matrix1[3]*matrix2[8]+
               matrix1[7]*matrix2[9]+
               matrix1[11]*matrix2[10]+
               matrix1[15]*matrix2[11];
    result[15]=matrix1[3]*matrix2[12]+
               matrix1[7]*matrix2[13]+
               matrix1[11]*matrix2[14]+
               matrix1[15]*matrix2[15];
}
void glhTranslatef2(float *matrix, float x, float y, float z)
{
    matrix[12]=matrix[0]*x+matrix[4]*y+matrix[8]*z+matrix[12];
    matrix[13]=matrix[1]*x+matrix[5]*y+matrix[9]*z+matrix[13];
    matrix[14]=matrix[2]*x+matrix[6]*y+matrix[10]*z+matrix[14];
    matrix[15]=matrix[3]*x+matrix[7]*y+matrix[11]*z+matrix[15];
}
void _gluLookAt(float *matrix, float *eyePosition3D, float *center3D, float *upVector3D)
{
    float forward[3], side[3], up[3];
    float matrix2[16], resultMatrix[16];

    forward[0]=center3D[0]-eyePosition3D[0];
    forward[1]=center3D[1]-eyePosition3D[1];
    forward[2]=center3D[2]-eyePosition3D[2];
    NormalizeVectorFLOAT_2(forward);

    //Side = forward x up
    ComputeNormalOfPlaneFLOAT_2(side, forward, upVector3D);
    NormalizeVectorFLOAT_2(side);

    //Recompute up as: up = side x forward
    ComputeNormalOfPlaneFLOAT_2(up, side, forward);

    matrix2[0]=side[0];
    matrix2[4]=side[1];
    matrix2[8]=side[2];
    matrix2[12]=0.0;

    matrix2[1]=up[0];
    matrix2[5]=up[1];
    matrix2[9]=up[2];
    matrix2[13]=0.0;

    matrix2[2]=-forward[0];
    matrix2[6]=-forward[1];
    matrix2[10]=-forward[2];
    matrix2[14]=0.0;

    matrix2[3]=matrix2[7]=matrix2[11]=0.0;
    matrix2[15]=1.0;

    MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, matrix2);
    glhTranslatef2(resultMatrix, -eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);

    memcpy(matrix, resultMatrix, 16*sizeof(float));
}

Vertex cam_eye;
Vertex center;
Vertex cam_head;
static const GLfloat _vertices[] = {
        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f,
        1.0f,1.0f,0.0f,
        -1.0f,1.0f,0.0f
};
static const GLfloat texCoords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
};
void transByPoint(Vertex p) {
    Vertex x_axis = cross(cam_head, cam_eye).normalize();
    Vertex y_axis = cam_head;

    cam_eye = add(
            cam_eye,
            add(mul(x_axis, p.x),
                mul(y_axis, p.y)));
    if (cam_eye.y < 0.1f)
        cam_eye.y = 0.1f;
    cam_eye = cam_eye.normalize();

    double theta = acos(cam_eye.z);
    double phi = acos(cam_eye.x / sin(theta));

    cam_head = Vertex(
            (float) (sin(theta - PI / 2) * cos(phi)),
            (float) (sin(theta - PI / 2) * sin(phi)),
            (float) (cos(theta - PI / 2))).normalize();

//    LOGI( "%f %f %f %f %f", p.x, p.y, cam_eye.x, cam_eye.y, cam_eye.z);
}
#endif //HELLO_GL2_GL_CODE_H
