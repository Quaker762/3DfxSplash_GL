#version 330 core
layout (location = 0) in vec3 vertex_position;

uniform mat4 mat_projection;
uniform mat4 mat_view;
uniform mat4 mat_model;

void main()
{
    mat4 mvp = mat_projection * mat_view * mat_model;

    gl_Position = mvp * vec4(vertex_position, 1.0);
}