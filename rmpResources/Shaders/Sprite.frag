#version 330 core
out vec4 color;
in vec2 TexCoords;
uniform sampler2D sprite;

void main()
{    
    vec4 sprite = texture(sprite, TexCoords);
    color = sprite;
    //color = vec4(1.0,1.0,0.0,1.0);
}  