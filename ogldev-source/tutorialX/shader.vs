#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

uniform mat4 gWVP;

out vec2 TexCoord0;
out vec3 position;

void main()
{
    /*
	gl_Position = gWVP * vec4(Position, 1.0);
    TexCoord0 = TexCoord;
	position = gl_Position.xyz/gl_Position.w;
	*/
	gl_Position = vec4(Position, 1.0);
	position = vec3(Position.xy, 1.0);
	TexCoord0 = TexCoord;
	position.xy = (position.xy + 1.0) / 2.0;
}
