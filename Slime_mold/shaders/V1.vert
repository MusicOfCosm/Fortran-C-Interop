R"(
#version 410 core

//layout(location = 0) in vec2 aModelPos;
//layout(location = 1) in vec3 aPos;
layout(location = 0) in vec3 aPos;

uniform mat4 projection;

//out vec3 color;

void main()
{
	//gl_Position = projection * model * vec4(aModelPos + aPos.xy, 0.0, 1.0);
	gl_Position = projection * vec4(aPos.xy, 0.0, 1.0);
}
)"