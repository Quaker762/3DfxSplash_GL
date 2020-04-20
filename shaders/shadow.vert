#version 330 core
layout (location = 0) in vec3 vertex_data;

// Uniforms
uniform mat4 mat_projection;    // Projection matrix
uniform mat4 mat_view;          // View matrix
uniform mat4 mat_model;         // Model matrix

void main()
{
    mat4 mat_mvp = mat_projection * mat_view * mat_model;
    gl_Position = mat_mvp * vec4(vertex_data, 1.0);
}
