#extension GL_OES_EGL_image_external : require
precision mediump float;
varying   vec2 v_texCoord;
varying   vec4 v_texPosition;
uniform sampler2D u_texture;
void main() {
   if(v_texPosition.x>0.8 || v_texPosition.x<-0.8)
    {
    gl_FragColor = texture2D(u_texture, v_texCoord);
//        gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);
    }
    else
     {
        gl_FragColor = texture2D(u_texture, v_texCoord);
     }
}