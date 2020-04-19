#version 330 core

// In variables
in vec3 frag_vertex;                // Transformed vertex in eye space
in vec3 frag_vertex_color;          // Color of this vertex
in vec3 frag_normal;                // Translated normal

// Out variables
out vec4 frag_color;                // The output color of this fragment

// Uniform variables
vec3 light_position = vec3(-0.57735f, -0.57735f, -0.57735f);
void main()
{
    if(frag_material > 0)
        frag_color = vec4(frag_vertex_color , 1.0);
    else
        framg_color = vec4(frag_vertex_color.grb , 1.0)
}