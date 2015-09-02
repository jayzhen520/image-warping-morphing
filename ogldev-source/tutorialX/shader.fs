#version 330

in vec2 TexCoord0;
in vec3 position;

out vec4 FragColor;

uniform sampler2D gSampler;
uniform sampler2D gSampler2;

uniform mat3 originMatrix;
uniform mat3 targetMatrix;

uniform float originWeight;

void main()
{
    vec3 originPosition = vec3(position.xy, 1.0) * originMatrix;
    vec3 targetPosition = vec3(position.xy, 1.0) * targetMatrix;

    originPosition.y = 1.0 - originPosition.y;
    targetPosition.y = 1.0 - targetPosition.y;

    vec4 color1 = texture2D(gSampler, originPosition.xy);
    vec4 color2 = texture2D(gSampler2, targetPosition.xy);
    FragColor = color1 * originWeight + color2 * (1.0 - originWeight);
//vec4 color2 = texture2D(gSampler2, TexCoord0);
    //FragColor = color1;
 
}