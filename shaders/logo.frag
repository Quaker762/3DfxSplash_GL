#version 330 core

// In variables
in vec3 frag_vertex;                // Transformed vertex in eye space
in vec4 frag_vertex_lightspace;
in vec3 frag_vertex_color;          // Color of this vertex
in vec3 frag_normal;                // Translated normal
flat in int frag_material;

// Out variables
out vec4 frag_color;                // The output color of this fragment

// Uniform variables
uniform vec3 light0_position;
uniform vec3 light1_position;
uniform sampler2D shadow_map;

// Locals
vec3 light = vec3(-0.57735f, -0.57735f, -0.57735f);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float ambient_strength = 0.3;
float specularStrength = 0.6;
// Pass 1 is already the ambient based on the normals

vec3 calculate_diffuse(vec3 normal, vec3 light_position, vec3 light_col)
{
    vec3 ret;

    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(light_position - frag_vertex);
    float diff = max(dot(norm, light_direction), 0.0);

    ret = diff * light_col;
    return ret;
}

float shadow(vec4 frag_pos_lightspace)
{
    vec3 proj_coords = frag_pos_lightspace.xyz / frag_pos_lightspace.z;
    proj_coords = proj_coords * 0.5 + 0.5;
    float closestDepth = texture(shadow_map, proj_coords.xy).r; 
    float currentDepth = proj_coords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    // Ambient lighting
    vec3 ambient = ambient_strength * light_color;
    vec3 diffuse = vec3(0, 0, 0);

    diffuse += calculate_diffuse(frag_normal, light0_position, light_color);

    float factor = ((light.x * frag_normal.x + light.y * frag_normal.y + light.z * frag_normal.z) + 0.4);


    // Only apply specular to the `3D`
    // This doens't work!
    if(frag_material == 0)
    {
        diffuse += calculate_diffuse(frag_normal, light1_position, light_color) * 0.5;
        frag_color = vec4((ambient + diffuse) * 0.8 * frag_vertex_color, 1.0);
    }
    else
    {    
        float shadow_value = shadow(frag_vertex_lightspace);
        frag_color = vec4((ambient + (1.0 - shadow_value * 0.45)) * diffuse * 0.7 * frag_vertex_color, 1.0);
    }
}
