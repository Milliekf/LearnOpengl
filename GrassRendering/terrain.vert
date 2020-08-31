#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

varying vec3 vPosition;
varying vec2 vTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vTexCoords = aTexCoords; 
	vPosition=aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}