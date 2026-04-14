#version 300 es
precision mediump float;
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 matModel;
uniform mat4 mvp;
uniform vec3 lightDir;
uniform float ambientR;
uniform float ambientG;
uniform float ambientB;
uniform float bright;

out vec2 fragTexCoord;
out float fragShadeR;
out float fragShadeG;
out float fragShadeB;

void main(){
    fragTexCoord = vertexTexCoord;
    vec3 nLightDir = normalize(lightDir);
    mat3 oopssorry = mat3(matModel);
    oopssorry[0] = normalize(oopssorry[0]);
    oopssorry[1] = normalize(oopssorry[1]);
    oopssorry[2] = normalize(oopssorry[2]);
    vec3 thenewnorm = oopssorry * vertexNormal;
    //fragShade = max(dot(thenewnorm,nLightDir)*.2+.9,0);
    float absdot = dot(thenewnorm,nLightDir);
    if(absdot<0){
        absdot*=-.5;
    }
    //fragShadeR = (absdot*bright+ambientR+1)/3;
    fragShadeR = (absdot*bright+ambientR+1)/2.9;
    fragShadeG = (absdot*bright+ambientG+1)/2.9;
    fragShadeB = (absdot*bright+ambientB+1)/2.9;
    gl_Position = mvp * vec4(vertexPosition,1.0);
}