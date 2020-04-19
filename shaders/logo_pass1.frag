#version 330 core

// In variables
in vec3 frag_vertex;                // Transformed vertex in eye space
in vec3 frag_vertex_color;          // Color of this vertex
in vec3 frag_normal;                // Translated normal
flat in int frag_material;

// Out variables
out vec4 frag_color;                // The output color of this fragment

// Uniform variables
vec3 light = vec3(-0.57735f, -0.57735f, -0.57735f);
void main()
{
    float factor = ((light.x * frag_normal.x + light.y * frag_normal.y + light.z * frag_normal.z) + 0.003) * 0.5;
    frag_color = vec4(factor * frag_vertex_color , 1.0);
}