/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.gl2jni;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.PointF;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.GLUtils;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import com.android.gl2jni.SphereMath.Point3F;
import  com.android.gl2jni.RawResourceReader.*;

class GL2JNIView extends GLSurfaceView {
    private static String TAG = "GL2JNIView";
    private static final boolean DEBUG = false;
    public Renderer renderer;
    public ScaleGestureDetector mScaleDetector;
    public GL2JNIView(Context context) {
        super(context);
        init(context, false, 0, 0);
        this.mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
    }
    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            renderer.scaleByFloat(detector.getScaleFactor());
//            Log.i(TAG, "onScale: detector"+detector.getScaleFactor());
            requestRender();
            return true;
        }
    }
//    public GL2JNIView(Context context, boolean translucent, int depth, int stencil) {
//        super(context);
//        init(translucent, depth, stencil);
//    }

    private void init(Context context,boolean translucent, int depth, int stencil) {

        if (translucent) {
            this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }

        setEGLContextFactory(new ContextFactory());

        setEGLConfigChooser( translucent ?
                             new ConfigChooser(8, 8, 8, 8, depth, stencil) :
                             new ConfigChooser(5, 6, 5, 0, depth, stencil) );

        /* Set the renderer responsible for frame rendering */
        renderer = new Renderer(context);
        setRenderer(renderer);
    }

    private static class ContextFactory implements GLSurfaceView.EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            Log.w(TAG, "creating OpenGL ES 2.0 context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
            EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            Log.e(TAG, String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser {

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs2 =
        {
            EGL10.EGL_RED_SIZE, 4,
            EGL10.EGL_GREEN_SIZE, 4,
            EGL10.EGL_BLUE_SIZE, 4,
            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL10.EGL_NONE
        };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {

            /* Get the number of minimally matching EGL configurations
             */
            int[] num_config = new int[1];
            egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No configs match configSpec");
            }

            /* Allocate then read the array of minimally matching EGL configs
             */
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

            if (DEBUG) {
                 printConfigs(egl, display, configs);
            }
            /* Now return the "best" one
             */
            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                EGLConfig[] configs) {
            for(EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);

                // We need at least mDepthSize and mStencilSize bits
                if (d < mDepthSize || s < mStencilSize)
                    continue;

                // We want an *exact* match for red/green/blue/alpha
                int r = findConfigAttrib(egl, display, config,
                        EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config,
                            EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config,
                            EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config,
                        EGL10.EGL_ALPHA_SIZE, 0);

                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
                    return config;
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        private void printConfigs(EGL10 egl, EGLDisplay display,
            EGLConfig[] configs) {
            int numConfigs = configs.length;
            Log.w(TAG, String.format("%d configurations", numConfigs));
            for (int i = 0; i < numConfigs; i++) {
                Log.w(TAG, String.format("Configuration %d:\n", i));
                printConfig(egl, display, configs[i]);
            }
        }

        private void printConfig(EGL10 egl, EGLDisplay display,
                EGLConfig config) {
            int[] attributes = {
                    EGL10.EGL_BUFFER_SIZE,
                    EGL10.EGL_ALPHA_SIZE,
                    EGL10.EGL_BLUE_SIZE,
                    EGL10.EGL_GREEN_SIZE,
                    EGL10.EGL_RED_SIZE,
                    EGL10.EGL_DEPTH_SIZE,
                    EGL10.EGL_STENCIL_SIZE,
                    EGL10.EGL_CONFIG_CAVEAT,
                    EGL10.EGL_CONFIG_ID,
                    EGL10.EGL_LEVEL,
                    EGL10.EGL_MAX_PBUFFER_HEIGHT,
                    EGL10.EGL_MAX_PBUFFER_PIXELS,
                    EGL10.EGL_MAX_PBUFFER_WIDTH,
                    EGL10.EGL_NATIVE_RENDERABLE,
                    EGL10.EGL_NATIVE_VISUAL_ID,
                    EGL10.EGL_NATIVE_VISUAL_TYPE,
                    0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
                    EGL10.EGL_SAMPLES,
                    EGL10.EGL_SAMPLE_BUFFERS,
                    EGL10.EGL_SURFACE_TYPE,
                    EGL10.EGL_TRANSPARENT_TYPE,
                    EGL10.EGL_TRANSPARENT_RED_VALUE,
                    EGL10.EGL_TRANSPARENT_GREEN_VALUE,
                    EGL10.EGL_TRANSPARENT_BLUE_VALUE,
                    0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
                    0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
                    0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
                    0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
                    EGL10.EGL_LUMINANCE_SIZE,
                    EGL10.EGL_ALPHA_MASK_SIZE,
                    EGL10.EGL_COLOR_BUFFER_TYPE,
                    EGL10.EGL_RENDERABLE_TYPE,
                    0x3042 // EGL10.EGL_CONFORMANT
            };
            String[] names = {
                    "EGL_BUFFER_SIZE",
                    "EGL_ALPHA_SIZE",
                    "EGL_BLUE_SIZE",
                    "EGL_GREEN_SIZE",
                    "EGL_RED_SIZE",
                    "EGL_DEPTH_SIZE",
                    "EGL_STENCIL_SIZE",
                    "EGL_CONFIG_CAVEAT",
                    "EGL_CONFIG_ID",
                    "EGL_LEVEL",
                    "EGL_MAX_PBUFFER_HEIGHT",
                    "EGL_MAX_PBUFFER_PIXELS",
                    "EGL_MAX_PBUFFER_WIDTH",
                    "EGL_NATIVE_RENDERABLE",
                    "EGL_NATIVE_VISUAL_ID",
                    "EGL_NATIVE_VISUAL_TYPE",
                    "EGL_PRESERVED_RESOURCES",
                    "EGL_SAMPLES",
                    "EGL_SAMPLE_BUFFERS",
                    "EGL_SURFACE_TYPE",
                    "EGL_TRANSPARENT_TYPE",
                    "EGL_TRANSPARENT_RED_VALUE",
                    "EGL_TRANSPARENT_GREEN_VALUE",
                    "EGL_TRANSPARENT_BLUE_VALUE",
                    "EGL_BIND_TO_TEXTURE_RGB",
                    "EGL_BIND_TO_TEXTURE_RGBA",
                    "EGL_MIN_SWAP_INTERVAL",
                    "EGL_MAX_SWAP_INTERVAL",
                    "EGL_LUMINANCE_SIZE",
                    "EGL_ALPHA_MASK_SIZE",
                    "EGL_COLOR_BUFFER_TYPE",
                    "EGL_RENDERABLE_TYPE",
                    "EGL_CONFORMANT"
            };
            int[] value = new int[1];
            for (int i = 0; i < attributes.length; i++) {
                int attribute = attributes[i];
                String name = names[i];
                if ( egl.eglGetConfigAttrib(display, config, attribute, value)) {
                    Log.w(TAG, String.format("  %s: %d\n", name, value[0]));
                } else {
                    // Log.w(TAG, String.format("  %s: failed\n", name));
                    while (egl.eglGetError() != EGL10.EGL_SUCCESS);
                }
            }
        }

        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }

    public static class Renderer implements GLSurfaceView.Renderer {
        public float zoom;
        public float maxZoom;
        public float minZoom;
        public float mAngleX;
        public float maxAngleX;
        public float minAngleX;
        public float mAngleY;
        public float maxAngleY;
        public float minAngleY;
        public float mAngleZ;
        public int view_width;
        public int view_height= 1;
        private float gType4Delta;
        private boolean isPortrait = true;
        private int[] mTexture = new int[2];
        private Context mContext;
        private Bitmap bitmap;
        private int     bmpWidth = 1280;
        private int     bmpHeight = 720;
        private float[] mMVPMatrix = new float[16];
        private float[] mModelMatrix = new float[16];
        private float[] mViewMatrix = new float[16];
        private float[] mProjectionMatrix = new float[16];
        private float cam_scale = 0.5f;
        private Point3F cam_eye = new Point3F(0f, 1.0f, 0f);
        private Point3F cam_head = new Point3F(0f, 0f, 1.0f);
        private int gType = 6;
        public Renderer(Context ctx) {
            mContext = ctx;
        }

        public void transByPointF(PointF p) {
            //传进来的p  是手在屏幕上移动距离归一化后的数据
            Point3F x_axis = SphereMath.cross(cam_head, cam_eye).normalize();//相机的上和前相乘  得到右
            Point3F y_axis = cam_head;//y轴的方向是相机的上

            cam_eye = SphereMath.add( cam_eye, SphereMath.add(SphereMath.mul(x_axis, p.x),SphereMath.mul(y_axis, p.y)));//眼睛看向的方向更新
            if (cam_eye.y < 0.1f)
                cam_eye.y = 0.1f;
            cam_eye = cam_eye.normalize();

            double theta = Math.acos(cam_eye.z);
            double phi = Math.acos(cam_eye.x / Math.sin(theta));
            //eye更新后接着更新head
            cam_head = new Point3F(
                    (float) (Math.sin(theta - Math.PI / 2) * Math.cos(phi)),
                    (float) (Math.sin(theta - Math.PI / 2) * Math.sin(phi)),
                    (float) (Math.cos(theta - Math.PI / 2))).normalize();

//            Log.d(TAG, String.format("head [%f %f %f]  eye:[%f %f %f]",cam_head.x, cam_head.y, cam_head.z, cam_eye.x, cam_eye.y, cam_eye.z));
        }

        public void scaleByFloat(float scaleFactor) {
            cam_scale *= scaleFactor;
//            cam_scale = Math.min(11f, Math.max(0.1f, cam_scale));
        }

        public int getgType() {
            return gType;
        }

        public void setGLType(int Type) {
            this.gType = Type;
            switch (gType) {
                case 0:
                    //  第一种视图限制Y轴
                    maxAngleY = 180f;
                    minAngleY = -180f;

                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -3.0f;
                    minZoom = -5.0f;

                    mAngleX = 0f;
                    mAngleY = 29f;
                    mAngleZ = 0f;
                    zoom = -3.5f;

                    break;
                case 1:
                    //  第二种视图限制Y轴
                    minAngleY = -19f;
                    maxAngleY = minAngleY + 95f;
                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -4f;
                    minZoom = -4.5f;

                    mAngleX = 0f;
                    mAngleY = 30f;
                    mAngleZ = 0f;
                    zoom = -4.5f;

                    break;
                case 2:
                    //  第三种视图限制Y轴
                    maxAngleY = 50f;
                    minAngleY = -50f;

                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -3.2f;
                    minZoom = -5.0f;

                    mAngleX = 0f;
                    mAngleY = 29f;
                    mAngleZ = 0f;
                    zoom = -4.5f;
                    break;
                case 3:
//                    maxAngleY = 37f;
//                    minAngleY = 37f;
                    minAngleY = -18f;
                    maxAngleY = -1.7f;

                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -1.0f;
                    minZoom = -5.0f;

                    mAngleX = 0f;
                    mAngleY = -15f;
                    mAngleZ = 50f;
                    zoom = -4.5f;
                    break;
                case 4:
                    maxAngleY = 80f;
                    minAngleY = 30f;

                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -1.0f;
                    minZoom = -5.0f;

                    mAngleX = 0f;
                    mAngleY = 0f;
                    mAngleZ = 0f;
                    zoom = -4.5f;
                    break;
                case 5:
                    maxAngleY = 80f;
                    minAngleY = 30f;

                    maxAngleX = 180f;
                    minAngleX = 180f;

                    maxZoom = -1.0f;
                    minZoom = -5.0f;

                    mAngleX = 0f;
                    mAngleY = 0f;
                    mAngleZ = 0f;
                    zoom = -4.5f;
                    break;

                case 6:
                    maxAngleY = 180f;
                    minAngleY = -180f;

                    maxAngleX = 40f;
                    minAngleX = -40f;

                    maxZoom = -0.2f;
                    minZoom = -0.5f;

                    mAngleX = 0f;
                    mAngleY = 0f;
                    mAngleZ = 0f;
                    zoom = -0.5f;
                    break;
            }
        }

        public void updateImage(Bitmap bmp) {
            bitmap = bmp;
            bmpWidth = bmp.getWidth();
            bmpHeight = bmp.getHeight();
        }
        public void onDrawFrame(GL10 gl) {
            if (bitmap != null) {
                GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, bitmap, 0);
                bitmap.recycle();
                bitmap = null;
            }
            beforDraw(gl);
            GL2JNILib.step(gType, mTexture[0],mMVPMatrix);
        }
        private void genTexture(GL10 gl, Context context) {
            //生成纹理
            gl.glGenTextures(2, mTexture, 0);
            gl.glBindTexture(GL10.GL_TEXTURE_2D, mTexture[0]);
            //设置纹理映射的属性
            gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER,  GL10.GL_LINEAR);
            gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER,  GL10.GL_LINEAR);
            gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T,      GL10.GL_CLAMP_TO_EDGE);
            gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S,      GL10.GL_CLAMP_TO_EDGE);
        }

        private  void beforDraw(GL10 gl)
        {
//            gl.glFrontFace(GL10.GL_CCW);
            Matrix.setIdentityM(mModelMatrix, 0);
            Matrix.setIdentityM(mViewMatrix, 0);
            Matrix.setIdentityM(mProjectionMatrix, 0);

            Matrix.translateM(mModelMatrix,0, 0.0f,0.0f,cam_scale);
//            Matrix.scaleM(mModelMatrix,0,cam_scale,cam_scale,cam_scale);
//            Matrix.rotateM(mModelMatrix,0, 45, 0.0f,1.0f,0.0f);

            Matrix.setLookAtM(mViewMatrix, 0,
                    0, 0, 0,
                    cam_eye.x, cam_eye.y, cam_eye.z,
                    cam_head.x, cam_head.y, cam_head.z);

//            Matrix.setLookAtM(mViewMatrix, 0,
//                    0, 0, 0,   //center
//                    0.0f, 0.0f, 1.0f,//target
//                    0.0f, -1.0f, 1.0f);//upward

            float aspect = (float) view_width / view_height;

            float fovy = 90.0f;
            float top = 0.5f * ((float) Math.tan(fovy * Math.PI / 360.0));
            float bottom = -top;
            float left = bottom * aspect;
            float right = top * aspect;

            Matrix.frustumM(mProjectionMatrix, 0, left, right, bottom, top, 0.5f, 25);
//            Matrix.frustumM(mProjectionMatrix, 0, -aspect, aspect, -1, 1, 0.1f, 25f);
//            Matrix.frustumM(mProjectionMatrix, 0, -1, 1, -1, 1, 0.1f, 25);

            Log.i(TAG, "beforDraw: cam_scale"+cam_scale);
            Matrix.multiplyMM(mMVPMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
            Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mMVPMatrix, 0);

        }
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            view_width  = width;
            view_height = height;
            isPortrait  = view_width < view_height;
            gType4Delta =  (float)(view_width * 10 / view_height) / 10;
            GL2JNILib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {

            final String vertexShader = RawResourceReader.readTextFileFromRawResource(mContext, R.raw.vetext_sharder);
            final String fragmentShader = RawResourceReader.readTextFileFromRawResource(mContext, R.raw.fragment_sharder);
            GL2JNILib.initBuffer(view_width,view_height,vertexShader,fragmentShader);
            genTexture(gl, mContext);
        }
        public void releaseLib() {
            GL2JNILib.destroy();
        }
    }
}
