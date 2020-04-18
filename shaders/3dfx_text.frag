#version 330 core

struct light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// IN VARIABLES
in vec3 vertex_color;   // Vertex color from the vertex shader
in vec3 normal;         // Transformed normal calculated in the vertex shader

// OUT VARIABLES
out vec4 frag_color;    // The output color of this fragment

void main()
{
    frag_color = vec4(vertex_color, 1.0);
}