#version 330 core
layout (location = 0) in vec3 vertex_data;
layout (location = 1) in vec3 normal_data;
layout (location = 2) in vec2 texcoord_data;
layout (location = 3) in vec3 color_data;

struct light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Uniforms
uniform mat4 mat_projection;    // Projection matrix
uniform mat4 mat_view;          // View matrix
uniform mat4 mat_model;         // Model matrix

// Out variables
out vec3 vertex_color;          // Color of this vertex
out vec3 normal;                // Translated normal

void main()
{
    mat4 mat_mvp = mat_projection * mat_view * mat_model;
    normal = mat3(transpose(inverse(mat_model))) * normal_data;

    vertex_color = color_data;
    gl_Position = mat_mvp * vec4(vertex_data, 1.0);
}
