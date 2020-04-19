#version 330 core

// In variables
in vec3 frag_vertex;                // Transformed vertex in eye space
in vec3 frag_vertex_color;          // Color of this vertex
in vec3 frag_normal;                // Translated normal
in vec4 color;

// Out variables
out vec4 frag_color;                // The output color of this fragment

vec3 light_position = vec3(5.0f, 300.0f, -1500.0f);
vec3 light = vec3(-0.57735f, -0.57735f, -0.57735f);
vec3 light_color = vec3(1.0, 1.0, 1.0);
// Pass 1 is already the ambient based on the normals
void main()
{
    vec3 light_direction = normalize(light_position - frag_vertex);
    float diff = max(dot(frag_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    float factor = ((light.x * frag_normal.x + light.y * frag_normal.y + light.z * frag_normal.z));


    frag_color = vec4(frag_vertex_color, 1.0);
}