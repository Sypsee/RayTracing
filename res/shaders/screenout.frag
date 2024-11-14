#version 430 core
out vec4 FragColor;
	
in vec2 uv;

uniform sampler2D tex;

void main()
{
	vec3 finalColor = texture(tex, uv).rgb;

    FragColor = vec4(finalColor, 1.0);
}