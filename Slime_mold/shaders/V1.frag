R"(
#version 410 core

out vec4 outColor;

uniform vec3 color;

uniform bool restart;

void main()
{
	if (restart)
		outColor = vec4(vec3(0.0), 1.0);
	else
		outColor = vec4(color, 1.0);
}
)"