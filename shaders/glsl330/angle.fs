#version 330
in vec2 fragTexCoord;
in float fragShadeR;
in float fragShadeG;
in float fragShadeB;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

void main(){
    vec4 texelColor = texture(texture0,fragTexCoord);
    if(texelColor.a<.4)
        discard;
    finalColor = texelColor * colDiffuse * vec4(fragShadeR,fragShadeG,fragShadeB,1.0);
}