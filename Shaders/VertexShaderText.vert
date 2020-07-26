#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool ThreeDtext;
out vec2 TexCoords;

void main()
{
	vec2 result = vec4(aPos.xy,-1.0f, 1.0f).xy;
	gl_Position = vec4(result, -1.0f, 1.0f);
	if(ThreeDtext)
	{
	gl_Position = projection * view  * model * vec4(aPos, 1.0);
	}
	TexCoords = aTexCoords;
};