
#include "gl_code.h"
void Ball(float geo_r)
{
    int vertex_idx = 0;
    int texcoord_idx = 0;
    vBallCount = geo_vrange / geo_angleStep * geo_hrange / geo_angleStep * VERTEX_PER_SQUARE;
    float * verticesList = new float[vBallCount * GEO_COORDS_PER_VERTEX];
    float * texList =new float[vBallCount * TEX_COORDS_PER_VERTEX];
    for (int theta = 0; theta < geo_vrange; theta = theta + geo_angleStep)
    {
        for (int phi = 0; phi < geo_hrange; phi = phi + geo_angleStep)
        {
            Vertex v[4];
            v[0] = Vertex( phi, theta);
            v[1] = Vertex( phi + geo_angleStep, theta);
            v[2] = Vertex( phi + geo_angleStep, theta + geo_angleStep);
            v[3] = Vertex( phi, theta + geo_angleStep);

            v[0].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[0].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);
            v[1].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[1].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);
            v[3].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[3].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);

            v[1].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[1].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);
            v[2].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[2].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);
            v[3].push_back_vertex(verticesList, vertex_idx++, GEO_COORDS_PER_VERTEX);v[3].push_back_texcoord(texList, texcoord_idx++, TEX_COORDS_PER_VERTEX);

        }
    }
    BallVertexBuffer = verticesList;
    BallVertexBufferSize = (size_t)vBallCount * GEO_COORDS_PER_VERTEX * SIZE_OF_LOAT;
    BallTexcoordBuffer = texList;
    BallTexcoordBufferSize= (size_t)vBallCount * TEX_COORDS_PER_VERTEX* SIZE_OF_LOAT;
}

void Cylinder(float height, float r, float splitAngle, float splitHeight)
{
    int BufferSize = (int)((height / splitHeight + 1) * 360.0f / splitAngle * 20.0f);
    vCylinderCount = 0;//
    void* Buffer = malloc(BufferSize);
    int bufPtr = 0;
    for (float currentHeight = 0.0f; currentHeight <= height; currentHeight += splitHeight)
    {
        for (float angle = 0.0f; angle < 360.0f; angle += splitAngle)
        {
            *(float*)((char*)Buffer + bufPtr) = (float)cos(toRadius(angle)) * r; bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = (float)sin(toRadius(angle)) * r; bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = currentHeight;			  bufPtr += 4;

            float u = (float)cos(toRadius(angle)) * percentageX * (currentHeight / height) + offX;
            float v=(float)sin(toRadius(angle)) * percentageY * (currentHeight / height) + offY;

            *(float*)((char*)Buffer + bufPtr) = u;bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = v; bufPtr += 4;
        }
    }
    int rowCount = (int)(height / splitHeight + 1) * 3 /4;
    int col_Count = (int)(360 / splitAngle);
    int startRowCount = rowCount / 4;
    int tRowCount = (int)(height / splitHeight + 1) - 10;
    SAFE_DELETE(CylinderVertexBuffer);
    SAFE_DELETE(CylinderTexcoordBuffer);
    CylinderVertexBufferSize = (size_t)((rowCount) * (col_Count) * 12 * 7.2);
    CylinderVertexBuffer = malloc(CylinderVertexBufferSize);
    int vertexbufPtr = 0;
    CylinderTexcoordBufferSize = (size_t)((rowCount) * (col_Count) * 8 * 7.2);
    CylinderTexcoordBuffer = malloc(CylinderTexcoordBufferSize);
    CylinderTexcoordBufferOut = malloc(CylinderTexcoordBufferSize);
    int texcoordbufPtr = 0;


    for (int row_index = startRowCount; row_index <= tRowCount; row_index++)
    {
        if (row_index != tRowCount - 1)//不是最后一行
        {
            for (int col_index = 0; col_index < col_Count; col_index++)
            {
                int nextRowPos = row_index * col_Count + col_index;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5]; vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 2];    vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[(nextRowPos + col_Count) * 5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 4];
                texcoordbufPtr += 4;

                int next_pos = nextRowPos + 1;
                if (col_index == col_Count - 1) {
                    next_pos = (row_index)* col_Count;
                }
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5]; vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5 + 2];    vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[next_pos * 5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[next_pos * 5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[next_pos * 5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[next_pos * 5 + 4];
                texcoordbufPtr += 4;

                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[nextRowPos * 5]; vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[nextRowPos * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[nextRowPos * 5 + 2];    vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[nextRowPos * 5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[nextRowPos * 5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[nextRowPos * 5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[nextRowPos * 5 + 4];
                texcoordbufPtr += 4;
            }
        }
        if (row_index != startRowCount)// 不是第一行
        {
            for (int col_index = 0; col_index < col_Count; col_index++)
            {
                int preRowPos = row_index * col_Count + col_index;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5]; vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 2];    vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[(preRowPos - col_Count) * 5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 4];
                texcoordbufPtr += 4;

                int prePos = preRowPos - 1;
                if (col_index == 0) {
                    prePos = row_index * col_Count + col_Count - 1;
                }
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5]; vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5 + 2];    vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[prePos * 5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[prePos * 5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[prePos * 5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[prePos * 5 + 4];
                texcoordbufPtr += 4;

                //int pos = (row_index * col_Count + col_index) * 5;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[preRowPos *5];           vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[preRowPos *5 + 1];       vertexbufPtr += 4;
                *(float *)((char *)CylinderVertexBuffer + vertexbufPtr) = ((float *)Buffer)[preRowPos *5 + 2];       vertexbufPtr += 4; vCylinderCount++;

                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[preRowPos *5 + 3];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = calR(((float *)Buffer)[preRowPos *5 + 3]);
                texcoordbufPtr += 4;
                *(float *)((char *)CylinderTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[preRowPos *5 + 4];
                *(float *)((char *)CylinderTexcoordBufferOut + texcoordbufPtr) = ((float *)Buffer)[preRowPos *5 + 4];
                texcoordbufPtr += 4;
            }
        }
    }
//    vCylinderCount = (rowCount - 1) * col_Count * 3 * 2;
}

void Plane(float width, float height, float splitLength)
{
    int BufferSize = (int)((width / splitLength + 1) * ((height / splitLength + 1) * 20.0f));
    void* Buffer = malloc(BufferSize);
    int bufPtr = 0;
    vPlaneCount = 0;
    float splitAngle = (splitLength / width * 360.0f);
    for (float currentHeight = 0.0f; currentHeight <= height; currentHeight += splitLength)
    {
        for (float angle = -180.0f; angle <= 180.0f; angle += splitAngle)
        {
            *(float*)((char*)Buffer + bufPtr) = angle / 360.0f * width; bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = currentHeight; bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = 0;			  bufPtr += 4;

            *(float*)((char*)Buffer + bufPtr) = (float)cos(toRadius(angle)) * percentageX * (currentHeight / height) + offX; bufPtr += 4;
            *(float*)((char*)Buffer + bufPtr) = (float)sin(toRadius(angle)) * percentageY * (currentHeight / height) + offY; bufPtr += 4;
        }
    }
    int rowCount = (int)(height / splitLength + 1) * 3 /4;
    int col_Count = (int)(360 / splitAngle + 1);

    int startRowCount = rowCount /4;
    int endRowCount= (int)(height / splitLength + 1) - 6;
    SAFE_DELETE(PlaneVertexBuffer);
    SAFE_DELETE(PlaneTexcoordBuffer);
    PlanePlaneVertexBufferSize = (size_t)((rowCount - 1)* (col_Count - 1) * 12 * 6);
    PlaneVertexBuffer = malloc(PlanePlaneVertexBufferSize);
    int vertexbufPtr = 0;
    PlaneTexcoordBufferSize = (size_t)((rowCount - 1)* (col_Count - 1) * 8 * 6);
    PlaneTexcoordBuffer = malloc(PlaneTexcoordBufferSize);
    int texcoordbufPtr = 0;

    for (int row_index = startRowCount ; row_index < endRowCount; row_index++)
    {
        if (row_index != endRowCount - 1)//不是最后一行
        {
            for (int col_index = 0; col_index < col_Count - 1; col_index++)
            {
                int nextRowPos = row_index * col_Count + col_index;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5]; vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 2];    vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(nextRowPos + col_Count) * 5 + 4];    texcoordbufPtr += 4;

                int next_pos = nextRowPos + 1;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5]; vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[next_pos * 5 + 2];    vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[next_pos * 5 + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[next_pos * 5 + 4];    texcoordbufPtr += 4;

                int pos = (row_index * col_Count + col_index) * 5;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos]; vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos + 1];   vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos + 2];    vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[pos + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[pos + 4];    texcoordbufPtr += 4;

            }
        }
        if (row_index != startRowCount)// 不是第一行
        {
            for (int col_index = 1; col_index < col_Count; col_index++)
            {
                int preRowPos = row_index * col_Count + col_index;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5]; vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 2];    vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[(preRowPos - col_Count) * 5 + 4];    texcoordbufPtr += 4;

                int prePos = preRowPos - 1;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5]; vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5 + 1];   vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[prePos * 5 + 2];    vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[prePos * 5 + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[prePos * 5 + 4];    texcoordbufPtr += 4;

                int pos = (row_index * col_Count + col_index) * 5;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos];           vertexbufPtr += 4; vPlaneCount++;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos + 1];       vertexbufPtr += 4;
                *(float *)((char *)PlaneVertexBuffer + vertexbufPtr) = ((float *)Buffer)[pos + 2];       vertexbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[pos + 3];   texcoordbufPtr += 4;
                *(float *)((char *)PlaneTexcoordBuffer + texcoordbufPtr) = ((float *)Buffer)[pos + 4];    texcoordbufPtr += 4;
            }
        }
    }
//    vPlaneCount = (int)((rowCount - 1)*(col_Count - 1) * 3 * 2);//每个点使用6次
}

void setupBuffer(GLuint buffer,void* pointer,size_t size){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, pointer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
bool setupGraphics(int w, int h) {
    glGenBuffers(VERTEX_BUFFER_SIZE, _vertexBuffer);
    glGenBuffers(TEX_BUFFER_SIZE, _textureBuffer);

    setupBuffer(_vertexBuffer[0],BallVertexBuffer,BallVertexBufferSize);
    setupBuffer(_vertexBuffer[1],CylinderVertexBuffer,CylinderVertexBufferSize);
    setupBuffer(_vertexBuffer[2],PlaneVertexBuffer,PlanePlaneVertexBufferSize);

    setupBuffer(_textureBuffer[0],BallTexcoordBuffer,BallTexcoordBufferSize);
    setupBuffer(_textureBuffer[1],CylinderTexcoordBuffer,CylinderTexcoordBufferSize);
    setupBuffer(_textureBuffer[2],PlaneTexcoordBuffer,PlaneTexcoordBufferSize);
    setupBuffer(_textureBuffer[3],BallTexcoordBufferOut,BallTexcoordBufferSize);
    setupBuffer(_textureBuffer[4],CylinderTexcoordBufferOut,CylinderTexcoordBufferSize);
    checkGlError("glBufferData");

    gvPositionHandle = (GLuint)glGetAttribLocation(gProgram, "vPosition");
    gvTexturehandle = (GLuint)glGetAttribLocation(gProgram,"vTexCoordinate");
    guTextureHandle = (GLuint)glGetAttribLocation(gProgram,"u_texture");
    guMVPMatrixHandle = (GLuint)glGetUniformLocation(gProgram,"u_MVPMatrix");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);
    gWidth =w;gHeight = h;
    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}

bool init(int width,int height,char* vertex,char* frag) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// 黑色背景
    glClearDepthf(1.0f);// 设置深度缓存
    glEnable(GL_DEPTH_TEST);   // 启用深度测试
    glDepthFunc(GL_LEQUAL);// 所作深度测试的类型
    checkGlError("glDepthFunc");

    gProgram = createProgram(vertex, frag);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    if(!BallVertexBuffer)
        Ball(1);
    if(!CylinderVertexBuffer)
        Cylinder(1.5,1.2,1,0.001);
    if(!PlaneVertexBuffer)
        Plane(40,10,1.0f/9);
    setupGraphics(width,height);

    return true;
}

void renderGeometry(int texture,GLuint vertexBuffer,GLuint textureBuffer)
{
    glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer( GL_ARRAY_BUFFER, _textureBuffer[0] );
    glVertexAttribPointer(gvTexturehandle, 2, GL_FLOAT, GL_FALSE,0, NULL );
    glDrawArrays(GL_TRIANGLES, 0, vBallCount);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
void renderFrame(int gType, int gTexture, float* mvpMatrix) {
    static float grey;
    grey = 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(gProgram);
    checkGlError("glUseProgram");
    glActiveTexture(GL_TEXTURE0);
    checkGlError("glActiveTexture");
//    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTexture);
    checkGlError("glBindTexture");
//vertex
    glBindBuffer( GL_ARRAY_BUFFER, _vertexBuffer[0] );
    glEnableVertexAttribArray(gvPositionHandle);
    glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    checkGlError("glVertexAttribPointer1");
//textureCoordinate
    glBindBuffer( GL_ARRAY_BUFFER, _textureBuffer[0] );
    glEnableVertexAttribArray(gvTexturehandle);
    checkGlError("glEnableVertexAttribArray");
    glVertexAttribPointer(gvTexturehandle, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    checkGlError("glVertexAttribPointer2");

    glUniform1i(guTextureHandle,1);
    checkGlError("glUniform1i");
    float buffer[16]={1.0,0.0,0.0,0.0,
                      0.0,1.0,0.0,0.0,
                      0.0,0.0,1.0,0.0,
                      0.0,0.0,0.0,1.0};
    glUniformMatrix4fv(guMVPMatrixHandle, 1, GL_FALSE, mvpMatrix);
    checkGlError("glUniformMatrix4fv");
//    LOGE("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f ",mvpMatrix[0],mvpMatrix[1],mvpMatrix[2],mvpMatrix[3],
// mvpMatrix[4],mvpMatrix[5],mvpMatrix[6],mvpMatrix[7],mvpMatrix[8],mvpMatrix[9],mvpMatrix[10],mvpMatrix[11],mvpMatrix[12],mvpMatrix[13],mvpMatrix[14],mvpMatrix[15]);
//

    glDrawArrays(GL_TRIANGLES, 0, vBallCount);
    checkGlError("glDrawArrays");
}
void destroy()
{
    SAFE_DELETE(BallVertexBuffer);
    SAFE_DELETE(BallTexcoordBuffer);
    SAFE_DELETE(CylinderVertexBuffer);
    SAFE_DELETE(CylinderTexcoordBuffer);
    SAFE_DELETE(PlaneVertexBuffer);
    SAFE_DELETE(PlaneTexcoordBuffer);
    SAFE_DELETE(BallTexcoordBufferOut);
    SAFE_DELETE(CylinderTexcoordBufferOut);
    glDeleteBuffers( VERTEX_BUFFER_SIZE, _vertexBuffer );
    glDeleteBuffers( TEX_BUFFER_SIZE, _textureBuffer );
}
void resize(int width, int height)
{
    if (height==0)// 防止被零除
    {
        height=1;
    }
    gWidth = width;
    gHeight = height;

    glViewport(0, 0, width, height);// 重置当前的视口

//    _gluLookAt(viewMatrix.getFloatArray(),cam_eye.getFloatArray(),center.getFloatArray(),cam_head.getFloatArray());

}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
    {
//        setupGraphics(width, height);
    }

    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jclass type,jint gType, jint gTexture,jfloatArray mvpMatrix)
    {
        jboolean is_copy=0;
        jfloat* data_ptr=(env)->GetFloatArrayElements(mvpMatrix,&is_copy);
//        int n=(env)->GetArrayLength(mvpMatrix);
        renderFrame(gType, gTexture,data_ptr);
        (env)->ReleaseFloatArrayElements(mvpMatrix,data_ptr,0);

    }
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_initBuffer(JNIEnv * env, jclass type,jint width, jint height,jstring vertex,jstring frag)
    {
        jboolean is_copy = 0;
        char* vertex_utf8 = (char*) env->GetStringUTFChars(vertex, &is_copy);
        char* frag_utf8 = (char*) env->GetStringUTFChars(frag, &is_copy);
        init(width,height,vertex_utf8,frag_utf8);
        env->ReleaseStringUTFChars(vertex, vertex_utf8);
        env->ReleaseStringUTFChars(frag, frag_utf8);
    }
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_destroy(JNIEnv * env, jclass type)
    {
        destroy();
    }
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_transByPoint(JNIEnv * env, jclass type,  jfloat x, jfloat y,jfloat z)
    {
        transByPoint(Vertex(x,y,z));
    }
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_resize(JNIEnv * env, jclass type,  jint width, jint height)
    {
        resize(width, height);
    }
};