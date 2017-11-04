attribute vec4 vPosition;
attribute vec2 vTexCoordinate;
varying   vec2 v_texCoord;
varying   vec4 v_texPosition;
uniform   mat4 u_MVPMatrix;
void main()
{
    v_texCoord = vTexCoordinate;
    v_texPosition = vPosition;
//    gl_Position =  vPosition * u_MVPMatrix;
    gl_Position =    u_MVPMatrix*vPosition;
//    gl_Position = vPosition;
}