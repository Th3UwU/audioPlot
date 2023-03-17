#version 420 core

layout (location = 0) in ivec2 position;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model * vec4(position, 0.0, 1.0);
}
