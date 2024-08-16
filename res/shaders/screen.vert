#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

void main()
{
    FragPos = aPos * 0.5 + 0.5;
    FragPos.x *= 600;
    FragPos.y *= 600;

    gl_Position = vec4(aPos, 1.0);
}  