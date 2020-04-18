#version 330 core
layout (location = 0) in vec3 vertex_data;
layout (location = 1) in vec3 normal_data;
layout (location = 2) in vec2 texcoord_data;
layout (location = 3) in vec3 color_data;

uniform mat4 mat_mvp;
out vec3 vertex_color;

void main()
{
    vertex_color = color_data;
    gl_Position = mat_mvp * vec4(vertex_data, 1.0);
}
