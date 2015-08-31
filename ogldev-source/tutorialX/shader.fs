#version 330

in vec2 TexCoord0;

out vec4 FragColor;

uniform sampler2D gSampler;
uniform sampler2D gSampler2;

void main()
{
    vec4 color1 = texture2D(gSampler, TexCoord0);
    vec4 color2 = texture2D(gSampler2, TexCoord0);
    FragColor = (color1 + color2) / 2.0;
}