#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;
uniform bool parallax;
uniform bool parallaxslow;
void main()
{
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f); 
    vec2 texture = vertex.zw;
    if(chaos)
    {
        float strength = 0.3;
        vec2 pos = vec2(texture.x - .8 * time /6, texture.y);        
        TexCoords = pos;
    }
    else if(confuse) // scrolling background parallax
    {
       TexCoords = vec2(texture.x- .8 * time/6, texture.y );
    }
    else if(parallax)
    {
       TexCoords = vec2(texture.x - .8 * time/6, texture.y );
    }
    else if(parallaxslow)
    {
       TexCoords = vec2(texture.x- .8 * time/6, texture.y);
    }
    else
    {
        TexCoords = texture;
    }
    if (shake)
    {
        float strength = 0.01;
        gl_Position.x += cos(time * 10) * strength;        
        gl_Position.y += cos(time * 15) * strength;        
    }
}