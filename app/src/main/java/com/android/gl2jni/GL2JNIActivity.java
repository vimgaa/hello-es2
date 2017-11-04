/*
 * Copyright (C) 2007 The Android Open Source Project
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

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;


public class GL2JNIActivity extends Activity {

    GL2JNIView mView;
    Toast   mToast;
    private CountDownTimer autoCount;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GL2JNIView(getApplication());
	setContentView(mView);
        mView.setOnTouchListener(onTouchListener);

//        WindowManager wm = this.getWindowManager();

//        DisplayMetrics outMetrics = new DisplayMetrics();
//        wm.getDefaultDisplay().getMetrics(outMetrics);

//        Rect frame = new Rect();
//        this.getWindow().getDecorView().getWindowVisibleDisplayFrame(frame);
//
//        int height  = outMetrics.heightPixels + frame.top;
//        float persent = height *0.618f;
//
//        if (outMetrics.heightPixels < outMetrics.widthPixels) {
//            persent = height;
//        }
//        ViewGroup.LayoutParams vp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, (int) persent);
//
//        this.addContentView(mView, vp);

        Button myButton = new Button(mView.getContext());
        this.addContentView(myButton, new ActionBar.LayoutParams(ActionBar.LayoutParams.WRAP_CONTENT ,
                ActionBar.LayoutParams.WRAP_CONTENT));
        myButton.setText("变!!!");
        myButton.setBackgroundColor(Color.parseColor("#11F5F5DC"));
        myButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int type = mView.renderer.getgType();

                if (++type == 7) type = 0;

                getBitmap(type);

                countDownTimer.cancel();
                mView.renderer.setGLType(type);
            }
        });
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        getBitmap(mView.renderer.getgType());
        getBitmap(1);

        mView.onResume();
        mView.renderer.setGLType(mView.renderer.getgType());

        autoCount = new CountDownTimer(Integer.MAX_VALUE, 10) {
            @Override
            public void onTick(long millisUntilFinished) {


                float xAngle = mView.renderer.mAngleX + mXSpeed/100.0f;
                if (mView.renderer.maxAngleX > mView.renderer.minAngleX) {
                    if (xAngle > mView.renderer.maxAngleX) {
                        xAngle = mView.renderer.maxAngleX;
                        if (mView.renderer.getgType() == 6)
                            mXSpeed *= -1;
                    } else if (xAngle < mView.renderer.minAngleX) {
                        xAngle = mView.renderer.minAngleX;
                        if (mView.renderer.getgType() == 6)
                            mXSpeed *= -1;
                    }
                }
                mView.renderer.mAngleX = xAngle;
            }

            @Override
            public void onFinish() {

            }
        };
        autoCount.start();
    }


    @Override
    protected void onDestroy() {
        mView.renderer.releaseLib();
        super.onDestroy();
    }
    private Bitmap loadBitmap(Context context, int resourceId) {
        InputStream is = context.getResources().openRawResource(resourceId);
        Bitmap bitmap = null;
        try {

            // 利用BitmapFactory生成Bitmap
            bitmap = BitmapFactory.decodeStream(is);
        } finally {
            try {

                // 关闭流
                is.close();
                is = null;
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
        return bitmap;

    }
    public void getBitmap(int type) {

//        Bitmap bitmap = loadBitmap(GL2JNIActivity.this, type != 6 ? R.drawable.image : R.drawable.bg);
//        Bitmap bitmap = loadBitmap(GL2JNIActivity.this, type != 6 ? R.drawable.xm343 : R.drawable.bg);

        Bitmap bitmap = loadBitmap(GL2JNIActivity.this, R.drawable.xm3);
        if (bitmap != null) {
            mView.renderer.updateImage(bitmap);
        }

        if (mToast != null)
            mToast.cancel();

        switch (type) {
            case 0:
                mToast = Toast.makeText(this, "0", Toast.LENGTH_SHORT);
                break;

            case 1:
                mToast = Toast.makeText(this, "1", Toast.LENGTH_SHORT);
                break;

            case 2:
                mToast = Toast.makeText(this, "2", Toast.LENGTH_SHORT);
                break;

            case 3:
                mToast = Toast.makeText(this, "3", Toast.LENGTH_SHORT);
                break;

            case 4:
                mToast = Toast.makeText(this, "4", Toast.LENGTH_SHORT);
                break;

            case 5:
                mToast = Toast.makeText(this, "5", Toast.LENGTH_SHORT);
                break;

            case 6:
                mToast = Toast.makeText(this, "6", Toast.LENGTH_SHORT);
                break;

            default:
                return;

        }
//        mXSpeed = type == 6 ? 10 : 5;
        mToast.show();
    }

    private View.OnTouchListener onTouchListener = new View.OnTouchListener() {
        float lastX, lastY;

        private int mode = 0; // 触控点的个数

        float oldDist = 0;
        private VelocityTracker vTracker ;

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            mView.mScaleDetector.onTouchEvent(event);
            switch (event.getAction() & MotionEvent.ACTION_MASK) {
                case MotionEvent.ACTION_DOWN:
                    countDownTimer.cancel();
                    //初始化速度检测器
                    if(vTracker == null){
                        vTracker = VelocityTracker.obtain();
                    }else{
                        vTracker.clear();
                    }
                    vTracker.addMovement(event);

                    mode = 1;
                    lastX = event.getRawX();
                    lastY = event.getRawY();
                    break;

                case MotionEvent.ACTION_POINTER_DOWN:
                    mode += 1;

                    oldDist = caluDist(event);

                    break;

                case MotionEvent.ACTION_POINTER_UP:
                    mode -= 1;
                    break;

                case MotionEvent.ACTION_UP:
//                case MotionEvent.ACTION_CANCEL:
                    if (vTracker != null) {
                        if (Math.abs(vTracker.getXVelocity())  > 1000) {
                            mDelta  = 2;
                            mXSpeed = (vTracker.getXVelocity() < 0 ? -1 : 1) * (mView.renderer.getgType() == 6 ? 5 : 10);
                            countDownTimer.start();
                        }
                        vTracker.recycle();
                        vTracker = null;
                    }
                    mode = 0;
                    break;

                case MotionEvent.ACTION_MOVE:
                    vTracker.addMovement(event);
                    vTracker.computeCurrentVelocity(1000);
                    if (mode >= 2) {
                        float newDist = caluDist(event);
                        if (Math.abs(newDist - oldDist) > 2f) {
                            zoom(newDist, oldDist);
                        }
                    } else {
                        float dx = event.getRawX() - lastX;
                        float dy = event.getRawY() - lastY;
                        //TODO
                        float width =mView.renderer.view_width;
                        float height=mView.renderer.view_height;
                        float a = 160.0f / 320;
                        mView.renderer.transByPointF(new PointF(dx / width*2,dy / height *2));
//                        mView.requestRender();
                        float xAngle = mView.renderer.mAngleX + dx * a;
                        if (mView.renderer.maxAngleX  > mView.renderer.minAngleX) {
                            if (xAngle > mView.renderer.maxAngleX) {
                                xAngle = mView.renderer.maxAngleX;
                            } else if (xAngle < mView.renderer.minAngleX) {
                                xAngle = mView.renderer.minAngleX;
                            }
                        }
                        mView.renderer.mAngleX = xAngle;

                        float yAngle = mView.renderer.mAngleY - dy * a / 10;
                        if (mView.renderer.maxAngleY > mView.renderer.minAngleY) {
                            if (yAngle > mView.renderer.maxAngleY) {
                                yAngle = mView.renderer.maxAngleY;
                            } else if (yAngle < mView.renderer.minAngleY) {
                                yAngle = mView.renderer.minAngleY;
                            }
                        }
                        mView.renderer.mAngleY = yAngle;
                    }
                    break;
            }

            lastX = (int) event.getRawX();
            lastY = (int) event.getRawY();
            return true;
        }
    };

    public void zoom(float newDist, float oldDist) {
        DisplayMetrics displayMetrics = getResources().getDisplayMetrics();

        float px = displayMetrics.widthPixels;
        float py = displayMetrics.heightPixels;

        float zoomValue = mView.renderer.zoom;

        zoomValue += (newDist - oldDist) * ( mView.renderer.maxZoom -  mView.renderer.minZoom) / Math.sqrt(px * px + py * py) / 4;
        if ( zoomValue >  mView.renderer.maxZoom) {
            zoomValue =  mView.renderer.maxZoom;
        } else if ( zoomValue <  mView.renderer.minZoom) {
            zoomValue =  mView.renderer.minZoom;
        }
        mView.renderer.zoom = zoomValue;
    }

    public float caluDist(MotionEvent event) {
        float dx = event.getX(0) - event.getX(1);
        float dy = event.getY(0) - event.getY(1);
        return (float)Math.sqrt((double)(dx * dx + dy * dy));
    }

    private float mXSpeed = 10;
    private float mDelta = 2;
    public CountDownTimer countDownTimer =  new CountDownTimer(3 * 1000, 25) {

        @Override
        public void onTick(long millisUntilFinished) {

            float xAngle = mView.renderer.mAngleX + mXSpeed / (float)Math.sqrt(mDelta);

            if (mView.renderer.maxAngleX  > mView.renderer.minAngleX) {
                if (xAngle > mView.renderer.maxAngleX) {
                    xAngle = mView.renderer.maxAngleX;
                } else if (xAngle < mView.renderer.minAngleX) {
                    xAngle = mView.renderer.minAngleX;
                }
            }
            mView.renderer.mAngleX = xAngle;
            mDelta++;
        }

        @Override
        public void onFinish() {
        }
    };
}
