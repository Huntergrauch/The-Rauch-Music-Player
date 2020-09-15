#version 330 core
layout (location = 0) in vec3 aPos;

void main()
{
	vec2 result = vec4(aPos.xy,-1.0f, 1.0f).xy;
	gl_Position = vec4(result, -1.0f, 1.0f);
};